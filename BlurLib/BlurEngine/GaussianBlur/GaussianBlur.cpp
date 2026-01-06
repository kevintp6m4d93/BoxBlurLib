#include "GaussianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/Error/Error.h"
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

void GaussianBlur::checkSpecificParams(const BlurParam* blurParam) {
	const GaussianBlurParam* gaussianBlurParam = dynamic_cast<const GaussianBlurParam*>(blurParam);
	if (gaussianBlurParam->sigmaX < 0 || gaussianBlurParam->sigmaY < 0)
		THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Invalid sigma, sigma value of Gaussian blur should >= 0");
}