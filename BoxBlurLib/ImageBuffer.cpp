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

    void ImageBuffer::SetPixelValue(int x, int y, const uint8_t* srcPixel, PixelFormat srcPixelFmt) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("Pixel coordinates are out of bounds");
        }

        uint8_t rgba[4] = { 0, 0, 0, 255 };

        switch (srcPixelFmt) {
        case PixelFormat::RGB:
            rgba[0] = srcPixel[0];
            rgba[1] = srcPixel[1];
            rgba[2] = srcPixel[2];
            break;
        case PixelFormat::BGR:
            rgba[0] = srcPixel[2];
            rgba[1] = srcPixel[1];
            rgba[2] = srcPixel[0];
            break;
        case PixelFormat::RGBA:
            rgba[0] = srcPixel[0];
            rgba[1] = srcPixel[1];
            rgba[2] = srcPixel[2];
            rgba[3] = srcPixel[3];
            break;
        case PixelFormat::BGRA:
            rgba[0] = srcPixel[2];
            rgba[1] = srcPixel[1];
            rgba[2] = srcPixel[0];
            rgba[3] = srcPixel[3];
            break;
        }

        uint8_t* dst = data.get();
        int idx = y * stride + x * num_channels;

        switch (pixel_format) {
        case PixelFormat::RGB:
            dst[idx + 0] = rgba[0];
            dst[idx + 1] = rgba[1];
            dst[idx + 2] = rgba[2];
            break;

        case PixelFormat::BGR:
            dst[idx + 0] = rgba[2];
            dst[idx + 1] = rgba[1];
            dst[idx + 2] = rgba[0];
            break;

        case PixelFormat::RGBA:
            dst[idx + 0] = rgba[0];
            dst[idx + 1] = rgba[1];
            dst[idx + 2] = rgba[2];
            dst[idx + 3] = rgba[3];
            break;

        case PixelFormat::BGRA:
            dst[idx + 0] = rgba[2];
            dst[idx + 1] = rgba[1];
            dst[idx + 2] = rgba[0];
            dst[idx + 3] = rgba[3];
            break;
        }
    }

    void ImageBuffer::GetPixelValue(int x, int y, uint8_t* outPixel) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
			throw std::out_of_range("Pixel coordinates are out of bounds");
        }

        int rowSize = num_channels * width;
        int bufferIndex = y * rowSize + x * num_channels;
        for (int c = 0; c < num_channels; c++) {
            outPixel[c] = data.get()[bufferIndex + c];
        }
    }
}
