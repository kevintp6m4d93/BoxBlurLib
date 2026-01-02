#pragma once
#include "Utils/ImageCore/ImageBuffer.h"

class BoxBlurStrategy {
public:
	virtual void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) = 0;
	virtual ~BoxBlurStrategy() = default;
};
