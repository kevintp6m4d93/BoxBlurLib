#pragma once
#include "BlurEngine/BoxBlur/BoxBlur.h"
#include "BlurEngine/GaussianBlur/GaussianBlur.h"
#include "BlurEngine/MedianBlur/MedianBlur.h"

namespace BlurFactory {
	enum class BlurType {
		Gaussian,
		Median,
		Box,
	};

	std::unique_ptr<IBlur> Create(BlurType);
}