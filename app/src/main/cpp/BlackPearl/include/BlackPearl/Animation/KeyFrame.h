#pragma once
#include "BlackPearl/Animation/JointTransform.h"
namespace BlackPearl {

	class KeyFrame
	{
	public:
		KeyFrame(float timeStamp, std::vector<JointTransform> jointTransform) {
			m_TimeStamp = timeStamp;
			m_JoinTransforms = jointTransform;
		}
	private:
		float m_TimeStamp;
		std::vector<JointTransform> m_JoinTransforms;



	};
}

