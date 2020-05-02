#include "pch.h"
#include "SphericalHarmonics.h"
#include "glm/glm.hpp"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	const float PI = 3.14159265359f;
	std::vector<std::vector<glm::vec3>> cubeMapFaceNormal = {
		{{0, 0, -1},{0, -1, 0},{1, 0, 0}},  // posx
		{{0, 0, 1},{0, -1, 0},{-1, 0, 0}},  // negx

		{{1, 0, 0},{0, 0, 1},{0, 1, 0}},    // posy
		{{1, 0, 0},{0, 0, -1},{0, -1, 0}},  // negy

		{{1, 0, 0},{0, -1, 0},{0, 0, 1}},   // posz
		{{-1, 0, 0},{0, -1, 0},{0, 0, -1}}  // negz

	};
	std::vector<std::vector<glm::vec3> > SphericalHarmonics::cubeMapVecs;
	bool initialCubeMapVector = false;

	float SphericalHarmonics::areaElement(float x, float y) {
		return atan2(x * y, sqrt(x * x + y * y + 1.0));
	}
	void SphericalHarmonics::InitialCubeMapVector(unsigned int cubeMapWidth)
	{
		unsigned int width = cubeMapWidth;
		// generate cube map vectors
		for (int index = 0; index < 6; index++)
		{
			std::vector<glm::vec3> faceVec;

			for (int v = 0; v < width; v++)
			{
				for (int u = 0; u < width; u++)
				{
					float fu = (2.0 * u / (width - 1.0)) - 1.0;
					float fv = (2.0 * v / (width - 1.0)) - 1.0;
					glm::vec3 vecX = cubeMapFaceNormal[index][0] * fu;
					glm::vec3 vecY = cubeMapFaceNormal[index][1] * fv;
					glm::vec3 vecZ = cubeMapFaceNormal[index][2];

					glm::vec3 res = glm::normalize(vecX + vecY + vecZ);
					faceVec.push_back(res);
				}
			}

			SphericalHarmonics::cubeMapVecs.push_back(faceVec);
		}

		initialCubeMapVector = true;
	}
	// give me a cubemap, its size and number of channels
	// and i'll give you spherical harmonics
	std::vector<std::vector<float>> SphericalHarmonics::UpdateCoeffs(std::shared_ptr<CubeMapTexture> environmentCubeMap)
	{
		std::vector<std::vector<float>> SHCoeffs(9, std::vector<float>(3,0.0));
		float size = environmentCubeMap->GetWidth() * environmentCubeMap->GetHeight() * 3;
		/*float* bufferPosX = new float[size];
		float* bufferNegX = new float[size];
		float* bufferPosY = new float[size];
		float* bufferNegY = new float[size];
		float* bufferPosZ = new float[size];
		float* bufferNegZ = new float[size];*/

		std::vector<float*> faces;
		faces.assign(6, new float[size]);
		environmentCubeMap->Bind();
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, GL_FLOAT, faces[0]);
		glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, GL_FLOAT, faces[1]);
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, GL_FLOAT, faces[2]);
		glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, GL_FLOAT, faces[3]);
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, GL_FLOAT, faces[4]);
		glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, GL_FLOAT, faces[5]);
		environmentCubeMap->UnBind();


		unsigned int width = environmentCubeMap->GetWidth();
		if (!initialCubeMapVector)
			InitialCubeMapVector(width);

		float weightAccum = 0.0f;
		for (int index = 0; index < 6; index++) {
			float* pixels = faces[index];

			for (int y = 0; y < width; y++)
			{
				for (int x = 0; x < width; x++)
				{
					glm::vec3 texelVect = SphericalHarmonics::cubeMapVecs[index][y * width + x];
					float weight = texelSolidAngle(x, y, width, width);
					// forsyths weights
					float weight1 = weight * 4.0f / 17.0f;
					float weight2 = weight * 8.0f / 17.0f;
					float weight3 = weight * 15.0f / 17.0f;
					float weight4 = weight * 5.0f / 68.0f;
					float weight5 = weight * 15.0f / 68.0f;
					float dx = texelVect[0];
					float dy = texelVect[1];
					float dz = texelVect[2];



					for (unsigned int c = 0; c < 3; c++) {
						int offset = y * width * 3 + x * 3 + c;
						float value = *(pixels + offset);
					//	value /= 255.0f;

						// indexeud by coeffiecent + color
						SHCoeffs[0][c] += value * weight1;
						SHCoeffs[1][c] += value * weight2 * dx;
						SHCoeffs[2][c] += value * weight2 * dy;
						SHCoeffs[3][c] += value * weight2 * dz;

						SHCoeffs[4][c] += value * weight3 * dx * dz;
						SHCoeffs[5][c] += value * weight3 * dz * dy;
						SHCoeffs[6][c] += value * weight3 * dy * dx;

						SHCoeffs[7][c] += value * weight4 * (3.0 * dz * dz - 1.0);
						SHCoeffs[8][c] += value * weight5 * (dx * dx - dy * dy);

						weightAccum += weight;
					}
				}
			}
			//delete pixels;
			//pixels = nullptr;
		}

		for (int i = 0; i < 9; i++) {
			SHCoeffs[i][0] *= 4.0f * PI / weightAccum;
			SHCoeffs[i][1] *= 4.0f * PI / weightAccum;
			SHCoeffs[i][2] *= 4.0f * PI / weightAccum;
		}
		//for (int i=0;i<6;i++)
		
		delete[] faces[0];
		return SHCoeffs;

	}

	float SphericalHarmonics::texelSolidAngle(int aU, int aV, unsigned int width, unsigned int height)
	{
		// transform from [0..res - 1] to [- (1 - 1 / res) .. (1 - 1 / res)]
		// ( 0.5 is for texel center addressing)
		float U = (2.0 * (aU + 0.5) / width) - 1.0;
		float V = (2.0 * (aV + 0.5) / height) - 1.0;

		// shift from a demi texel, mean 1.0 / size  with U and V in [-1..1]
		float invResolutionW = 1.0 / width;
		float invResolutionH = 1.0 / height;

		// U and V are the -1..1 texture coordinate on the current face.
		// get projected area for this texel
		float x0 = U - invResolutionW;
		float y0 = V - invResolutionH;
		float x1 = U + invResolutionW;
		float y1 = V + invResolutionH;
		float angle = areaElement(x0, y0) - areaElement(x0, y1) - areaElement(x1, y0) + areaElement(x1, y1);

		return angle;
	}


}