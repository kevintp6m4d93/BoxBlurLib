#pragma once

#include "BlurEngine/IBlur.h"

struct GaussianBlurParam : public BlurParam {
	GaussianBlurParam(
		const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, double sigmaX = 0.0, double sigmaY = 0.0)
		: BlurParam(srcBuffer, dstBuffer, kernelSize), sigmaX(sigmaX), sigmaY(sigmaY) {}
	double sigmaX = 0.0;
	double sigmaY = 0.0;
};

class GaussianBlur : public IBlur {
public:
	GaussianBlur() = default;
	~GaussianBlur() = default;
	void applyInternal(const BlurParam* blurParam) override;
	void checkSpecificParams(const BlurParam* blurParam) override;
};