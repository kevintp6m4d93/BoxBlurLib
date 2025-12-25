#include "OpenCVAdapter.h"
#include <opencv2/core.hpp>

cv::Mat OpenCVAdapter::ToMatView(ImageCore::ImageBuffer& buffer) {
    int height = buffer.GetHeight();
	int width = buffer.GetWidth();
	int stride = buffer.GetStride();
    uint8_t* data = buffer.GetBufferPtr();

    cv::Mat matBuffer(height, width, CV_8UC3, data, stride);
	return matBuffer;
}

cv::Mat OpenCVAdapter::ToMatView(const ImageCore::ImageBuffer& buffer) {
	int height = buffer.GetHeight();
	int width = buffer.GetWidth();
	int stride = buffer.GetStride();
	const uint8_t* data = buffer.GetBufferPtr();

	cv::Mat matBuffer(height, width, CV_8UC3, const_cast<uint8_t*>(data), stride);
	return matBuffer;

}
