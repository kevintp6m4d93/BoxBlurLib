#include <cassert>
#include "ImageBuffer.h"
#include "Utils/Error/Error.h"
#include "Utils/Logger/Logger.h"

namespace ImageCore {

    ImageBuffer::ImageBuffer(int w, int h, PixelFormat pixel_format) : width(w)
        , height(h)
        , num_channels(0)
        , stride(0)
        , pixel_format(pixel_format)
    {
        if (w <= 0 || h <= 0) {
            THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Invalid image size");
        }

        LOG_DEBUG(
            "Creating ImageBuffer: width=" + std::to_string(w) +
            ", height=" + std::to_string(h) +
            ", pixel_format=" + std::to_string(static_cast<int>(pixel_format))
		);
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

    ImageBuffer::ImageBuffer(int w, int h, int channels, int stride, PixelFormat pixel_format, uint8_t *dataPtr)
        : width(w)
        , height(h)
        , num_channels(channels)
        , stride(stride)
        , pixel_format(pixel_format)
    {
        if (w <= 0 || h <= 0) {
            THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Invalid image size");
        }
        if (!dataPtr) {
            THROW_BLUR_EXCEPTION(BlurErrorCode::NullArgument, "dataPtr is null");
        }
		if (stride != w * channels) {
            THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Stride does not match width * channel number");
        }
        switch (channels) {
            case 3:
                if (pixel_format != PixelFormat::RGB && pixel_format != PixelFormat::BGR) {
                    THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Pixel format does not match channel number");
                }
				break;
            case 4:
                if (pixel_format != PixelFormat::RGBA && pixel_format != PixelFormat::BGRA) {
                    THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Pixel format does not match channel number");
				}
                break;
			default:
				THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Unsupported channel number");
        }
            
        data = std::shared_ptr<uint8_t>(
            dataPtr,
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
