#pragma once
#include "Utils/ImageCore/ImageBuffer.h"
#include <string>

struct IBlurSpecificParam {
	virtual ~IBlurSpecificParam() = default;
};

class IBlur {
public:
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam *blurSpecificParam);
	virtual ~IBlur() = default;
private:
	virtual void applyInternal(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam *blurSpecificParam) = 0;
	virtual void checkSpecificParams(const IBlurSpecificParam* blurSpecificParam) {}
	void checkCommonParams(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
};