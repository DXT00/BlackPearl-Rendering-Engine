#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/Material/Texture3D.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Buffer.h"
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

		void Init(unsigned int viewportWidth,unsigned int viewportHeight);
		void InitVoxelization();
		void InitVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight);
		void Voxilize(const std::vector<Object*>&objs, Object * m_QuadObj, Object * m_CubeObj, bool clearVoxelizationFirst = true);
		void Render(
			const std::vector<Object*>&objs, 
			Object * m_QuadObj,
			Object * m_CubeObj,
			const LightSources* lightSources,
			unsigned int viewportWidth,
			unsigned int viewportHeight,
			RenderingMode reneringMode = RenderingMode::VOXELIZATION_VISUALIZATION
		);
		void RenderVoxelVisualization(const std::vector<Object*>&objs, Object * m_QuadObj, Object * m_CubeObj, unsigned int viewportWidth, unsigned int viewportHeight);
		void RenderScene(const std::vector<Object*>&objs, unsigned int viewportWidth, unsigned int viewportHeight);

	

	private:
		Texture3D* m_VoxelTexture = nullptr;
		unsigned int m_VoxelTextureSize = 128;// 64;


		// ----------------
		// Voxelization.
		// ----------------

		bool m_AutomaticallyRegenerateMipmap = true;
		bool m_RegenerateMipmapQueued = true;
		bool m_AutomaticallyVoxelize = true;
		bool m_VoxelizationQueued = true;
		int  m_VoxelizationSparsity = 1; // Number of ticks between mipmap generation. 
		// (voxelization sparsity gives unstable framerates, so not sure if it's worth it in interactive applications.)
		int m_TicksSinceLastVoxelization = m_VoxelizationSparsity;

		// ----------------
		// Rendering.
		// ----------------
		bool m_Shadows = true;
		bool m_IndirectDiffuseLight = true;
		bool m_IndirectSpecularLight = true;
		bool m_DirectLight = true;


		std::shared_ptr<Shader> m_VoxelizationShader;
		std::shared_ptr<Shader> m_WorldPositionShader;
		std::shared_ptr<Shader> m_VoxelVisualizationShader;
		std::shared_ptr<Shader> m_VoxelConeTracingShader;


		std::shared_ptr<FrameBuffer> m_FrameBuffer1;
		std::shared_ptr<FrameBuffer> m_FrameBuffer2;

		


	};

}
