#include "MedianBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/OpenCV/OpenCVAdapter.h"
#include <opencv2/opencv.hpp>

void MedianBlur::applyInternal(const BlurParam* blurParam) {
	const cv::Mat srcMat = OpenCVAdapter::ToMatView(blurParam->srcBuffer);
	cv::Mat dstMat = OpenCVAdapter::ToMatView(blurParam->dstBuffer);
	cv::medianBlur(srcMat, dstMat, blurParam->kernelSize);
}