#include "IBlur.h"
#include <cassert>

void IBlur::Apply(const BlurParam* blurParam) {
	checkCommonParams(blurParam);
	checkSpecificParams(blurParam);
	applyInternal(blurParam);
}

void IBlur::checkCommonParams(const BlurParam* blurParam) {
	// TODO: Null ptr check and don't use assert
	assert(blurParam.kernelSize % 2 == 1 && blurParam.kernelSize >= 1);
	assert(blurParam.srcBuffer.GetPixelFormat() == blurParam.dstBuffer.GetPixelFormat());
}