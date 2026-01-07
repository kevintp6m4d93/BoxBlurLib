#include "MedianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/Logger/Logger.h"
#include "Utils/CLBlur/CLBlur.h"

void MedianBlur::applyInternal(const BlurParam* blurParam) {
	CLMedianBlur(blurParam->srcBuffer, blurParam->dstBuffer, blurParam->kernelSize);
	LOG_INFO("Median blur operation completed successfully");
}