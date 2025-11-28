// BoxBlur.cpp
#include "BoxBlur.h"
#include <array>

namespace Blur {
	BoxBlur::BoxBlur(PaddingMode paddingMode) : paddingMode_(paddingMode) {}

	void BoxBlur::Apply(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height, int numChannels, int kernelSize) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				blurOnePixel(x, y, srcBuffer, dstBuffer, width, height, numChannels, kernelSize);
			}
		}
	}

	void BoxBlur::blurOnePixel(int x, int y, const uint8_t* srcBuffer, uint8_t* dstBuffer, int width, int height, int numChannels, int kernelSize) {
		std::array<float, 4> accPixels = { 0.0f, 0.0f, 0.0f, 0.0f };
		int halfKernel = kernelSize / 2;
		int rowSize = numChannels * width;
		for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
			for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
				int sample_x = x + kernel_x;
				int sample_y = y + kernel_y;
				uint8_t samplePixel[4] = { 0, 0, 0, 0 };
				getSrcPixelValue(sample_x, sample_y, srcBuffer, width, height, numChannels, samplePixel);
				for (int c = 0; c < numChannels; c++) {
					accPixels[c] += samplePixel[c];
				}
			}
		}
		int kernelArea = kernelSize * kernelSize;
		int curBufferIndex = y * rowSize + x * numChannels;
		for (int c = 0; c < numChannels; c++) {
			dstBuffer[curBufferIndex+c] = static_cast<uint8_t>(accPixels[c] / kernelArea);
		}
	}

	void BoxBlur::getSrcPixelValue(int x, int y, const uint8_t* srcBuffer, int width, int height, int numChannels, uint8_t* outPixel) {
		int clampedX = x;
		int clampedY = y;
		switch (paddingMode_) {
		case PaddingMode::Zeros:
			if (x < 0 || x >= width || y < 0 || y >= height) {
				for (int c = 0; c < numChannels; c++) {
					outPixel[c] = 0;
				}
				return;
			}
			break;
		case PaddingMode::Replicate:
			if (x < 0) clampedX = 0;
			if (x >= width) clampedX = width - 1;
			if (y < 0) clampedY = 0;
			if (y >= height) clampedY = height - 1;
			break;
		case PaddingMode::Mirror:
			if (x < 0) clampedX = -x - 1;
			if (x >= width) clampedX = 2 * width - x - 1;
			if (y < 0) clampedY = -y - 1;
			if (y >= height) clampedY = 2 * height - y - 1;
			break;
		}
		int rowSize = numChannels * width;
		int bufferIndex = clampedY * rowSize + clampedX * numChannels;
		for (int c = 0; c < numChannels; c++) {
			outPixel[c] = srcBuffer[bufferIndex + c];
		}
	}
}
