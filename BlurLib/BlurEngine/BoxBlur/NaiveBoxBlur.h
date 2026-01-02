#pragma once

#include "BoxBlurStrategy.h"

class NaiveBoxBlur : public BoxBlurStrategy {
public:
	NaiveBoxBlur() = default;
	~NaiveBoxBlur() = default;
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) override;
private:
	void blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
};
