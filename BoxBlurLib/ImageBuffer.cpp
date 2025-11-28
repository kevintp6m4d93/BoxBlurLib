#include <opencv2/core.hpp>
#include "ImageBuffer.h"

namespace ImageCore {

    ImageBuffer::ImageBuffer(int w, int h, PixelFormat pixel_format) : width(w)
        , height(h)
        , num_channels(0)
        , stride(0)
        , pixel_format(pixel_format)
    {
        switch (pixel_format)
        {
        case PixelFormat::RGB:
        case PixelFormat::BGR:
            num_channels = 3;
            break;
        case PixelFormat::RGBA:
        case PixelFormat::BGRA:
            num_channels = 4;
            break;
        }

        stride = width * num_channels;
        data = std::shared_ptr<uint8_t>(
            new uint8_t[stride * height],
            std::default_delete<uint8_t[]>()
        );
    }

    ImageBuffer::ImageBuffer(const cv::Mat& mat)
        : width(mat.cols)
        , height(mat.rows)
        , num_channels(mat.channels())
        , stride(mat.step[0])
    {
        switch (num_channels) {
        case 3:
            pixel_format = PixelFormat::BGR;
            break;
        case 4:
            pixel_format = PixelFormat::BGRA;
            break;
        }
        data = std::shared_ptr<uint8_t>(
            (uint8_t*)mat.data,
			[](uint8_t*) {}
		);
	}
}
