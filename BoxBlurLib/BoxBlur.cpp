#include "BoxBlur.h"
#include "ImageBuffer.h"
#include "NaiveBoxBlur.h"
#include "DPBoxBlur.h"
#include "MTDPBoxBlur.h"
#include <array>
#include <cassert>

BoxBlur::BoxBlur(int numThreads, bool forceNaive) {
    if (forceNaive) {
        boxBlurStrategy = std::make_unique<NaiveBoxBlur>();
    }
    else {
#if USE_DYNAMIC_PROGRAMMING
#if USE_MULTI_THREAD
        if (numThreads > 0)
            boxBlurStrategy = std::make_unique<MTDPBoxBlur>(numThreads);
        else
            boxBlurStrategy = std::make_unique<DPBoxBlur>();
#else
        boxBlurStrategy = std::make_unique<DPBoxBlur>();
#endif
#else
        boxBlurStrategy = std::make_unique<NaiveBoxBlur>();
#endif
    }
}

BoxBlur::~BoxBlur() {
}

void BoxBlur::Apply(const ImageCore::ImageBuffer& srcBuffer, ImageCore::ImageBuffer& dstBuffer, int kernelSize) {
    // TODO: Null ptr check
    assert(kernelSize % 2 == 1 && kernelSize >= 1);
	assert(srcBuffer.GetPixelFormat() == dstBuffer.GetPixelFormat());
    boxBlurStrategy->Apply(srcBuffer, dstBuffer, kernelSize);
}