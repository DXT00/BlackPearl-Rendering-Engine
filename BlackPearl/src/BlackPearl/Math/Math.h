#pragma once
#include <random>
#include "glm/glm.hpp"
namespace donut::math {}
namespace dm = donut::math;

#include "basics.h"
#include "vector.h"
#include "matrix.h"
#include "affine.h"
#include "box.h"
#include "color.h"
#include "quat.h"
#include "sphere.h"
#include "frustum.h"

namespace BlackPearl {

	class Math {
	public:

		static int Rand_I();
		static unsigned int Rand_UI();
		static float Rand_F();
		static double Rand_D();
		static const float PI;
		static size_t Fit2Square(size_t size);

		template <typename T>
		static T Min(const std::vector<T>& v)
		{
			if (v.empty()) {
				return static_cast<T>(0);
			}
			T min_v(v[0]);
			for (size_t i = 1; i < v.size(); i++)
			{
				min_v = glm::min(min_v, v[i]);
			}
			return min_v;
		}

		//template <typename T>
		static float Max(const std::vector<float>& v)
		{
			if (v.empty()) {
				return static_cast<float>(0);
			}
			float max_v(v[0]);
			for (size_t i = 1; i < v.size(); i++)
			{
				max_v = glm::max<float>(max_v, v[i]);
			}
			return max_v;
		}
		template <typename T>
		static T Min(T a, T b)
		{
			return (a < b) ? a : b;
		}

		template <typename T>
		static T Max(T a, T b)
		{
			return (a > b) ? a : b;
		}

		static glm::vec3 Max(const std::vector<glm::vec3>& v)
		{
			if (v.empty()) {
				return static_cast<glm::vec3>(0);
			}
			glm::vec3 max_v(v[0]);
			for (size_t i = 1; i < v.size(); i++)
			{
				max_v = glm::max(max_v, v[i]);
			}
			return max_v;
		}

		template <class T>
		static constexpr T DivideAndRoundUp(T Dividend, T Divisor)
		{
			return (Dividend + Divisor - 1) / Divisor;
		}


	};

}