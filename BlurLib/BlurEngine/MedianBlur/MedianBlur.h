#pragma once

#include "BlurEngine/IBlur.h"

class MedianBlur : public IBlur {
public:
	MedianBlur() = default;
	~MedianBlur() = default;
	void applyInternal(const BlurParam* blurParam) override;
};