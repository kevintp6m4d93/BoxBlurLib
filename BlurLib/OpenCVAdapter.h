#pragma once
#include "ImageBuffer.h"
namespace cv {
	class Mat;
}

class OpenCVAdapter {
public:
	static cv::Mat ToMatView(ImageCore::ImageBuffer& buffer);
	static cv::Mat ToMatView(const ImageCore::ImageBuffer& buffer);
};

