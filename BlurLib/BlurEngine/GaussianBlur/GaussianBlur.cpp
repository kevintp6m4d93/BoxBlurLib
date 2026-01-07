#include "GaussianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/Error/Error.h"
#include "Utils/CLBlur/CLBlur.h"
#include "Utils/Logger/Logger.h"

void GaussianBlur::applyInternal(const BlurParam* blurParam) {
	const GaussianBlurParam *gaussianBlurParam = dynamic_cast<const GaussianBlurParam*>(blurParam);
	LOG_DEBUG(
		"Gaussian blur parameters: kernelSize=" + std::to_string(gaussianBlurParam->kernelSize) +
		", sigmaX=" + std::to_string(gaussianBlurParam->sigmaX) +
		", sigmaY=" + std::to_string(gaussianBlurParam->sigmaY) +
		", src=(" + std::to_string(blurParam->srcBuffer.GetWidth()) + "x" +
		std::to_string(blurParam->srcBuffer.GetHeight()) + "x" +
		std::to_string(blurParam->srcBuffer.GetNumChannels()) + ")" +
		", dst=(" + std::to_string(blurParam->dstBuffer.GetWidth()) + "x" +
		std::to_string(blurParam->dstBuffer.GetHeight()) + "x" +
		std::to_string(blurParam->srcBuffer.GetNumChannels()) + ")"
	);
	CLGaussianBlur(
		gaussianBlurParam->srcBuffer, 
		gaussianBlurParam->dstBuffer, 
		gaussianBlurParam->kernelSize,
		gaussianBlurParam->sigmaX,
		gaussianBlurParam->sigmaY
	);
	LOG_INFO("Gaussian blur operation completed successfully");
}

void GaussianBlur::checkSpecificParams(const BlurParam* blurParam) {
	const GaussianBlurParam* gaussianBlurParam = dynamic_cast<const GaussianBlurParam*>(blurParam);
	if (gaussianBlurParam->sigmaX < 0 || gaussianBlurParam->sigmaY < 0)
		THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Invalid sigma, sigma value of Gaussian blur should >= 0");
}