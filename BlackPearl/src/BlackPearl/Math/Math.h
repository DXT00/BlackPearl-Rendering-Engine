#pragma once
#include <random>
#include "glm/glm.hpp"
namespace BlackPearl {

	class Math {
	public:

		static int Rand_I();
		static unsigned int Rand_UI();
		static float Rand_F();
		static double Rand_D();
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

		template <typename T>
		static T Max(const std::vector<T>& v)
		{
			if (v.empty()) {
				return static_cast<T>(0);
			}
			T max_v(v[0]);
			for (size_t i = 1; i < v.size(); i++)
			{
				max_v = glm::max(max_v, v[i]);
			}
			return max_v;
		}
	};

}