#include "GaussianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/OpenCV/OpenCVAdapter.h"
#include <opencv2/opencv.hpp>
// third party library 分離到額外的utils，CL gaussian blur，這邊完全不用改
void GaussianBlur::applyInternal(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam* blurSpecificParam) {
	const GaussianBlurSpecificParam *gaussianBlurSpecificParam = dynamic_cast<const GaussianBlurSpecificParam*>(blurSpecificParam);
	double sigmaX = 0.0;
	double sigmaY = 0.0;
	if (gaussianBlurSpecificParam) {
		sigmaX = gaussianBlurSpecificParam->sigmaX;
		sigmaY = gaussianBlurSpecificParam->sigmaY;
	}
	const cv::Mat srcMat = OpenCVAdapter::ToMatView(srcBuffer);
	cv::Mat dstMat = OpenCVAdapter::ToMatView(dstBuffer);
	cv::GaussianBlur(srcMat, dstMat, cv::Size(kernelSize, kernelSize), sigmaX, sigmaY);
}