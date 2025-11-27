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
		void computeOnePixel(int x, int y, const uint8_t* srcBuffer, uint8_t* dstBuffer, int width, int height, int numChannels, int kernelSize);
		void getPixelAt(int x, int y, const uint8_t* srcBuffer, int width, int height, int numChannels, uint8_t* outPixel);
	};
}

#pragma once