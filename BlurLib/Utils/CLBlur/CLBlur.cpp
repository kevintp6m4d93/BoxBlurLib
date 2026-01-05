#include "CLBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/OpenCV/OpenCVAdapter.h"
#include <opencv2/opencv.hpp>

void CLGaussianBlur(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, float sigmaX, float sigmaY) {
	const cv::Mat srcMat = OpenCVAdapter::ToMatView(srcBuffer);
	cv::Mat dstMat = OpenCVAdapter::ToMatView(dstBuffer);
	cv::GaussianBlur(srcMat, dstMat, cv::Size(kernelSize, kernelSize), sigmaX, sigmaY);
}

void CLMedianBlur(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
	const cv::Mat srcMat = OpenCVAdapter::ToMatView(srcBuffer);
	cv::Mat dstMat = OpenCVAdapter::ToMatView(dstBuffer);
	cv::medianBlur(srcMat, dstMat, kernelSize);
}