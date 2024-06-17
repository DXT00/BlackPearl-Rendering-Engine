#include "pch.h"
#include "Sobol.h"
#include "SobolSequence.h"

namespace BlackPearl {

	static inline uint8_t get_first_zero_bit(unsigned x)
	{
		uint8_t c = kTable[x & 255];
		if (c != 8) return c;
		c += kTable[(x >> 8) & 255];
		if (c != 16) return c;
		c += kTable[(x >> 16) & 255];
		if (c != 24) return c;
		return c + kTable[(x >> 24) & 255];
	}

	void Sobol::Next(float* out)
	{
		uint8_t c = get_first_zero_bit(m_index++);
		std::vector<float> tmp(m_dim, 0);
		for (unsigned j = 0; j < m_dim; ++j) {
			out[j] = (float)((m_x[j] ^= kMatrices[j][c]) / 4294967296.0);
			tmp[j] = out[j];
		}
	}


}