#include "IBlur.h"
#include "Utils/Error/Error.h"
#include "Utils/Logger/Logger.h"
#include <cassert>

void IBlur::Apply(const BlurParam* blurParam) {
	try {
		checkCommonParams(blurParam);
		checkSpecificParams(blurParam);
		applyInternal(blurParam);
	}
	catch (const std::exception& e) {
		THROW_BLUR_EXCEPTION(BlurErrorCode::Unexpected, e.what());
	}
	catch (...) {
		std::string msg = "Unknown exception caught during blur operation";
		THROW_BLUR_EXCEPTION(BlurErrorCode::Unexpected, msg);
	}
}

void IBlur::checkCommonParams(const BlurParam* blurParam) {
	if (blurParam == nullptr)
		THROW_BLUR_EXCEPTION(BlurErrorCode::NullArgument, "BlurParam pointer is null");
	if (blurParam->srcBuffer.GetBufferPtr() == nullptr)
		THROW_BLUR_EXCEPTION(BlurErrorCode::NullArgument, "Source buffer points to null");
	if (blurParam->dstBuffer.GetBufferPtr() == nullptr)
		THROW_BLUR_EXCEPTION(BlurErrorCode::NullArgument, "Destination buffer points to null");
	if (blurParam->kernelSize <= 0)
		THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Invalid kernel size, kernel size should be a positive value");
	if (blurParam->kernelSize % 2 == 0)
		THROW_BLUR_EXCEPTION(BlurErrorCode::BadParams, "Invalid kernel size, kernel size should be an odd value");
	if (blurParam->srcBuffer.GetWidth() != blurParam->dstBuffer.GetWidth() ||
		blurParam->srcBuffer.GetHeight() != blurParam->dstBuffer.GetHeight())
		THROW_BLUR_EXCEPTION(BlurErrorCode::BufferMismatch, "Source and destination buffer size mismatch");
	if (blurParam->srcBuffer.GetNumChannels() != blurParam->dstBuffer.GetNumChannels())
		THROW_BLUR_EXCEPTION(BlurErrorCode::BufferMismatch, "Source and destination buffer channel mismatch");
	if (blurParam->srcBuffer.GetPixelFormat() != blurParam->dstBuffer.GetPixelFormat())
		THROW_BLUR_EXCEPTION(BlurErrorCode::BufferMismatch, "Source and destination buffer pixel format mismatch");
	if (blurParam->kernelSize > std::max(blurParam->dstBuffer.GetWidth(), blurParam->dstBuffer.GetHeight()))
		LOG_WARNING("The kernel size is greater than the image max side. This may cause extremely slow performance and unexpected results.");
}