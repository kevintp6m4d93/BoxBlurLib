#pragma once
#include "Utils/ImageCore/ImageBuffer.h"
#include <string>

struct BlurParam {
	virtual ~BlurParam() = default;
	BlurParam(const ImageCore::ImageBuffer &srcBuffer, ImageCore::ImageBuffer &dstBuffer, int kernelSize) : 
		srcBuffer(srcBuffer), dstBuffer(dstBuffer), kernelSize(kernelSize) {}
	const ImageCore::ImageBuffer &srcBuffer;
	ImageCore::ImageBuffer &dstBuffer;
	int kernelSize;
};

class IBlur {
public:
	void Apply(const BlurParam*blurParam);
	virtual ~IBlur() = default;
private:
	virtual void applyInternal(const BlurParam* blurParam) = 0;
	virtual void checkSpecificParams(const BlurParam* blurParam) {}
	void checkCommonParams(const BlurParam* blurParam);
};