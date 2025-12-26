#pragma once

#include "IBlur.h"

struct GaussianBlurSpecificParam : public IBlurSpecificParam {
	GaussianBlurSpecificParam(double sigmaX = 0.0, double sigmaY = 0.0)
		: sigmaX(sigmaX), sigmaY(sigmaY) {}
	double sigmaX = 0.0;
	double sigmaY = 0.0;
};

class GaussianBlur : public IBlur {
public:
	GaussianBlur() = default;
	~GaussianBlur() = default;
	void applyInternal(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam* blurSpecificParam) override;
};