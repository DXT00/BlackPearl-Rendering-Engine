#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHIPipeline.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Scene/Scene.h"

namespace BlackPearl {

	class RayTraceRenderer {
	public:
		RayTraceRenderer() {
		}
		bool Init(IDevice* device,
			ShaderFactory& shaderFactory,
			const char* shaderName,
			const std::vector<ShaderMacro>& extraMacros,
			bool useRayQuery,
			uint32_t computeGroupSize,
			IBindingLayout* bindingLayout,
			IBindingLayout* extraBindingLayout,
			IBindingLayout* bindlessLayout);
		void Render(ICommandList* commandList,
			int width,
			int height,
			IBindingSet* bindingSet,
			IBindingSet* extraBindingSet,
			IDescriptorTable* descriptorTable,
			const void* pushConstants,
			const size_t pushConstantSize);



		ShaderHandle ComputeShader;
		ComputePipelineHandle ComputePipeline;

		ShaderLibraryHandle ShaderLibrary;
		RayTracingPipelineHandle RayTracingPipeline;
		ShaderTableHandle ShaderTable;

		uint32_t ComputeGroupSize = 0;

	};
}
