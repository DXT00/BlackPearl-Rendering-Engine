#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"

#include "Renderer/Shader/MaterialShader.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "Renderer/Voxel/Voxel.h"
#include "MainCamera/MainCamera.h"

namespace BlackPearl {

    enum VoxelizeStrategy {
        Camera_Center, // Voxelization centered on the camera ， used for voxel contracing
        Tile_Center //Separate tiles for voxelization to store light , used for sdf contracing
    };

    //场景分Tile，每个Tile一个 voxel
	class VoxelConeTracingRenderer: public BasicRenderer
	{
	public:
		enum RenderingMode {

			VOXELIZATION_VISUALIZATION = 0, // Voxelization visualization.
			VOXEL_CONE_TRACING = 1,			// Global illumination using voxel cone tracing.
            VOXELIZE = 2            //Only voxelize for other pass
        };
		
        VoxelConeTracingRenderer(IDevice* device);
		~VoxelConeTracingRenderer();

		void Init(Scene* scene);
		void InitVoxelization();
		void InitVoxelVisualization();

        void Voxilize(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, bool clearVoxelizationFirst = true);


		void Voxilize(const std::vector<Object*>&objs, Object* skybox,
			bool clearVoxelizationFirst = true);

        
		void Render(
            ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene,
			RenderingMode reneringMode
		);

		void RenderVoxelVisualization(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
		void RenderScene(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);

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
        void _VoxelClear(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
        void _VoxelizeScene(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
        void _VoxelizeSky(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
		TextureHandle m_VoxelTexture = nullptr;
		unsigned int m_VoxelTextureSize = 256;// 256;// 64;

		// ----------------
		// Voxelization.
		// ----------------

		bool m_AutomaticallyRegenerateMipmap = true;
		bool m_RegenerateMipmapQueued = true;
		bool m_AutomaticallyVoxelize = true;
		bool m_VoxelizationQueued = true;
		int  m_VoxelizationSparsity = 1; // Number of ticks between mipmap generation. 
		int  m_TicksSinceLastVoxelization = m_VoxelizationSparsity;

		// ----------------
		// Rendering.
		// ----------------
		MaterialShader* m_VoxelizationShader;
        MaterialShader* m_VoxelizationSkyboxShader;

		MaterialShader* m_WorldPositionShader;
		MaterialShader* m_VoxelVisualizationShader;
		MaterialShader* m_VoxelConeTracingShader;

        MaterialShader* m_VoxelClearShader;

		// ----------------
		// PBR BRDF LUT render.
		// ----------------
        MaterialShader* m_SpecularBRDFLutShader;


		/*只计算一次*/
		TextureHandle m_SpecularBrdfLUTTexture = nullptr;
		Object* m_BrdfLUTQuadObj = nullptr;//pbr brdf LUT map render

		/*Debug function*/
		//std::shared_ptr<Shader> m_VoxelizationTestShader;
		//std::shared_ptr<Shader> m_FrontBackCubeTestShader;



		Object * m_QuadObj = nullptr;//用于显示体素化结果 ： voxel Visualization
		Object * m_CubeObj = nullptr; //控制体素化渲染范围
		Object* m_DebugQuadObj = nullptr; //darw front face and back face of cube


		bool m_IsInitialize = false;

        // voxel 分区, 每个 area 一个voxel
        //std::vector<Voxel> m_Voxels;
        TextureHandle m_DummyVoxelRT;

        //Voxel binding sets;
        BindingLayoutHandle m_VoxelBindingLayout;
        BindingSetHandle    m_VoxelBindingSet;
        GraphicsPipelineHandle m_VoxelPso = nullptr;
        BufferHandle    m_VoxelCB;

        // Voxel orth view
        MainCamera* m_OrthCamera;



        //Voxel visualize binding sets;
        BindingLayoutHandle m_VoxelVisualBindingLayout;
        BindingSetHandle    m_VoxelVisualBindingSet;
        GraphicsPipelineHandle m_VoxelVisualPso = nullptr;
        BufferHandle    m_VoxelVisualCB;
	

        //Voxel clear sets;
        BindingLayoutHandle m_VoxelClearBindingLayout;
        BindingSetHandle    m_VoxelClearBindingSet;
        ComputePipelineHandle m_VoxelClearPso = nullptr;


        // Skybox materials
        TextureHandle   m_SkyboxTexture[3];
        BufferHandle    m_SkyCB;
        BindingLayoutHandle m_SkyboxBindingLayout;
        BindingSetHandle    m_SkyboxBindingSet;
        GraphicsPipelineHandle m_VoxelSkyboxPso = nullptr;
	};

}
