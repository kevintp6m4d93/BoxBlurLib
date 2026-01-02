#pragma once
#if USE_DYNAMIC_PROGRAMMING
#include "ImageBuffer.h"
#include "BoxBlurStrategy.h"
#include "DPBoxBlurHelper.h"

class DPBoxBlur : public BoxBlurStrategy {
public:
	DPBoxBlur() {}
	~DPBoxBlur() {}
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) override;
private:
	DPBoxBlurHelper dpHelper_;
};

#endif