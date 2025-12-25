#pragma once

#include "IBlur.h"

class GaussianBlur : public IBlur {
public:
	GaussianBlur() = default;
	~GaussianBlur() = default;
	void applyInternal(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam* blurSpecificParam) override;
};