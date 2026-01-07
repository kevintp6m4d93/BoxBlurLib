#include "OpenCVAdapter.h"
#include <opencv2/core.hpp>
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Error/Error.h"

cv::Mat OpenCVAdapter::ToMatView(ImageCore::ImageBuffer& buffer) {
    int height = buffer.GetHeight();
	int width = buffer.GetWidth();
	int stride = buffer.GetStride();
    uint8_t* data = buffer.GetBufferPtr();
	auto matType = buffer.GetNumChannels() == 3 ? CV_8UC3 : CV_8UC4;

    cv::Mat matBuffer(height, width, matType, data, stride);
	return matBuffer;
}

cv::Mat OpenCVAdapter::ToMatView(const ImageCore::ImageBuffer& buffer) {
	int height = buffer.GetHeight();
	int width = buffer.GetWidth();
	int stride = buffer.GetStride();
	const uint8_t* data = buffer.GetBufferPtr();
	auto matType = buffer.GetNumChannels() == 3 ? CV_8UC3 : CV_8UC4;

	cv::Mat matBuffer(height, width, matType, const_cast<uint8_t*>(data), stride);
	return matBuffer;
}

ImageCore::ImageBuffer OpenCVAdapter::ToImageBufferView(const cv::Mat& mat) {
	int num_channels = mat.channels();
	ImageCore::PixelFormat pixel_format;
	switch (num_channels) {
	case 3:
		pixel_format = ImageCore::PixelFormat::BGR;
		break;
	case 4:
		pixel_format = ImageCore::PixelFormat::BGRA;
		break;
	default:
		THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Unsupported channel number");
	}
	ImageCore::ImageBuffer imageBuffer(mat.cols, mat.rows, num_channels, mat.step[0], pixel_format, static_cast<uint8_t*>(mat.data));
	return imageBuffer;
}
