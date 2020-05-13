#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/Material/Texture3D.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Renderer/Material/TextureImage2D.h"
namespace BlackPearl {
	class VoxelConeTracingRenderer:public BasicRenderer
	{
	public:
		enum RenderingMode {
			VOXELIZATION_VISUALIZATION = 0, // Voxelization visualization.
			VOXEL_CONE_TRACING = 1			// Global illumination using voxel cone tracing.
		};
		VoxelConeTracingRenderer();
		~VoxelConeTracingRenderer();

		void Init(unsigned int viewportWidth,unsigned int viewportHeight, Object * quadObj,Object* brdfLUTQuadObj, Object * cubeObj);
		void InitVoxelization();
		void InitVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight);
		void Voxilize(const std::vector<Object*>&objs, Object* skybox,
			bool clearVoxelizationFirst = true);
		void Render(
			Camera* camera,
			const std::vector<Object*>&objs, 
			const LightSources* lightSources,
			unsigned int viewportWidth,
			unsigned int viewportHeight, Object* skybox,
			RenderingMode reneringMode = RenderingMode::VOXELIZATION_VISUALIZATION
		);
		void RenderVoxelVisualization(Camera* camera,const std::vector<Object*>&objs,  unsigned int viewportWidth, unsigned int viewportHeight);
		void RenderScene(const std::vector<Object*>&objs, unsigned int viewportWidth, unsigned int viewportHeight, Object* skybox);

		void RenderSpecularBRDFLUTMap();
		
		/*Debug function*/
		//void VoxelizeTest(const std::vector<Object*>& objs);
		/* Settings */
		static bool s_Shadows;
		static bool s_IndirectDiffuseLight;
		static bool s_IndirectSpecularLight;
		static bool s_DirectLight;
		static float s_GICoeffs;
		static bool s_VoxelizeNow;
		static bool s_HDR;
		static int s_VisualizeMipmapLevel;
	private:
		Texture3D* m_VoxelTexture = nullptr;
		unsigned int m_VoxelTextureSize = 256;// 256;// 64;


		// ----------------
		// Voxelization.
		// ----------------

		bool m_AutomaticallyRegenerateMipmap = true;
		bool m_RegenerateMipmapQueued = true;
		bool m_AutomaticallyVoxelize = true;
		bool m_VoxelizationQueued = true;
		int  m_VoxelizationSparsity = 1; // Number of ticks between mipmap generation. 
		int m_TicksSinceLastVoxelization = m_VoxelizationSparsity;

		// ----------------
		// Rendering.
		// ----------------
		std::shared_ptr<Shader> m_VoxelizationShader;
		std::shared_ptr<Shader> m_WorldPositionShader;
		std::shared_ptr<Shader> m_VoxelVisualizationShader;
		std::shared_ptr<Shader> m_VoxelConeTracingShader;

		// ----------------
		// PBR BRDF LUT render.
		// ----------------
		std::shared_ptr<Shader> m_SpecularBRDFLutShader;
		/*只计算一次*/
		std::shared_ptr<Texture> m_SpecularBrdfLUTTexture = nullptr;
		Object* m_BrdfLUTQuadObj = nullptr;//pbr brdf LUT map render

		/*Debug function*/
		//std::shared_ptr<Shader> m_VoxelizationTestShader;
		//std::shared_ptr<Shader> m_FrontBackCubeTestShader;



		Object * m_QuadObj = nullptr;//用于显示体素化结果 ： voxel Visualization
		Object * m_CubeObj = nullptr; //控制体素化渲染范围
		Object* m_DebugQuadObj = nullptr; //darw front face and back face of cube


		bool m_IsInitialize = false;



	

	};

}
