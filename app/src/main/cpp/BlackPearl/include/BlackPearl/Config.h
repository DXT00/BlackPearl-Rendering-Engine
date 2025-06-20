#pragma once
#include<bitset>
namespace BlackPearl {
	//һ��Entity���32��Component
	//inline static void SyncGPU();
#define GE_OLD_WORLD_MAX			2097152.0						/* UE4 maximum world size */
#define GE_OLD_HALF_WORLD_MAX		(GE_OLD_WORLD_MAX * 0.5)		/* UE4 half maximum world size */

#ifdef GE_API_OPENGL
#define GL_BACKBUFFER_CNT 1
#endif
	class Configuration {
	public:
        static const bool GUseThreadedRendering = false;
		static const unsigned int MaxComponents = 32;
		typedef std::bitset<MaxComponents> ComponentMask;

		static const unsigned int WindowWidth = 960;  ///300;//960;// 1920 1024x768 
		static const unsigned int WindowHeight = 540;// 300;//540;// 540;1080

        //多重采样
		static const unsigned int MSAA_SAMPLES = 1;

		//VSync 默认禁用垂直同步
		static const bool Vsync = true;//false;
        static int32_t SyncInterval;
		//static const VoxelConeTracingRenderer::RenderingMode RenderingMode; 
		static const bool MobileMultiView = false; //for vr
		// ------------------------------------------
		// Animaition Joints conut
		// ------------------------------------------
		static const int MaxJointsCount = 70;


		// ------------------------------------------
		// Max pointLight Num
		// ------------------------------------------
		static const unsigned int MaxPointLightNum = 20;


		// ------------------------------------------
		// Shadow map
		// ------------------------------------------
        /*对于每个 PointLight 在 m_ShadowRaduis 球体范围内的物体才会画出shadow */
		static const float ShadowMapPointLightRadius;


		// ------------------------------------------
		// light probe environment map	and specular map resolution
		// ------------------------------------------
        /* prefilterMap.glsl里的	float resolution =512.0;也要改 */
		static const float EnvironmantMapResolution;
		//static const float GICoeefs;

		static void SyncGPU();
        static const bool bUpdateProbePerFrame;

		// ------------------------------------------
		// Map config
		// ------------------------------------------
		static const unsigned int MapSize = 100;
		static const unsigned int AreaSize = 10;

		//  ------------------------------------------
		// Ray Tracing config
		// ------------------------------------------
		static const unsigned int MaxRayRecursionDepth = 3;

		//  ------------------------------------------
		// Mesh shader && Meshlet config
		// ------------------------------------------
		static const unsigned int MaxVerts = 256;
		static const unsigned int MaxPrims = 256;

		static const unsigned int DefaultVerts = 128;
		static const unsigned int DefaultPrims = 128;


		//-----------------------------------------------
		// Batch rendering limit
		//-----------------------------------------------
		//if exceed MaxObjsInABatch objs in one batch shader compiler will give an error:
		//"offset for relative array access outside supported range"
		// no sulution yet, details: https://forums.developer.nvidia.com/t/bug-compiler-issues-internal-error-offset-for-relative-array-access-outside-supported-range/46430
		static const unsigned int MaxObjsInABatch = 300;


		/* Rendering Device Settings*/
		static const unsigned int SwapchainCount = 2;
	
		/* shader path settings*/
		static const char* GetShaderTypeName();

        /* if gl program store to bin file*/
        static const bool bCacheGLProgram;


		/* OpenGL settings*/

        //-----------------------------------------------
        // Render Pipeline settings
        //-----------------------------------------------
        static const bool bDeferredShading;
        static const bool bUseSinglePass;
        static const bool bUseIBL;


	};
	struct ShaderConfig {


		static const char* AMBIENT_COLOR;
		static const char* DIFFUSE_COLOR;
		static const char* SPECULAR_COLOR;
		static const char* EMISSION_COLOR;

		static const char* METALLIC_VALUE;
		static const char* ROUGHNESS_VALUE;
		static const char* AO_VALUE;

		static const char* DIFFUSE_TEXTURE2D;
		static const char* SPECULAR_TEXTURE2D;
		static const char* EMISSION_TEXTURE2D;
		static const char* NORMAL_TEXTURE2D;
		static const char* HEIGHT_TEXTURE2D;
		static const char* AO_TEXTURE2D;
		static const char* ROUGHNESS_TEXTURE2D;
		static const char* METALLIC_TEXTURE2D;
		static const char* DEPTH_TEXTURE2D;

		static const char* CUBE_TEXTURECUBE;
		static const char* SHININESS;

		static const char* IS_BLINNLIGHT;
		static const char* IS_PBR_TEXTURE_SAMPLE;
		static const char* IS_AMBIENT_TEXTURE_SAMPLE;
		static const char* IS_DIFFUSE_TEXTURE_SAMPLE;
		static const char* IS_SPECULAR_TEXTURE_SAMPLE;
		static const char* IS_METALLIC_TEXTURE_SAMPLE;
		static const char* IS_EMISSION_TEXTURE_SAMPLE;
		static const char* IS_HEIGHT_TEXTURE_SAMPLE;

	};

    /*
     * [TextureQuality@2]
        r.Streaming.MipBias=0
        r.Streaming.AmortizeCPUToGPUCopy=0
        r.Streaming.MaxNumTexturesToStreamPerFrame=0
        r.Streaming.Boost=1
        r.MaxAnisotropy=4
        r.VT.MaxAnisotropy=8
        r.Streaming.LimitPoolSizeToVRAM=1
        r.Streaming.PoolSize=800
        r.Streaming.MaxEffectiveScreenSize=0

        [TextureQuality@3]
        r.Streaming.MipBias=0
        r.Streaming.AmortizeCPUToGPUCopy=0
        r.Streaming.MaxNumTexturesToStreamPerFrame=0
        r.Streaming.Boost=1
        r.MaxAnisotropy=8
        r.VT.MaxAnisotropy=8
        r.Streaming.LimitPoolSizeToVRAM=0
        r.Streaming.PoolSize=1000
        r.Streaming.MaxEffectiveScreenSize=0
     * */

}

namespace BlackPearl {
/** Thread ID of the main/game thread																		*/
    extern uint32_t GGameThreadId;
    extern uint32_t GRenderThreadId;
    extern bool     GUseThreadedRendering;

    extern uint32_t GAndroidWindowDPI;
    extern uint32_t  GAndroid3DSceneMinDPI;
    extern int32_t GAndroid3DSceneMaxDesiredPixelCount;
    extern int32_t GAndroidWindowDPIQueryMethod;

    extern int MobileUseHWsRGBEncodingCVAR;
    extern int CVarDisableASTC;
    extern int CVarOverrideExternalTextureSupport;
    extern int CVarDisableEarlyFragmentTests;

    extern int CVarEnableAdrenoTilingHint;
    extern int CVarDisableFBFNonCoherent;

    extern int MobileHDRCvar;
    extern int CVarStreamingTexturePoolSize;

    extern int CVarDisjointTimerQueries;

}