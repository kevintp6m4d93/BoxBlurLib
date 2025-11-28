// BoxBlur.h
#pragma once
#include <cstdint>

namespace Blur {

	enum class PaddingMode {
		Zeros,
		Replicate,
		Mirror
	};

	class BoxBlur {
	public:
		BoxBlur(PaddingMode paddingMode);
		void Apply(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height, int numChannels, int kernelSize);
	private:
		PaddingMode paddingMode_;
		void blurOnePixel(int x, int y, const uint8_t* srcBuffer, uint8_t* dstBuffer, int width, int height, int numChannels, int kernelSize);
		void getSrcPixelValue(int x, int y, const uint8_t* srcBuffer, int width, int height, int numChannels, uint8_t* outPixel);
	};
}
