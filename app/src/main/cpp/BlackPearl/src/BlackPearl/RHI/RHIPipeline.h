#pragma once
#include<string>
#include<vector>
#include "RHIDefinitions.h"
#include "RHIShader.h"
#include "RHIBindingLayoutDesc.h"
#include "RHIInputLayout.h"
#include "RHIFrameBuffer.h"
namespace BlackPearl {
	struct PipelineShaderDesc
	{
		std::string exportName;
		ShaderHandle shader;
		BindingLayoutHandle bindingLayout;

		PipelineShaderDesc& setExportName(const std::string& value) { exportName = value; return *this; }
		PipelineShaderDesc& setShader(IShader* value) { shader = value; return *this; }
		PipelineShaderDesc& setBindingLayout(IBindingLayout* value) { bindingLayout = value; return *this; }
	};

	struct PipelineHitGroupDesc
	{
		std::string exportName;
		ShaderHandle closestHitShader;
		ShaderHandle anyHitShader;
		ShaderHandle intersectionShader;
		BindingLayoutHandle bindingLayout;
		bool isProceduralPrimitive = false;

		PipelineHitGroupDesc& setExportName(const std::string& value) { exportName = value; return *this; }
		PipelineHitGroupDesc& setClosestHitShader(IShader* value) { closestHitShader = value; return *this; }
		PipelineHitGroupDesc& setAnyHitShader(IShader* value) { anyHitShader = value; return *this; }
		PipelineHitGroupDesc& setIntersectionShader(IShader* value) { intersectionShader = value; return *this; }
		PipelineHitGroupDesc& setBindingLayout(IBindingLayout* value) { bindingLayout = value; return *this; }
		PipelineHitGroupDesc& setIsProceduralPrimitive(bool value) { isProceduralPrimitive = value; return *this; }
	};

	struct RayTracingPipelineDesc
	{
		std::vector<PipelineShaderDesc> shaders;
		std::vector<PipelineHitGroupDesc> hitGroups;
		std::vector<BindingLayoutHandle> globalBindingLayouts;
		uint32_t maxPayloadSize = 0;
		uint32_t maxAttributeSize = sizeof(float) * 2; // typical case: float2 uv;
		uint32_t maxRecursionDepth = 1;
		int32_t hlslExtensionsUAV = -1;

		RayTracingPipelineDesc& addShader(const PipelineShaderDesc& value) { shaders.push_back(value); return *this; }
		RayTracingPipelineDesc& addHitGroup(const PipelineHitGroupDesc& value) { hitGroups.push_back(value); return *this; }
		RayTracingPipelineDesc& addBindingLayout(IBindingLayout* value) { globalBindingLayouts.push_back(value); return *this; }
		RayTracingPipelineDesc& setMaxPayloadSize(uint32_t value) { maxPayloadSize = value; return *this; }
		RayTracingPipelineDesc& setMaxAttributeSize(uint32_t value) { maxAttributeSize = value; return *this; }
		RayTracingPipelineDesc& setMaxRecursionDepth(uint32_t value) { maxRecursionDepth = value; return *this; }
		RayTracingPipelineDesc& setHlslExtensionsUAV(int32_t value) { hlslExtensionsUAV = value; return *this; }
	};

	class IRayTracingPipeline : public IResource
	{
	public:
		[[nodiscard]] virtual const RayTracingPipelineDesc& getDesc() const = 0;
		virtual ShaderTableHandle createShaderTable() = 0;
	};

	typedef RefCountPtr<IRayTracingPipeline> RayTracingPipelineHandle;

	struct GraphicsPipelineDesc
	{
		PrimitiveType primType = PrimitiveType::TriangleList;
		uint32_t patchControlPoints = 0;
		InputLayoutHandle inputLayout;

		ShaderHandle VS;
		ShaderHandle HS;
		ShaderHandle DS;
		ShaderHandle GS;
		ShaderHandle PS;

		bool bFromPSOFileCache;

		BlendState blendState;
		DepthStencilState depthStencilState;
		RasterState rasterState;
		SinglePassStereoState singlePassStereo;

		void setBlendState(const BlendState& value) { blendState = value; }
		void setDepthStencilState(const DepthStencilState& value) { depthStencilState = value;}
		void setRasterState(const RasterState& value) { rasterState = value; }
		void setSinglePassStereoState(const SinglePassStereoState& value) { singlePassStereo = value; }
		
		
		VariableRateShadingState shadingRateState;

		std::vector<BindingLayoutHandle> bindingLayouts;

		GraphicsPipelineDesc& setPrimType(PrimitiveType value) { primType = value; return *this; }
		GraphicsPipelineDesc& setPatchControlPoints(uint32_t value) { patchControlPoints = value; return *this; }
		GraphicsPipelineDesc& setInputLayout(IInputLayout* value) { inputLayout = value; return *this; }
		GraphicsPipelineDesc& setVertexShader(IShader* value) { VS = value; return *this; }
		GraphicsPipelineDesc& setHullShader(IShader* value) { HS = value; return *this; }
		GraphicsPipelineDesc& setTessellationControlShader(IShader* value) { HS = value; return *this; }
		GraphicsPipelineDesc& setDomainShader(IShader* value) { DS = value; return *this; }
		GraphicsPipelineDesc& setTessellationEvaluationShader(IShader* value) { DS = value; return *this; }
		GraphicsPipelineDesc& setGeometryShader(IShader* value) { GS = value; return *this; }
		GraphicsPipelineDesc& setPixelShader(IShader* value) { PS = value; return *this; }
		GraphicsPipelineDesc& setFragmentShader(IShader* value) { PS = value; return *this; }
		//GraphicsPipelineDesc& setRenderState(const RenderState& value) { renderState = value; return *this; }
		GraphicsPipelineDesc& setVariableRateShadingState(const VariableRateShadingState& value) { shadingRateState = value; return *this; }
		GraphicsPipelineDesc& addBindingLayout(IBindingLayout* layout) { bindingLayouts.push_back(layout); return *this; }
	};

	class IGraphicsPipeline : public IResource
	{
	public:
		GraphicsPipelineDesc desc;

		[[nodiscard]] virtual const GraphicsPipelineDesc& getDesc() const = 0;
		[[nodiscard]] virtual const FramebufferInfo& getFramebufferInfo() const = 0;
	};

	typedef RefCountPtr<IGraphicsPipeline> GraphicsPipelineHandle;

	struct ComputePipelineDesc
	{
		ShaderHandle CS;
		std::vector<BindingLayoutHandle> bindingLayouts;

		ComputePipelineDesc& setComputeShader(IShader* value) { CS = value; return *this; }
		ComputePipelineDesc& addBindingLayout(IBindingLayout* layout) { bindingLayouts.push_back(layout); return *this; }
	};

	class IComputePipeline : public IResource
	{
	public:
		[[nodiscard]] virtual const ComputePipelineDesc& getDesc() const = 0;
	};

	typedef RefCountPtr<IComputePipeline> ComputePipelineHandle;

	struct MeshletPipelineDesc
	{
		PrimitiveType primType = PrimitiveType::TriangleList;

		ShaderHandle AS;
		ShaderHandle MS;
		ShaderHandle PS;

		RenderState renderState;

		std::vector<BindingLayoutHandle> bindingLayouts;

		MeshletPipelineDesc& setPrimType(PrimitiveType value) { primType = value; return *this; }
		MeshletPipelineDesc& setTaskShader(IShader* value) { AS = value; return *this; }
		MeshletPipelineDesc& setAmplificationShader(IShader* value) { AS = value; return *this; }
		MeshletPipelineDesc& setMeshShader(IShader* value) { MS = value; return *this; }
		MeshletPipelineDesc& setPixelShader(IShader* value) { PS = value; return *this; }
		MeshletPipelineDesc& setFragmentShader(IShader* value) { PS = value; return *this; }
		MeshletPipelineDesc& setRenderState(const RenderState& value) { renderState = value; return *this; }
		MeshletPipelineDesc& addBindingLayout(IBindingLayout* layout) { bindingLayouts.push_back(layout); return *this; }
	};

	class IMeshletPipeline : public IResource
	{
	public:
		[[nodiscard]] virtual const MeshletPipelineDesc& getDesc() const = 0;
		[[nodiscard]] virtual const FramebufferInfo& getFramebufferInfo() const = 0;
	};

	typedef RefCountPtr<IMeshletPipeline> MeshletPipelineHandle;

}

