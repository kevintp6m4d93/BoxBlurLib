#if USE_MULTI_THREAD && USE_DYNAMIC_PROGRAMMING
#include "MTDPBoxBlur.h"
#include <vector>

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