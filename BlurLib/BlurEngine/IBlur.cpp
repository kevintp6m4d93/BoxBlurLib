#include "IBlur.h"
#include <cassert>

void IBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
	checkCommonParams(srcBuffer, dstBuffer, kernelSize);
	applyInternal(srcBuffer, dstBuffer, kernelSize, nullptr);
}

void IBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam *blurSpecificParam) {
	checkCommonParams(srcBuffer, dstBuffer, kernelSize);
	checkSpecificParams(blurSpecificParam);
	applyInternal(srcBuffer, dstBuffer, kernelSize, blurSpecificParam);
}

void IBlur::checkCommonParams(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
	// TODO: Null ptr check and don't use assert
	assert(kernelSize % 2 == 1 && kernelSize >= 1);
	assert(srcBuffer.GetPixelFormat() == dstBuffer.GetPixelFormat());
}