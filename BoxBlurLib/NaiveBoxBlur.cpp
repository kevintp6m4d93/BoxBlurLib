#include "NaiveBoxBlur.h"

void NaiveBoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    int src_height = srcBuffer.GetHeight();
    int src_width = srcBuffer.GetWidth();
    for (int y = 0; y < src_height; y++) {
        for (int x = 0; x < src_width; x++) {
            blurOnePixel(x, y, srcBuffer, dstBuffer, kernelSize);
        }
    }
}

void NaiveBoxBlur::blurOnePixel(int x, int y, const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    int numSrcChannels = srcBuffer.GetNumChannels();
    float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
    int halfKernel = kernelSize / 2;
    int validKernelArea = kernelSize * kernelSize;
    for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
        for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
            int sample_x = x + kernel_x;
            int sample_y = y + kernel_y;
            uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
            if (sample_x < 0 || sample_x >= srcBuffer.GetWidth() ||
                sample_y < 0 || sample_y >= srcBuffer.GetHeight()) {
                validKernelArea -= 1;
                continue;
            }
            srcBuffer.GetPixelValue(sample_x, sample_y, samplePixel);
            for (int c = 0; c < numSrcChannels; c++) {
                accSrcPixels[c] += samplePixel[c];
            }
        }
    }
    uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
    for (int c = 0; c < numSrcChannels; c++) {
        avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / validKernelArea);
    }
    dstBuffer.SetPixelValue(x, y, avgPixel);
}
