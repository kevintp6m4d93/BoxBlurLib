#include <opencv2/core.hpp>
#include <cassert>
#include "ImageBuffer.h"
#include "Utils/Error/Error.h"

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
        default:
			THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Unsupported pixel format");
        }

        if (w <= 0 || h <= 0) {
            THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Invalid image size");
        }

        stride = width * num_channels;
        try {
            data = std::shared_ptr<uint8_t>(
                new uint8_t[stride * height],
                std::default_delete<uint8_t[]>()
            );
        }
        catch (const std::bad_alloc&) {
			THROW_BLUR_EXCEPTION(BlurErrorCode::AllocationFailed, "Failed to allocate image buffer");
        }
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
        default:
            THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Unsupported pixel format");
        }
        data = std::shared_ptr<uint8_t>(
            (uint8_t*)mat.data,
			[](uint8_t*) {}
		);
	}

    void ImageBuffer::SetPixelValue(int x, int y, const uint8_t* srcPixel) {
		assert(srcPixel != nullptr);
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);

        uint8_t* dst = data.get();
        int idx = y * stride + x * num_channels;
        for (int c = 0; c < num_channels; c++) {
            dst[idx + c] = srcPixel[c];
		}
    }

    void ImageBuffer::GetPixelValue(int x, int y, uint8_t* outPixel) const {
        assert(outPixel != nullptr);
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);

        int bufferIndex = y * stride + x * num_channels;
        for (int c = 0; c < num_channels; c++) {
            outPixel[c] = data.get()[bufferIndex + c];
        }
    }
}
