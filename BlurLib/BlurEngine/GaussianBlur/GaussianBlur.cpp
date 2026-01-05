#include "GaussianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/CLBlur/CLBlur.h"

void GaussianBlur::applyInternal(const BlurParam* blurParam) {
	const GaussianBlurParam *gaussianBlurParam = dynamic_cast<const GaussianBlurParam*>(blurParam);
	CLGaussianBlur(
		gaussianBlurParam->srcBuffer, 
		gaussianBlurParam->dstBuffer, 
		gaussianBlurParam->kernelSize,
		gaussianBlurParam->sigmaX,
		gaussianBlurParam->sigmaY
	);
}