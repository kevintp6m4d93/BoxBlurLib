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

namespace Blur {
	class BoxBlur {
	public:
		BoxBlur(int numThreads = 0, bool forceNaive=false);
		~BoxBlur();
		void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
	private:
		int numThreads_;
		int forceNaive_;
#if USE_MULTI_THREAD
		std::unique_ptr<CThreadPool> threadPool_;
#endif
		void naiveApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		void blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);

#if USE_DYNAMIC_PROGRAMMING
		void fastApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		void blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, float* tmpBufferPtr, int row_index, int kernelSize);
		void blurSingleCol(const float* tmpBufferPtr, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize);
#if  USE_MULTI_THREAD
		void fastApplyMultiThread(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		static void rowMultiThreadWraper(void* pFunctionParam, HRESULT* phr);
		static void colMultiThreadWraper(void* pFunctionParam, HRESULT* phr);
		struct rowMultiThreadData {
			BoxBlur* pThis;
			const ImageCore::ImageBuffer* srcBuffer;
			float* tmpBufferPtr;
			int start_row_index;
			int end_row_index;
			int kernelSize;
			int* pCompleted;
			pthread_mutex_t* pMutex;
		};
		struct colMultiThreadData {
			BoxBlur* pThis;
			const float* tmpBufferPtr;
			ImageCore::ImageBuffer* dstBuffer;
			int start_col_index;
			int end_col_index;
			int kernelSize;
			int* pCompleted;
			pthread_mutex_t* pMutex;
		};
#endif
#endif
	};
}
