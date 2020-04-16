#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/Material/Texture3D.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Renderer/Material/TextureImage2D.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Renderer/Material/BufferTexture.h"
namespace BlackPearl{

	class VoxelConeTracingSVORenderer:public BasicRenderer
	{
	public:
		enum RenderingMode {
			VOXELIZATION_VISUALIZATION = 0, // Voxelization visualization.
			VOXEL_CONE_TRACING = 1			// Global illumination using voxel cone tracing.
		};
		VoxelConeTracingSVORenderer();
		~VoxelConeTracingSVORenderer();
		

		void Init(unsigned int viewportWidth, unsigned int viewportHeight, Object* cubeObj, Object* brdfLUTQuadObj, Object* quadFinalScreenObj,
			std::vector<Object*> objs, Object* skybox);
		void InitVoxelization();
		void InitVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight);
		
		/*void Voxilize(const std::vector<Object*>& objs, Object* skybox,
			bool clearVoxelizationFirst = true);*/
		void Voxelize(const std::vector<Object*>& objs, Object* skybox,bool storeData);

		void Render(
			Camera* camera,
			const std::vector<Object*>& objs,
			const LightSources* lightSources,
			unsigned int viewportWidth,
			unsigned int viewportHeight, Object* skybox,
			RenderingMode reneringMode = RenderingMode::VOXELIZATION_VISUALIZATION
		);
		void RenderVoxelVisualization(Camera* camera, const std::vector<Object*>& objs, unsigned int viewportWidth, unsigned int viewportHeight);
		void RenderScene(const std::vector<Object*>& objs, const LightSources* lightSources,
			unsigned int viewportWidth, unsigned int viewportHeight, Object* skybox);

		/* Debug */
		void ShowBufferTexture(std::shared_ptr<BufferTexture> bufferTexture, int dataLength);

		/* GBuffer */
	
		void DrawGBuffer(Object* gBufferDebugQuad);


		/* SVO */
		void BuildFragmentList(const std::vector<Object*>& obj, Object* skybox);
		void BuildSVO();
		void RebuildSVO(const std::vector<Object*>& obj, Object* skybox);

		/* Settings */
		static bool s_Shadows;
		static bool s_IndirectDiffuseLight;
		static bool s_IndirectSpecularLight;
		static bool s_DirectLight;
		static float s_GICoeffs;
		static bool s_VoxelizeNow;
		static bool s_HDR;
		static bool s_GuassianHorizontal;
		static bool s_GuassianVertical;
		static bool s_ShowBlurArea;
		static float s_SpecularBlurThreshold;
		static int s_VisualizeMipmapLevel;
		static bool s_MipmapBlurSpecularTracing;
	private:

		//Texture3D* m_VoxelTexture = nullptr;
		unsigned int m_VoxelTextureSize = 128;// 128;// 256;

		unsigned int m_ScreenWidth = Configuration::WindowWidth;
		unsigned int m_ScreenHeight = Configuration::WindowHeight;
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
		std::shared_ptr<Shader> m_VoxelizationShader;//SVO
		std::shared_ptr<Shader> m_VoxelVisualizationShader;
		std::shared_ptr<Shader> m_SVOTracingShader;
		std::shared_ptr<Shader> m_FinalScreenShader;


		// ----------------
		// PBR BRDF LUT render.
		// ----------------
		Object* m_BrdfLUTQuadObj = nullptr;//pbr brdf LUT map render

		/*Debug function*/
		// ----------------
		// Object
		// ----------------
		//Object* m_VisualizationQuadObj = nullptr;//用于显示体素化结果 ： voxel Visualization
		Object* m_CubeObj = nullptr; //控制体素化渲染范围
		//Object* m_DebugQuadObj = nullptr; //darw front face and back face of cube
		Object* m_QuadFinalScreenObj = nullptr;//show SVO tracing results
		//Object* m_SurroundSphere = nullptr;
		// ----------------
		// GBuffer
		// ----------------
		std::shared_ptr<GBuffer> m_GBuffer;
		std::shared_ptr<Shader> m_GBufferShader;
		Object* m_QuadGbufferObj = nullptr;//用于显示体素化结果 ： voxel Visualization

		// ----------------
		// Atomic Counter Buffer
		// ----------------
		std::shared_ptr<AtomicBuffer> m_AtomicCountBuffer;
		unsigned int m_NumVoxelFrag = 0;//voxelized fragments'number

		// ----------------
		// Image Texture
		// ----------------
		std::shared_ptr<BufferTexture> m_VoxelPosBufTexture;

		std::shared_ptr<BufferTexture> m_VoxelDiffuseBufTexture;
		std::shared_ptr<BufferTexture> m_VoxelNormBufTexture;

		// ----------------
		// SVO
		// ----------------
		unsigned int m_OctreeLevel = 7;
		unsigned int m_TotalTreeNode = 0;
		std::shared_ptr<BufferTexture> m_OctreeNodeTex[2];
		std::shared_ptr<BufferTexture> m_DebugOctreeBufTexture;

		std::shared_ptr<Shader> m_NodeFlagShader;
		std::shared_ptr<Shader> m_NodeAllocShader;
		std::shared_ptr<Shader> m_NodeInitShader;
		std::shared_ptr<Shader> m_LeafStoreShader;

		std::shared_ptr<VertexArray> m_PointCubeVAO;

		bool m_IsInitialize = false;

	};

}


