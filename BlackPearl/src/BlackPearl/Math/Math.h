#pragma once
#include<random>




namespace BlackPearl {

	class Math {
	public:

		static int Rand_I();
		static unsigned int Rand_UI();
		static float Rand_F();
		static double Rand_D();
		static size_t Fit2Square(size_t size);
	};
}