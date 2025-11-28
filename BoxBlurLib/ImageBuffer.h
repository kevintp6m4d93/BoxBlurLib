#pragma once

#include <memory>
#include <cstdint>

namespace cv {
	class Mat;
}

namespace ImageCore {

	enum class PixelFormat {
		RGB,
		BGR,
		RGBA,
		BGRA,
	};

	class ImageBuffer {
	public:
		ImageBuffer(int w, int h, PixelFormat pixel_format);
		ImageBuffer(const cv::Mat& mat);
	private:
		int width;
		int height;
		int num_channels;
		int stride;
		PixelFormat pixel_format;

		std::shared_ptr<uint8_t> data;
	};
}
