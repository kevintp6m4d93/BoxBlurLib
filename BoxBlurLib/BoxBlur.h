#pragma once
#include <cstdint>
#include <memory>

#ifndef USE_DYNAMIC_PROGRAMMING
#define USE_DYNAMIC_PROGRAMMING 1
#endif

#ifndef USE_MULTI_THREAD
#define USE_MULTI_THREAD 1
#endif

#if USE_MULTI_THREAD
#define HAVE_STRUCT_TIMESPEC
#include "ThreadTool.h"
#endif

namespace ImageCore {
	enum class PixelFormat;
	class ImageBuffer;
}

class BoxBlurStrategy {
public:
	virtual void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) = 0;
	virtual ~BoxBlurStrategy() = default;
};

class NaiveBoxBlur : public BoxBlurStrategy {
public:
	NaiveBoxBlur() {}
	~NaiveBoxBlur() {}
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) override;	
private:
	void blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
};

class DPBoxBlurHelper {
public:
	DPBoxBlurHelper() {}
	~DPBoxBlurHelper() {}
	void blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, float* tmpBufferPtr, int row_index, int kernelSize);
	void blurSingleCol(const float* tmpBufferPtr, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize);
};

class DPBoxBlur : public BoxBlurStrategy {
public:
	DPBoxBlur() {}
	~DPBoxBlur() {}
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) override;
private:
	DPBoxBlurHelper dpHelper_;
};

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

class BoxBlur {
public:
	BoxBlur(int numThreads = 0, bool forceNaive=false);
	~BoxBlur();
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
private:
	std::unique_ptr<BoxBlurStrategy> boxBlurStrategy;
};
