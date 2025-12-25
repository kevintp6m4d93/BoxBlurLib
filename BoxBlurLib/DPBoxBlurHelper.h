#pragma once
#if USE_DYNAMIC_PROGRAMMING
#include "ImageBuffer.h"

class DPBoxBlurHelper {
public:
	DPBoxBlurHelper() {}
	~DPBoxBlurHelper() {}
	void blurSingleRow(const ImageCore::ImageBuffer& srcBuffer, float* tmpBufferPtr, int row_index, int kernelSize);
	void blurSingleCol(const float* tmpBufferPtr, ImageCore::ImageBuffer& dstBuffer, int col_index, int kernelSize);
};
#endif