#include "ImageBuffer.h"
#include <string>

struct IBlurSpecificParam {
	virtual ~IBlurSpecificParam() = default;
};

class OpenCVAdapter {
public:
	static cv::Mat ToMatView(ImageCore::ImageBuffer& buf);
	static cv::Mat ToMatView(const ImageCore::ImageBuffer& buf);
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