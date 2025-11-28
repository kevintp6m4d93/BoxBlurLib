#pragma once

#include <memory>
#include <cstdint>

namespace cv {
	class Mat;
}

namespace ImageCore {
	const int MAX_NUM_CHANNELS = 4;

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

		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		int GetNumChannels() const { return num_channels; }
		int GetStride() const { return stride; }
		PixelFormat GetPixelFormat() const { return pixel_format; }
		uint8_t* GetBufferPtr() const { return data.get(); }
		void GetPixelValue(int x, int y, uint8_t* outPixel) const;

		void SetPixelValue(int x, int y, const uint8_t* srcPixel, PixelFormat srcPixelFmt);

	private:
		int width;
		int height;
		int num_channels;
		int stride;
		PixelFormat pixel_format;
		std::shared_ptr<uint8_t> data;
	};
}
