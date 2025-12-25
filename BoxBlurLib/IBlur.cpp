#include "IBlur.h"
#include <cassert>

class OpenCVAdapter {
public:
	static cv::Mat ToMatView(ImageCore::ImageBuffer& buf);
	static cv::Mat ToMatView(const ImageCore::ImageBuffer& buf);
};

void IBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam& blurSpecificParam) {
	checkCommonParams(srcBuffer, dstBuffer, kernelSize);
	checkSpecificParams(blurSpecificParam);
	applyInternal(srcBuffer, dstBuffer, kernelSize, blurSpecificParam);
}

void IBlur::checkCommonParams(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
	// TODO: Null ptr check and don't use assert
	assert(kernelSize % 2 == 1 && kernelSize >= 1);
	assert(srcBuffer.GetPixelFormat() == dstBuffer.GetPixelFormat());
}