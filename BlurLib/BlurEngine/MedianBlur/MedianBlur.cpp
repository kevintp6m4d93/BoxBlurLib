#include "MedianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/Logger/Logger.h"
#include "Utils/CLBlur/CLBlur.h"

void MedianBlur::applyInternal(const BlurParam* blurParam) {
    LOG_DEBUG(
        "Median blur parameters: kernelSize=" + std::to_string(blurParam->kernelSize) +
        ", src=(" + std::to_string(blurParam->srcBuffer.GetWidth()) + "x" +
        std::to_string(blurParam->srcBuffer.GetHeight()) + "x" +
        std::to_string(blurParam->srcBuffer.GetNumChannels()) + ")" +
        ", dst=(" + std::to_string(blurParam->dstBuffer.GetWidth()) + "x" +
        std::to_string(blurParam->dstBuffer.GetHeight()) + "x" +
        std::to_string(blurParam->srcBuffer.GetNumChannels()) + ")"
    );
	CLMedianBlur(blurParam->srcBuffer, blurParam->dstBuffer, blurParam->kernelSize);
	LOG_INFO("Median blur operation completed successfully");
}