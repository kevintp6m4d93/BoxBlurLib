#pragma once
#if USE_DYNAMIC_PROGRAMMING
#include "Utils/ImageCore/ImageBuffer.h"

class DPBoxBlurHelper {
public:
	DPBoxBlurHelper() = default;
	~DPBoxBlurHelper() = default;
	void blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, float* tmpBufferPtr, int row_index, int kernelSize);
	void blurSingleCol(const float* tmpBufferPtr, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize);
};
#endif