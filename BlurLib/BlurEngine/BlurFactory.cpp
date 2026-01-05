#include "BlurFactory.h"

std::unique_ptr<IBlur> BlurFactory::Create(BlurFactory::BlurType blurType) {
	switch (blurType) {
		case BlurFactory::BlurType::Gaussian:
			return std::make_unique<GaussianBlur>();
		case BlurFactory::BlurType::Median:
			return std::make_unique<MedianBlur>();
		case BlurFactory::BlurType::Box:
			return std::make_unique<BoxBlur>(8);
		default:
			return nullptr;
	}
}