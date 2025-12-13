#include "BoxBlur.h"
#include "ImageBuffer.h"
#include <array>
#include <vector>
#include <cassert>

namespace Blur {
    BoxBlur::BoxBlur(PaddingMode paddingMode, int numThreads) : paddingMode_(paddingMode), numThreads_(numThreads) {
#if USE_MULTI_THREAD
        if (numThreads_ > 0) {
            threadPool_ = std::make_unique<CThreadPool>();
            threadPool_->Initialize(numThreads_);
        }
#endif
    }

    void BoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        // TODO: Null ptr check
        assert(kernelSize % 2 == 1 && kernelSize >= 1);
		assert(srcBuffer.GetPixelFormat() == dstBuffer.GetPixelFormat());
        
#if USE_MULTI_THREAD && USE_DYNAMIC_PROGRAMMING
        if (numThreads_ > 0) {
            fastApplyMultiThread(srcBuffer, dstBuffer, kernelSize);
        }
        else {
#if USE_DYNAMIC_PROGRAMMING
            fastApply(srcBuffer, dstBuffer, kernelSize);
#else
            naiveApply(srcBuffer, dstBuffer, kernelSize);
#endif
        }
#elif USE_DYNAMIC_PROGRAMMING
        fastApply(srcBuffer, dstBuffer, kernelSize);
#else
        naiveApply(srcBuffer, dstBuffer, kernelSize);
#endif
    }

    void BoxBlur::naiveApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        int src_height = srcBuffer.GetHeight();
        int src_width = srcBuffer.GetWidth();
        for (int y = 0; y < src_height; y++) {
            for (int x = 0; x < src_width; x++) {
                blurOnePixel(x, y, srcBuffer, dstBuffer, kernelSize);
            }
        }
    }

    void BoxBlur::blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        int numSrcChannels = srcBuffer.GetNumChannels();
        float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
        int halfKernel = kernelSize / 2;
        for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
            for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
                int sample_x = x + kernel_x;
                int sample_y = y + kernel_y;
                uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
                samplePixelWithBoundary(sample_x, sample_y, srcBuffer, samplePixel);
                for (int c = 0; c < numSrcChannels; c++) {
                    accSrcPixels[c] += samplePixel[c];
                }
            }
        }
        int kernelArea = kernelSize * kernelSize;
        uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        for (int c = 0; c < numSrcChannels; c++) {
            avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / kernelArea);
        }
        dstBuffer.SetPixelValue(x, y, avgPixel);
    }

#if USE_DYNAMIC_PROGRAMMING
    void BoxBlur::fastApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        int src_height = srcBuffer.GetHeight();
        int src_width = srcBuffer.GetWidth();
        int src_stride = srcBuffer.GetStride();

        std::unique_ptr<float[]> tmpBuffer = std::make_unique<float[]>(src_stride * src_height);
        for (int y = 0; y < src_height; y++) {
            blurSingleRow(srcBuffer, tmpBuffer.get(), y, kernelSize);
        }
        for (int x = 0; x < src_width; x++) {
            blurSingleCol(tmpBuffer.get(), dstBuffer, x, kernelSize);
        }
    }

    void BoxBlur::blurSingleCol(const float* tmpBufferPtr, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize) {
        int height = dstBuffer.GetHeight();
        int width = dstBuffer.GetWidth();
        int stride = dstBuffer.GetStride();
        int numChannels = dstBuffer.GetNumChannels();
        int halfKernel = kernelSize / 2;
        uint8_t* dstPtr = dstBuffer.GetBufferPtr();
        float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };

        int clampedX = clampCoordinate(col_index, width);
        int colBaseIndex = clampedX * numChannels;
        int clampedY;
        for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
            clampedY = clampCoordinate(kernel_y, height);
            int pixelStartIdx = clampedY * stride + colBaseIndex;
            for (int c = 0; c < numChannels; c++) {
                accSrcPixels[c] += tmpBufferPtr[pixelStartIdx + c];
            }
        }
        int kernelArea = kernelSize * kernelSize;
        uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        for (int c = 0; c < numChannels; c++) {
            avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / kernelArea);
        }
        dstBuffer.SetPixelValue(col_index, 0, avgPixel);

        for (int y = 1; y < height; y++) {
            int prev_y = y - halfKernel - 1;
            int new_y = y + halfKernel;
            int clampedPrevIndex = clampCoordinate(prev_y, height) * stride + colBaseIndex;
            int clampedNewIndex = clampCoordinate(new_y, height) * stride + colBaseIndex;
            int currentIndex = y * stride + colBaseIndex;

            for (int c = 0; c < numChannels; c++) {
                accSrcPixels[c] = accSrcPixels[c] - float(tmpBufferPtr[clampedPrevIndex + c]) + float(tmpBufferPtr[clampedNewIndex + c]);
                dstPtr[currentIndex + c] = static_cast<uint8_t>(accSrcPixels[c] / kernelArea);
            }
        }
    }

    void BoxBlur::blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, float* tmpBufferPtr, int row_index, int kernelSize) {
        int src_width = srcBuffer.GetWidth();
        int src_stride = srcBuffer.GetStride();
        int numSrcChannels = srcBuffer.GetNumChannels();
        int halfKernel = kernelSize / 2;
        float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };

        for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
            uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            samplePixelWithBoundary(kernel_x, row_index, srcBuffer, samplePixel);
            for (int c = 0; c < numSrcChannels; c++) {
                accSrcPixels[c] += samplePixel[c];
            }
        }
        int rowBaseIndex = row_index * src_stride;
        for (int c = 0; c < numSrcChannels; c++) {
            tmpBufferPtr[rowBaseIndex + c] = accSrcPixels[c];
        }

        for (int x = 1; x < src_width; x++) {
            int prev_x = x - halfKernel - 1;
            int new_x = x + halfKernel;
            uint8_t prevPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            uint8_t newPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            samplePixelWithBoundary(prev_x, row_index, srcBuffer, prevPixel);
            samplePixelWithBoundary(new_x, row_index, srcBuffer, newPixel);
            for (int c = 0; c < numSrcChannels; c++) {
                accSrcPixels[c] = accSrcPixels[c] - float(prevPixel[c]) + float(newPixel[c]);
                tmpBufferPtr[rowBaseIndex + (x * numSrcChannels) + c] = accSrcPixels[c];
            }
        }
    }
#endif

    int BoxBlur::clampCoordinate(int coord, int maxCoord) const {
        int clampedCoordinate = coord;
        switch (paddingMode_) {	// No need to handle padding
        case PaddingMode::Replicate:
            if (coord < 0) clampedCoordinate = 0;
            if (coord >= maxCoord) clampedCoordinate = maxCoord - 1;
            break;
        case PaddingMode::Mirror:
            if (coord < 0) clampedCoordinate = -coord - 1;
            if (coord >= maxCoord) clampedCoordinate = 2 * maxCoord - coord - 1;
            break;
        }
        return clampedCoordinate;
    }

    // Return value: what information should AP know
    void BoxBlur::samplePixelWithBoundary(int x, int y, const ImageCore::ImageBuffer& buffer, uint8_t* outPixel) {
        int clampedX = x;
        int clampedY = y;
        int width = buffer.GetWidth();
        int height = buffer.GetHeight();
        int numChannels = buffer.GetNumChannels();
        clampedX = clampCoordinate(x, width);
        clampedY = clampCoordinate(y, height);

        buffer.GetPixelValue(clampedX, clampedY, outPixel);
    }

#if USE_MULTI_THREAD && USE_DYNAMIC_PROGRAMMING
    void BoxBlur::fastApplyMultiThread(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        int src_height = srcBuffer.GetHeight();
        int src_width = srcBuffer.GetWidth();
        int src_stride = srcBuffer.GetStride();

		int numRowsPerThread = src_height / numThreads_;
		int numColsPerThread = src_width / numThreads_;

        std::unique_ptr<float[]> tmpBuffer = std::make_unique<float[]>(src_stride * src_height);
        std::vector<HRESULT> taskResults(numThreads_);
        std::vector<pthread_cond_t> taskEvents(numThreads_);
        std::vector<pthread_mutex_t> taskMutexes(numThreads_);
        std::vector<int> taskCompleted(numThreads_, 0);
		std::vector<rowMultiThreadData> rowTaskDataList(numThreads_);
        
        for (int threadIdx = 0; threadIdx < numThreads_; threadIdx++) {
			rowTaskDataList[threadIdx].pThis = this;
			rowTaskDataList[threadIdx].srcBuffer = &srcBuffer;
			rowTaskDataList[threadIdx].tmpBufferPtr = tmpBuffer.get();
			rowTaskDataList[threadIdx].start_row_index = threadIdx * numRowsPerThread;
			rowTaskDataList[threadIdx].end_row_index = (threadIdx == numThreads_ - 1) ? src_height : (threadIdx + 1) * numRowsPerThread;
            rowTaskDataList[threadIdx].kernelSize = kernelSize;
			pthread_cond_init(&taskEvents[threadIdx], nullptr);
			pthread_mutex_init(&taskMutexes[threadIdx], nullptr);
			rowTaskDataList[threadIdx].pCompleted = &taskCompleted[threadIdx];
			rowTaskDataList[threadIdx].pMutex = &taskMutexes[threadIdx];
            
            threadPool_->AssignTask(
                BoxBlur::rowMultiThreadWraper,
                static_cast<void*>(&(rowTaskDataList[threadIdx])),
                &taskResults[threadIdx],
                &taskEvents[threadIdx],
                &taskMutexes[threadIdx]
			);
        }
        
		for (int threadIdx = 0; threadIdx < numThreads_; threadIdx++) {
			pthread_mutex_lock(&taskMutexes[threadIdx]);
            while (!taskCompleted[threadIdx]) {
			    pthread_cond_wait(&taskEvents[threadIdx], &taskMutexes[threadIdx]);
            }
			pthread_mutex_unlock(&taskMutexes[threadIdx]);
        }

        for (int threadIdx = 0; threadIdx < numThreads_; threadIdx++) {
            pthread_cond_destroy(&taskEvents[threadIdx]);
            pthread_mutex_destroy(&taskMutexes[threadIdx]);
            pthread_cond_init(&taskEvents[threadIdx], nullptr);
            pthread_mutex_init(&taskMutexes[threadIdx], nullptr);
            taskCompleted[threadIdx] = 0;
        }

        std::vector<colMultiThreadData> colTaskDataList(numThreads_);
        for (int threadIdx = 0; threadIdx < numThreads_; threadIdx++) {
            colTaskDataList[threadIdx].pThis = this;
            colTaskDataList[threadIdx].tmpBufferPtr = tmpBuffer.get();
            colTaskDataList[threadIdx].dstBuffer = &dstBuffer;
            colTaskDataList[threadIdx].start_col_index = threadIdx * numColsPerThread;
            colTaskDataList[threadIdx].end_col_index = (threadIdx == numThreads_ - 1) ? src_width : (threadIdx + 1) * numColsPerThread;
            colTaskDataList[threadIdx].kernelSize = kernelSize;
            colTaskDataList[threadIdx].pCompleted = &taskCompleted[threadIdx];
            colTaskDataList[threadIdx].pMutex = &taskMutexes[threadIdx];
            
            threadPool_->AssignTask(
                BoxBlur::colMultiThreadWraper,
                static_cast<void*>(&(colTaskDataList[threadIdx])),
                &taskResults[threadIdx],
                &taskEvents[threadIdx],
                &taskMutexes[threadIdx]
            );
        }
        
        for (int threadIdx = 0; threadIdx < numThreads_; threadIdx++) {
            pthread_mutex_lock(&taskMutexes[threadIdx]);
            while (!taskCompleted[threadIdx]) {
                pthread_cond_wait(&taskEvents[threadIdx], &taskMutexes[threadIdx]);
            }
            pthread_mutex_unlock(&taskMutexes[threadIdx]);

			pthread_cond_destroy(&taskEvents[threadIdx]);
			pthread_mutex_destroy(&taskMutexes[threadIdx]);
        }
    }

    void BoxBlur::rowMultiThreadWraper(void* pFunctionParam, HRESULT* phr) {
		rowMultiThreadData* data = static_cast<rowMultiThreadData*>(pFunctionParam);
        for (int row = data->start_row_index; row < data->end_row_index; ++row) {
			data->pThis->blurSingleRow(*(data->srcBuffer), data->tmpBufferPtr, row, data->kernelSize);
        }
        
        pthread_mutex_lock(data->pMutex);
        *(data->pCompleted) = 1;
        pthread_mutex_unlock(data->pMutex);
    }

    void BoxBlur::colMultiThreadWraper(void* pFunctionParam, HRESULT* phr) {
		colMultiThreadData* data = static_cast<colMultiThreadData*>(pFunctionParam);
        for (int col = data->start_col_index; col < data->end_col_index; col++) {
			data->pThis->blurSingleCol(data->tmpBufferPtr, *(data->dstBuffer), col, data->kernelSize);
        }
        
        pthread_mutex_lock(data->pMutex);
        *(data->pCompleted) = 1;
        pthread_mutex_unlock(data->pMutex);
    }
#endif
}
