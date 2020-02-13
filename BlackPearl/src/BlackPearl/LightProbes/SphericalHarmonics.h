#pragma once
#include "BlackPearl/Renderer/Material/CubeMapTexture.h"
#include "glm/glm.hpp"
/*
Reference   : This is an implementation of the method described by
			  Ravi Ramamoorthi and Pat Hanrahan in their SIGGRAPH 2001
		  paper, "An Efficient Representation for Irradiance
		  Environment Maps".

Author      : Ravi Ramamoorthi

Date        : Last modified on April 13, 2001

*/
namespace BlackPearl {
	class SphericalHarmonics
	{
	public:
		/* input: environmentMap,output SHCoeffs[9]*/
		static std::vector<std::vector<float>> UpdateCoeffs(std::shared_ptr<CubeMapTexture> environmentCubeMap);
		static float texelSolidAngle(int x,int y,unsigned int width,unsigned int height);
		static float areaElement(float x, float y);
		static void InitialCubeMapVector(unsigned int cubeMapWidth);


	//private:
		//bool s_InitialCubeMapVector;
		static std::vector<std::vector<glm::vec3> > cubeMapVecs;


	};

}



