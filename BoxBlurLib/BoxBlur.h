#pragma once
#include <cstdint>
#include <memory>
#define HAVE_STRUCT_TIMESPEC
#include "ThreadTool.h"

namespace ImageCore {
	enum class PixelFormat;
	class ImageBuffer;
}

namespace Blur {
	enum class PaddingMode {
		Zeros,
		Replicate,
		Mirror
	};

	class BoxBlur {
	public:
		BoxBlur(PaddingMode paddingMode, int numThreads = 0);
		void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
	private:
		int numThreads_;
		std::unique_ptr<CThreadPool> threadPool_;
		PaddingMode paddingMode_;
		void naiveApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		void blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);

		void fastApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		void blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, const std::shared_ptr<float>& tmpBuffer, int row_index, int kernelSize);
		void blurSingleCol(const std::shared_ptr<float>& tmpBuffer, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize);
		int clampCoordinate(int coord, int maxCoord) const;
		void samplePixelWithBoundary(int x, int y, const ImageCore::ImageBuffer& buffer, uint8_t* outPixel);

		void fastApplyMultiThread(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		static void rowMultiThreadWraper(void* pFunctionParam, HRESULT* phr);
		static void colMultiThreadWraper(void* pFunctionParam, HRESULT* phr);
		struct rowMultiThreadData {
			BoxBlur* pThis;
			const ImageCore::ImageBuffer* srcBuffer;
			std::shared_ptr<float> tmpBuffer;
			int start_row_index;
			int end_row_index;
			int kernelSize;
			int* pCompleted;
			pthread_mutex_t* pMutex;
		};
		struct colMultiThreadData {
			BoxBlur* pThis;
			std::shared_ptr<float> tmpBuffer;
			ImageCore::ImageBuffer* dstBuffer;
			int start_col_index;
			int end_col_index;
			int kernelSize;
			int* pCompleted;
			pthread_mutex_t* pMutex;
		};
	};
}
