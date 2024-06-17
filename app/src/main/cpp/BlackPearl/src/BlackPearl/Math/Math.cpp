#include "pch.h"
#include "Math.h"
namespace BlackPearl {

	static std::uniform_int_distribution<unsigned> uiMap;
	static std::uniform_int_distribution<signed> iMap;
	static std::uniform_real_distribution<float> fMap(0.0f, 1.0f);
	static std::uniform_real_distribution<double> dMap(0.0, 1.0);
	static std::default_random_engine engine;
	const float Math::PI = 3.14159265359f;

	int Math::Rand_I() {
		return iMap(engine);
	}

	unsigned int Math::Rand_UI() {
		return uiMap(engine);
	}

	float Math::Rand_F() {
		return fMap(engine);
	}

	double Math::Rand_D() {
		return dMap(engine);
	}

	size_t Math::Fit2Square(size_t size) {
		size_t n = static_cast<size_t>(sqrt(size));
		if (n * n < size)
			n++;
		return n;
	}

	

}
