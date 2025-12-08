#include "BoxBlur.h"
#include "ImageBuffer.h"
#include <array>
#include <cassert>

namespace Blur {
    BoxBlur::BoxBlur(PaddingMode paddingMode) : paddingMode_(paddingMode) {}

    void BoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        // TODO: Null ptr check
        assert(kernelSize % 2 == 1 && kernelSize >= 1, "Kernel size must be a positive odd integer.");
        fastApply(srcBuffer, dstBuffer, kernelSize);
    }

    void BoxBlur::naiveApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        int src_height = srcBuffer.GetHeight();
        int src_width = srcBuffer.GetWidth();
        for (int y = 0; y < src_height; y++) {
            for (int x = 0; x < src_width; x++) {
                blurOnePixel(x, y, srcBuffer, dstBuffer, kernelSize);
            }
        }
    }

    void BoxBlur::blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        int numSrcChannels = srcBuffer.GetNumChannels();
        float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
        int halfKernel = kernelSize / 2;
        for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
            for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
                int sample_x = x + kernel_x;
                int sample_y = y + kernel_y;
                uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
                samplePixelWithBoundary(sample_x, sample_y, srcBuffer, samplePixel);
                for (int c = 0; c < numSrcChannels; c++) {
                    accSrcPixels[c] += samplePixel[c];
                }
            }
        }
        int kernelArea = kernelSize * kernelSize;
        uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        for (int c = 0; c < numSrcChannels; c++) {
            avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / kernelArea);
        }
        dstBuffer.SetPixelValue(x, y, avgPixel, srcBuffer.GetPixelFormat());
    }

    void BoxBlur::fastApply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
        int src_height = srcBuffer.GetHeight();
        int src_width = srcBuffer.GetWidth();
        int src_stride = srcBuffer.GetStride();

        std::shared_ptr<float> tmpBuffer(
            new float[src_stride * src_height],
            std::default_delete<float[]>()
        );
        for (int y = 0; y < src_height; y++) {
            blurSingleRow(srcBuffer, tmpBuffer, y, kernelSize);
        }
        for (int x = 0; x < src_width; x++) {
            blurSingleCol(tmpBuffer, dstBuffer, x, kernelSize);
        }
    }

    void BoxBlur::blurSingleCol(const std::shared_ptr<float>& tmpBuffer, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize) {
        int height = dstBuffer.GetHeight();
        int width = dstBuffer.GetWidth();
        int stride = dstBuffer.GetStride();
        int numChannels = dstBuffer.GetNumChannels();
        int halfKernel = kernelSize / 2;
        float* tmpPtr = tmpBuffer.get();
        uint8_t* dstPtr = dstBuffer.GetBufferPtr();
        float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };

        int clampedX = clampCoordinate(col_index, width);
        int colBaseIndex = clampedX * numChannels;
        int clampedY;
        for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
            clampedY = clampCoordinate(kernel_y, height);
            int pixelStartIdx = clampedY * stride + colBaseIndex;
            for (int c = 0; c < numChannels; c++) {
                accSrcPixels[c] += tmpPtr[pixelStartIdx + c];
            }
        }
        int kernelArea = kernelSize * kernelSize;
        uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        for (int c = 0; c < numChannels; c++) {
            avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / kernelArea);
        }
        // TODO: 之後把轉換color格式的部分拿掉，這邊就不用傳src的格式了
        dstBuffer.SetPixelValue(col_index, 0, avgPixel, dstBuffer.GetPixelFormat());

        for (int y = 1; y < height; y++) {
            int prev_y = y - halfKernel - 1;
            int new_y = y + halfKernel;
            int clampedPrevIndex = clampCoordinate(prev_y, height) * stride + colBaseIndex;
            int clampedNewIndex = clampCoordinate(new_y, height) * stride + colBaseIndex;
            int currentIndex = y * stride + colBaseIndex;

            for (int c = 0; c < numChannels; c++) {
                accSrcPixels[c] = accSrcPixels[c] - float(tmpPtr[clampedPrevIndex + c]) + float(tmpPtr[clampedNewIndex + c]);
                dstPtr[currentIndex + c] = static_cast<uint8_t>(accSrcPixels[c] / kernelArea);
            }
        }
    }

    void BoxBlur::blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, const std::shared_ptr<float>& tmpBuffer, int row_index, int kernelSize) {
        int src_width = srcBuffer.GetWidth();
        int src_stride = srcBuffer.GetStride();
        int numSrcChannels = srcBuffer.GetNumChannels();
        int halfKernel = kernelSize / 2;
        float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
        float* tmpPtr = tmpBuffer.get();

        for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
            uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            samplePixelWithBoundary(kernel_x, row_index, srcBuffer, samplePixel);
            for (int c = 0; c < numSrcChannels; c++) {
                accSrcPixels[c] += samplePixel[c];
            }
        }
        int rowBaseIndex = row_index * src_stride;
        for (int c = 0; c < numSrcChannels; c++) {
            tmpPtr[rowBaseIndex + c] = accSrcPixels[c];
        }

        for (int x = 1; x < src_width; x++) {
            int prev_x = x - halfKernel - 1;
            int new_x = x + halfKernel;
            uint8_t prevPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            uint8_t newPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            samplePixelWithBoundary(prev_x, row_index, srcBuffer, prevPixel);
            samplePixelWithBoundary(new_x, row_index, srcBuffer, newPixel);
            for (int c = 0; c < numSrcChannels; c++) {
                accSrcPixels[c] = accSrcPixels[c] - float(prevPixel[c]) + float(newPixel[c]);
                tmpPtr[rowBaseIndex + (x * numSrcChannels) + c] = accSrcPixels[c];
            }
        }
    }

    int BoxBlur::clampCoordinate(int coord, int maxCoord) const {
        int clampedCoordinate = coord;
        switch (paddingMode_) {	// No need to handle padding
        case PaddingMode::Replicate:
            if (coord < 0) clampedCoordinate = 0;
            if (coord >= maxCoord) clampedCoordinate = maxCoord - 1;
            break;
        case PaddingMode::Mirror:
            if (coord < 0) clampedCoordinate = -coord - 1;
            if (coord >= maxCoord) clampedCoordinate = 2 * maxCoord - coord - 1;
            break;
        }
        return clampedCoordinate;
    }

    // Return value: what information should AP know
    void BoxBlur::samplePixelWithBoundary(int x, int y, const ImageCore::ImageBuffer& buffer, uint8_t* outPixel) {
        int clampedX = x;
        int clampedY = y;
        int width = buffer.GetWidth();
        int height = buffer.GetHeight();
        int numChannels = buffer.GetNumChannels();
        clampedX = clampCoordinate(x, width);
        clampedY = clampCoordinate(y, height);

        buffer.GetPixelValue(clampedX, clampedY, outPixel);
    }
}
