#include "GaussianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/OpenCV/OpenCVAdapter.h"
#include <opencv2/opencv.hpp>
// third party library 分離到額外的utils，CL gaussian blur，這邊完全不用改
void GaussianBlur::applyInternal(const BlurParam* blurParam) {
	const GaussianBlurParam *gaussianBlurParam = dynamic_cast<const GaussianBlurParam*>(blurParam);
	double sigmaX = 0.0;
	double sigmaY = 0.0;
	if (gaussianBlurParam) {
		sigmaX = gaussianBlurParam->sigmaX;
		sigmaY = gaussianBlurParam->sigmaY;
	}
	const cv::Mat srcMat = OpenCVAdapter::ToMatView(gaussianBlurParam->srcBuffer);
	cv::Mat dstMat = OpenCVAdapter::ToMatView(gaussianBlurParam->dstBuffer);
	cv::GaussianBlur(srcMat, dstMat, cv::Size(gaussianBlurParam->kernelSize, gaussianBlurParam->kernelSize), sigmaX, sigmaY);
}