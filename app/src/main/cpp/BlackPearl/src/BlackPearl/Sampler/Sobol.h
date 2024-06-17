#pragma once

//Sobel is copied from https://github.com/AdamYuan/SparseVoxelOctree
//Sobol sequence wiki:https://en.wikipedia.org/wiki/Sobol_sequence
//https://math.stackexchange.com/questions/888490/understanding-sobol-sequences
namespace BlackPearl {
	class Sobol
	{
	public:
		Sobol(unsigned dim) : m_dim{ dim } { Reset(); }
		Sobol() = default;
		void Next(float* out);
		void Reset(unsigned dim) { m_index = 0; m_dim = dim; std::fill(m_x, m_x + dim, 0); }
		void Reset() { m_index = 0; std::fill(m_x, m_x + m_dim, 0); }
		unsigned Dim() const { return m_dim; }
	private:
		unsigned m_index = 0, m_x[10005] = {}, m_dim;
	};

}


