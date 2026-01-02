#include "MedianBlur.h"
#include "ImageBuffer.h"
#include "OpenCVAdapter.h"
#include <opencv2/opencv.hpp>

void MedianBlur::applyInternal(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, const IBlurSpecificParam* blurSpecificParam) {
	const cv::Mat srcMat = OpenCVAdapter::ToMatView(srcBuffer);
	cv::Mat dstMat = OpenCVAdapter::ToMatView(dstBuffer);
	cv::medianBlur(srcMat, dstMat, kernelSize);
}