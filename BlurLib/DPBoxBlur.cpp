#if USE_DYNAMIC_PROGRAMMING
#include "DPBoxBlur.h"

void DPBoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    int src_height = srcBuffer.GetHeight();
    int src_width = srcBuffer.GetWidth();
    int src_stride = srcBuffer.GetStride();

    std::unique_ptr<float[]> tmpBuffer = std::make_unique<float[]>(src_stride * src_height);
    for (int y = 0; y < src_height; y++) {
        dpHelper_.blurSingleRow(srcBuffer, tmpBuffer.get(), y, kernelSize);
    }
    for (int x = 0; x < src_width; x++) {
        dpHelper_.blurSingleCol(tmpBuffer.get(), dstBuffer, x, kernelSize);
    }
}

#endif