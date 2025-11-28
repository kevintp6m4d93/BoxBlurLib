// BoxBlur.h
#pragma once
#include <cstdint>

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
		void blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
		void samplePixelWithBoundary(int x, int y, const ImageCore::ImageBuffer& buffer, uint8_t* outPixel);
	};
}
