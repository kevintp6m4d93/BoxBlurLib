#include "BoxBlur.h"
#include "ImageBuffer.h"
#include <array>
#include <cassert>

namespace Blur {
	BoxBlur::BoxBlur(PaddingMode paddingMode) : paddingMode_(paddingMode) {}

	void BoxBlur::Apply(const ImageCore::ImageBuffer &srcBuffer, ImageCore::ImageBuffer &dstBuffer, int kernelSize) {
		assert(kernelSize % 2 == 1 && kernelSize >= 1, "Kernel size must be a positive odd integer.");
		int src_height = srcBuffer.GetHeight();
		int src_width = srcBuffer.GetWidth();
		for (int y = 0; y < src_height; y++) {
			for (int x = 0; x < src_width; x++) {
				blurOnePixel(x, y, srcBuffer, dstBuffer, kernelSize);
			}
		}
	}

	void BoxBlur::blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
		int numSrcChannels = srcBuffer.GetNumChannels();
		float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
		int halfKernel = kernelSize / 2;
		for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
			for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
				int sample_x = x + kernel_x;
				int sample_y = y + kernel_y;
				uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = {0};
				samplePixelWithBoundary(sample_x, sample_y, srcBuffer, samplePixel);
				for (int c = 0; c < numSrcChannels; c++) {
					accSrcPixels[c] += samplePixel[c];
				}
			}
		}
		int kernelArea = kernelSize * kernelSize;
		uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
		for (int c = 0; c < numSrcChannels; c++) {
			avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / kernelArea);
		}
		dstBuffer.SetPixelValue(x, y, avgPixel, srcBuffer.GetPixelFormat());
	}

	void BoxBlur::samplePixelWithBoundary(int x, int y, const ImageCore::ImageBuffer& buffer, uint8_t* outPixel) {
		int clampedX = x;
		int clampedY = y;
		int width = buffer.GetWidth();
		int height = buffer.GetHeight();
		int numChannels = buffer.GetNumChannels();
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
		
		buffer.GetPixelValue(clampedX, clampedY, outPixel);
	}
}
