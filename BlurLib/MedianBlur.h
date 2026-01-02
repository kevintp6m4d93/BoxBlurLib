#pragma once

#include "IBlur.h"

class MedianBlur : public IBlur {
public:
	MedianBlur() = default;
	~MedianBlur() = default;
	void applyInternal(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam* blurSpecificParam) override;
};