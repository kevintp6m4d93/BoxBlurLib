#pragma once
#if USE_DYNAMIC_PROGRAMMING && USE_MULTI_THREAD
#include "ImageBuffer.h"
#include "BoxBlurStrategy.h"
#include "DPBoxBlurHelper.h"

#define HAVE_STRUCT_TIMESPEC
#include "ThreadTool.h"

class MTDPBoxBlur : public BoxBlurStrategy {
public:
	MTDPBoxBlur(int numThreads);
	~MTDPBoxBlur();
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) override;
private:
	int numThreads_;
	std::unique_ptr<CThreadPool> threadPool_;
	DPBoxBlurHelper dpHelper_;
	void fastApplyMultiThread(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
	static void rowMultiThreadWraper(void* pFunctionParam, HRESULT* phr);
	static void colMultiThreadWraper(void* pFunctionParam, HRESULT* phr);
	struct rowMultiThreadData {
		MTDPBoxBlur* pThis;
		const ImageCore::ImageBuffer* srcBuffer;
		float* tmpBufferPtr;
		int start_row_index;
		int end_row_index;
		int kernelSize;
		int* pCompleted;
		pthread_mutex_t* pMutex;
	};
	struct colMultiThreadData {
		MTDPBoxBlur* pThis;
		const float* tmpBufferPtr;
		ImageCore::ImageBuffer* dstBuffer;
		int start_col_index;
		int end_col_index;
		int kernelSize;
		int* pCompleted;
		pthread_mutex_t* pMutex;
	};
};

#endif