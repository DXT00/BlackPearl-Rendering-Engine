#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
namespace BlackPearl {
	/*represent a joint in a skeleton*/
#define MAX_WEIGHT 12
	struct VertexBoneData {
		unsigned int jointIdx[3][4] = { 0 };
		float weights[3][4] = { 0 };
		unsigned int currentPos = 0;
	};
	struct Bone {
		Bone(unsigned int index, glm::mat4 meshToBoneTranform, glm::mat4 finalTransform)
			:index(index),meshToBoneTranform(meshToBoneTranform),finalTransform(finalTransform){}

		unsigned int index;
		//aiMatrix4x4 meshToBoneTranform;
		glm::mat4 meshToBoneTranform;
		glm::mat4 finalTransform;

	};
}

