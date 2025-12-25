#include "BoxBlur.h"
#include "ImageBuffer.h"
#include <array>
#include <vector>
#include <cassert>

void NaiveBoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    int src_height = srcBuffer.GetHeight();
    int src_width = srcBuffer.GetWidth();
    for (int y = 0; y < src_height; y++) {
        for (int x = 0; x < src_width; x++) {
            blurOnePixel(x, y, srcBuffer, dstBuffer, kernelSize);
        }
    }
}

void NaiveBoxBlur::blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    int numSrcChannels = srcBuffer.GetNumChannels();
    float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
    int halfKernel = kernelSize / 2;
    int validKernelArea = kernelSize * kernelSize;
    for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
        for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
            int sample_x = x + kernel_x;
            int sample_y = y + kernel_y;
            uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            if (sample_x < 0 || sample_x >= srcBuffer.GetWidth() ||
                sample_y < 0 || sample_y >= srcBuffer.GetHeight()) {
                validKernelArea -= 1;
                continue;
            }
            srcBuffer.GetPixelValue(sample_x, sample_y, samplePixel);
            for (int c = 0; c < numSrcChannels; c++) {
                accSrcPixels[c] += samplePixel[c];
            }
        }
    }
    uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
    for (int c = 0; c < numSrcChannels; c++) {
        avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / validKernelArea);
    }
    dstBuffer.SetPixelValue(x, y, avgPixel);
}


#if USE_DYNAMIC_PROGRAMMING

void DPBoxBlurHelper::blurSingleCol(const float* tmpBufferPtr, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize) {
    int height = dstBuffer.GetHeight();
    int width = dstBuffer.GetWidth();
    int stride = dstBuffer.GetStride();
    int numChannels = dstBuffer.GetNumChannels();
    int halfKernel = kernelSize / 2;
    uint8_t* dstPtr = dstBuffer.GetBufferPtr();
    float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
    int numValidLeftCoords = col_index - halfKernel >= 0 ? halfKernel : col_index;
    int numValidRightCoords = col_index + halfKernel < width ? halfKernel : (width - col_index - 1);
    int numValidXCoords = numValidLeftCoords + numValidRightCoords + 1;


    int colBaseIndex = col_index * numChannels;
    for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
        if (kernel_y >= 0 && kernel_y < height) {
            int pixelStartIdx = kernel_y * stride + colBaseIndex;
            for (int c = 0; c < numChannels; c++) {
                accSrcPixels[c] += tmpBufferPtr[pixelStartIdx + c];
            }
        }
    }

    int numValidTopCoords = 0;
    int numValidBottomCoords = halfKernel < height ? halfKernel : (height - 1);
    int numValidYCoords = numValidTopCoords + numValidBottomCoords + 1;
    int validKernelArea = numValidXCoords * numValidYCoords;
    uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
    for (int c = 0; c < numChannels; c++) {
        avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / validKernelArea);
    }
    dstBuffer.SetPixelValue(col_index, 0, avgPixel);

    for (int y = 1; y < height; y++) {
        int prev_y = y - halfKernel - 1;
        int new_y = y + halfKernel;
        // auucmulate index by stride
        int prevBufferIndex = prev_y * stride + colBaseIndex;
        int newBufferIndex = new_y * stride + colBaseIndex;
        int currentIndex = y * stride + colBaseIndex;
        float prevPixelValue = 0.0;
        float newPixelValue = 0.0;

        numValidTopCoords = y - halfKernel >= 0 ? halfKernel : y;
        numValidBottomCoords = y + halfKernel < height ? halfKernel : (height - y - 1);
        numValidYCoords = numValidTopCoords + numValidBottomCoords + 1;
        validKernelArea = numValidXCoords * numValidYCoords;
        for (int c = 0; c < numChannels; c++) {
            if (prev_y >= 0 && prev_y < height)
                prevPixelValue = float(tmpBufferPtr[prevBufferIndex + c]);
            if (new_y >= 0 && new_y < height)
                newPixelValue = float(tmpBufferPtr[newBufferIndex + c]);

            accSrcPixels[c] = accSrcPixels[c] - prevPixelValue + newPixelValue;
            dstPtr[currentIndex + c] = static_cast<uint8_t>(accSrcPixels[c] / validKernelArea);
        }
    }
}

void DPBoxBlurHelper::blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, float* tmpBufferPtr, int row_index, int kernelSize) {
    int width = srcBuffer.GetWidth();
    int stride = srcBuffer.GetStride();
    int numChannels = srcBuffer.GetNumChannels();
    int halfKernel = kernelSize / 2;
    float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };

    for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
        uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        if (kernel_x >= 0 && kernel_x < width)
            srcBuffer.GetPixelValue(kernel_x, row_index, samplePixel);
        for (int c = 0; c < numChannels; c++) {
            accSrcPixels[c] += samplePixel[c];
        }
    }
    int rowBaseIndex = row_index * stride;
    for (int c = 0; c < numChannels; c++) {
        tmpBufferPtr[rowBaseIndex + c] = accSrcPixels[c];
    }

    for (int x = 1; x < width; x++) {
        int prev_x = x - halfKernel - 1;
        int new_x = x + halfKernel;
        uint8_t prevPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        uint8_t newPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        if (prev_x >= 0 && prev_x < width)
            srcBuffer.GetPixelValue(prev_x, row_index, prevPixel);
        if (new_x >= 0 && new_x < width)
            srcBuffer.GetPixelValue(new_x, row_index, newPixel);
        for (int c = 0; c < numChannels; c++) {
            accSrcPixels[c] = accSrcPixels[c] - float(prevPixel[c]) + float(newPixel[c]);
            tmpBufferPtr[rowBaseIndex + (x * numChannels) + c] = accSrcPixels[c];
        }
    }
}

void DPBoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    int src_height = srcBuffer.GetHeight();
    int src_width = srcBuffer.GetWidth();
    int src_stride = srcBuffer.GetStride();

    std::unique_ptr<float[]> tmpBuffer = std::make_unique<float[]>(src_stride * src_height);
    for (int y = 0; y < src_height; y++) {
        dpHelper_.blurSingleRow(srcBuffer, tmpBuffer.get(), y, kernelSize);
    }
    for (int x = 0; x < src_width; x++) {
        dpHelper_.blurSingleCol(tmpBuffer.get(), dstBuffer, x, kernelSize);
    }
}

#endif

#if USE_MULTI_THREAD && USE_DYNAMIC_PROGRAMMING
MTDPBoxBlur::MTDPBoxBlur(int numThreads) : numThreads_(numThreads) {
    if (numThreads_ > 0) {
        threadPool_ = std::make_unique<CThreadPool>();
        threadPool_->Initialize(numThreads_);
    }
}

MTDPBoxBlur::~MTDPBoxBlur() {
    if (threadPool_) {
        threadPool_->Uninitialize();
    }
}

void MTDPBoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
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
        rowTaskDataList[threadIdx].pCompleted = &taskCompleted[threadIdx];
        rowTaskDataList[threadIdx].pMutex = &taskMutexes[threadIdx];

        pthread_cond_init(&taskEvents[threadIdx], nullptr);
        pthread_mutex_init(&taskMutexes[threadIdx], nullptr);

        threadPool_->AssignTask(
            MTDPBoxBlur::rowMultiThreadWraper,
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


    std::vector<colMultiThreadData> colTaskDataList(numThreads_);
    for (int threadIdx = 0; threadIdx < numThreads_; threadIdx++) {
        taskCompleted[threadIdx] = 0;
        colTaskDataList[threadIdx].pThis = this;
        colTaskDataList[threadIdx].tmpBufferPtr = tmpBuffer.get();
        colTaskDataList[threadIdx].dstBuffer = &dstBuffer;
        colTaskDataList[threadIdx].start_col_index = threadIdx * numColsPerThread;
        colTaskDataList[threadIdx].end_col_index = (threadIdx == numThreads_ - 1) ? src_width : (threadIdx + 1) * numColsPerThread;
        colTaskDataList[threadIdx].kernelSize = kernelSize;
        colTaskDataList[threadIdx].pCompleted = &taskCompleted[threadIdx];
        colTaskDataList[threadIdx].pMutex = &taskMutexes[threadIdx];

        threadPool_->AssignTask(
            MTDPBoxBlur::colMultiThreadWraper,
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

void MTDPBoxBlur::rowMultiThreadWraper(void* pFunctionParam, HRESULT* phr) {
    rowMultiThreadData* data = static_cast<rowMultiThreadData*>(pFunctionParam);
    for (int row = data->start_row_index; row < data->end_row_index; ++row) {
        data->pThis->dpHelper_.blurSingleRow(*(data->srcBuffer), data->tmpBufferPtr, row, data->kernelSize);
    }

    pthread_mutex_lock(data->pMutex);
    *(data->pCompleted) = 1;
    pthread_mutex_unlock(data->pMutex);
}

void MTDPBoxBlur::colMultiThreadWraper(void* pFunctionParam, HRESULT* phr) {
    colMultiThreadData* data = static_cast<colMultiThreadData*>(pFunctionParam);
    for (int col = data->start_col_index; col < data->end_col_index; col++) {
        data->pThis->dpHelper_.blurSingleCol(data->tmpBufferPtr, *(data->dstBuffer), col, data->kernelSize);
    }

    pthread_mutex_lock(data->pMutex);
    *(data->pCompleted) = 1;
    pthread_mutex_unlock(data->pMutex);
}
#endif

BoxBlur::BoxBlur(int numThreads, bool forceNaive) {
    if (forceNaive) {
        boxBlurStrategy = std::make_unique<NaiveBoxBlur>();
    }
    else {
#if USE_DYNAMIC_PROGRAMMING
#if USE_MULTI_THREAD
        if (numThreads > 0)
            boxBlurStrategy = std::make_unique<MTDPBoxBlur>(numThreads);
        else
            boxBlurStrategy = std::make_unique<DPBoxBlur>();
#else
        boxBlurStrategy = std::make_unique<DPBoxBlur>();
#endif
#else
        boxBlurStrategy = std::make_unique<NaiveBoxBlur>();
#endif
    }
}

BoxBlur::~BoxBlur() {
}

void BoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    // TODO: Null ptr check
    assert(kernelSize % 2 == 1 && kernelSize >= 1);
	assert(srcBuffer.GetPixelFormat() == dstBuffer.GetPixelFormat());
    boxBlurStrategy->Apply(srcBuffer, dstBuffer, kernelSize);
}