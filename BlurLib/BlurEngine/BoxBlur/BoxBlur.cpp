#include "BoxBlur.h"
#include "Utils/ImageCore/ImageBuffer.h"
#include "Utils/Logger/Logger.h"
#include "NaiveBoxBlur.h"
#include "DPBoxBlur.h"
#include "MTDPBoxBlur.h"
#include <array>

BoxBlur::BoxBlur(int numThreads, bool forceNaive) {
    if (forceNaive) {
        LOG_WARNING("`forceNaive` = true, execute naive box blur implementation");
        boxBlurStrategy = std::make_unique<NaiveBoxBlur>();
    }
    else {
#if USE_DYNAMIC_PROGRAMMING
#if USE_MULTI_THREAD
        if (numThreads > 0)
            boxBlurStrategy = std::make_unique<MTDPBoxBlur>(numThreads);
        else {
            boxBlurStrategy = std::make_unique<DPBoxBlur>();
        }
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

void BoxBlur::applyInternal(const BlurParam* blurParam) {
    boxBlurStrategy->Apply(blurParam->srcBuffer, blurParam->dstBuffer, blurParam->kernelSize);
    LOG_INFO("Box blur operation completed successfully");
}