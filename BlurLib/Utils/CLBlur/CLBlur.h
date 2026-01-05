#pragma once
#include "Utils/ImageCore/ImageBuffer.h"
void CLGaussianBlur(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize, float sigmaX, float sigmaY);
void CLMedianBlur(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);