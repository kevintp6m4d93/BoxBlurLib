#pragma once
#include <cstdint>
#include <memory>
#include "BoxBlurStrategy.h"

#ifndef USE_DYNAMIC_PROGRAMMING
#define USE_DYNAMIC_PROGRAMMING 1
#endif

#ifndef USE_MULTI_THREAD
#define USE_MULTI_THREAD 1
#endif


namespace ImageCore {
	enum class PixelFormat;
	class ImageBuffer;
}

class BoxBlur {
public:
	BoxBlur(int numThreads = 0, bool forceNaive=false);
	~BoxBlur();
	void Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize);
private:
	std::unique_ptr<BoxBlurStrategy> boxBlurStrategy;
};
