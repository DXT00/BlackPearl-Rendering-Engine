#pragma once
#include "KeyFrame.h"
namespace BlackPearl {
	class Animation
	{
	public:
		Animation(std::vector<KeyFrame> keyFrame) {
			m_Frames = keyFrame;
		}

	private:
		std::vector<KeyFrame> m_Frames;

	};

}
