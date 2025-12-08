#pragma once
#include <cstdint>
#include <memory>

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
		BoxBlur(PaddingMode paddingMode);
		void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
	private:
		PaddingMode paddingMode_;
		void naiveApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		void blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);

		void fastApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		void blurSingleCol(const std::shared_ptr<float>& tmpBuffer, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize);
		void blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, const std::shared_ptr<float>& tmpBuffer, int row_index, int kernelSize);
		int clampCoordinate(int coord, int maxCoord) const;
		void samplePixelWithBoundary(int x, int y, const ImageCore::ImageBuffer& buffer, uint8_t* outPixel);
	};
}
