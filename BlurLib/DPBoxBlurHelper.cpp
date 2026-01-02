#if USE_DYNAMIC_PROGRAMMING
#include "DPBoxBlurHelper.h"

void DPBoxBlurHelper::blurSingleCol(const float* tmpBufferPtr, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize) {
    int height = dstBuffer.GetHeight();
    int width = dstBuffer.GetWidth();
    int stride = dstBuffer.GetStride();
    int numChannels = dstBuffer.GetNumChannels();
    int halfKernel = kernelSize / 2;
    uint8_t* dstPtr = dstBuffer.GetBufferPtr();
    float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };
    int numValidLeftCoords = col_index - halfKernel >= 0 ? halfKernel : col_index;
    int numValidRightCoords = col_index + halfKernel < width ? halfKernel : (width - col_index - 1);
    int numValidXCoords = numValidLeftCoords + numValidRightCoords + 1;


    int colBaseIndex = col_index * numChannels;
    for (int kernel_y = -halfKernel; kernel_y <= halfKernel; kernel_y++) {
        if (kernel_y >= 0 && kernel_y < height) {
            int pixelStartIdx = kernel_y * stride + colBaseIndex;
            for (int c = 0; c < numChannels; c++) {
                accSrcPixels[c] += tmpBufferPtr[pixelStartIdx + c];
            }
        }
    }

    int numValidTopCoords = 0;
    int numValidBottomCoords = halfKernel < height ? halfKernel : (height - 1);
    int numValidYCoords = numValidTopCoords + numValidBottomCoords + 1;
    int validKernelArea = numValidXCoords * numValidYCoords;
    uint8_t avgPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
    for (int c = 0; c < numChannels; c++) {
        avgPixel[c] = static_cast<uint8_t>(accSrcPixels[c] / validKernelArea);
    }
    dstBuffer.SetPixelValue(col_index, 0, avgPixel);

    for (int y = 1; y < height; y++) {
        int prev_y = y - halfKernel - 1;
        int new_y = y + halfKernel;
        // auucmulate index by stride
        int prevBufferIndex = prev_y * stride + colBaseIndex;
        int newBufferIndex = new_y * stride + colBaseIndex;
        int currentIndex = y * stride + colBaseIndex;
        float prevPixelValue = 0.0;
        float newPixelValue = 0.0;

        numValidTopCoords = y - halfKernel >= 0 ? halfKernel : y;
        numValidBottomCoords = y + halfKernel < height ? halfKernel : (height - y - 1);
        numValidYCoords = numValidTopCoords + numValidBottomCoords + 1;
        validKernelArea = numValidXCoords * numValidYCoords;
        for (int c = 0; c < numChannels; c++) {
            if (prev_y >= 0 && prev_y < height)
                prevPixelValue = float(tmpBufferPtr[prevBufferIndex + c]);
            if (new_y >= 0 && new_y < height)
                newPixelValue = float(tmpBufferPtr[newBufferIndex + c]);

            accSrcPixels[c] = accSrcPixels[c] - prevPixelValue + newPixelValue;
            dstPtr[currentIndex + c] = static_cast<uint8_t>(accSrcPixels[c] / validKernelArea);
        }
    }
}

void DPBoxBlurHelper::blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, float* tmpBufferPtr, int row_index, int kernelSize) {
    int width = srcBuffer.GetWidth();
    int stride = srcBuffer.GetStride();
    int numChannels = srcBuffer.GetNumChannels();
    int halfKernel = kernelSize / 2;
    float accSrcPixels[ImageCore::MAX_NUM_CHANNELS] = { 0.0 };

    for (int kernel_x = -halfKernel; kernel_x <= halfKernel; kernel_x++) {
        uint8_t samplePixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        if (kernel_x >= 0 && kernel_x < width)
            srcBuffer.GetPixelValue(kernel_x, row_index, samplePixel);
        for (int c = 0; c < numChannels; c++) {
            accSrcPixels[c] += samplePixel[c];
        }
    }
    int rowBaseIndex = row_index * stride;
    for (int c = 0; c < numChannels; c++) {
        tmpBufferPtr[rowBaseIndex + c] = accSrcPixels[c];
    }

    for (int x = 1; x < width; x++) {
        int prev_x = x - halfKernel - 1;
        int new_x = x + halfKernel;
        uint8_t prevPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        uint8_t newPixel[ImageCore::MAX_NUM_CHANNELS] = { 0 };
        if (prev_x >= 0 && prev_x < width)
            srcBuffer.GetPixelValue(prev_x, row_index, prevPixel);
        if (new_x >= 0 && new_x < width)
            srcBuffer.GetPixelValue(new_x, row_index, newPixel);
        for (int c = 0; c < numChannels; c++) {
            accSrcPixels[c] = accSrcPixels[c] - float(prevPixel[c]) + float(newPixel[c]);
            tmpBufferPtr[rowBaseIndex + (x * numChannels) + c] = accSrcPixels[c];
        }
    }
}
#endif