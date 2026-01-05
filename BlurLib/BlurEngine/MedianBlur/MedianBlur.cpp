#include "MedianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/CLBlur/CLBlur.h"

void MedianBlur::applyInternal(const BlurParam* blurParam) {
	CLMedianBlur(blurParam->srcBuffer, blurParam->dstBuffer, blurParam->kernelSize);
}