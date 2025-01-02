#include "pch.h"
#include "RayTraceRenderer.h"
namespace BlackPearl {
	bool RayTraceRenderer::Init(IDevice* device,
        ShaderFactory& shaderFactory,
        const char* shaderName,
        const std::vector<ShaderMacro>& extraMacros,
        bool useRayQuery,
        uint32_t computeGroupSize,
        IBindingLayout* bindingLayout,
        IBindingLayout* extraBindingLayout,
        IBindingLayout* bindlessLayout)
	{
        ComputeGroupSize = computeGroupSize;

        std::vector<ShaderMacro> macros = { { "USE_RAY_QUERY", "1" } };

        macros.insert(macros.end(), extraMacros.begin(), extraMacros.end());

        if (useRayQuery)
        {
            ComputeShader = shaderFactory.CreateShader(shaderName, "main", &macros, ShaderType::Compute);
            if (!ComputeShader)
                return false;

            ComputePipelineDesc pipelineDesc;
            if (extraBindingLayout)
                pipelineDesc.bindingLayouts.push_back(extraBindingLayout);
            if (bindlessLayout)
                pipelineDesc.bindingLayouts.push_back(bindlessLayout);
            if (bindingLayout)
                pipelineDesc.bindingLayouts.push_back(bindingLayout);
            pipelineDesc.CS = ComputeShader;
            ComputePipeline = device->createComputePipeline(pipelineDesc);

            if (!ComputePipeline)
                return false;

            return true;
        }

        macros[0].definition = "0"; // USE_RAY_QUERY
        ShaderLibrary = shaderFactory.CreateShaderLibrary(shaderName, &macros);
        if (!ShaderLibrary)
            return false;

        RayTracingPipelineDesc rtPipelineDesc;
        rtPipelineDesc.globalBindingLayouts = { extraBindingLayout, bindlessLayout };
        if (bindingLayout)
            rtPipelineDesc.globalBindingLayouts.push_back(bindingLayout);
        rtPipelineDesc.shaders = {
            { "", ShaderLibrary->getShader("RayGen", ShaderType::RayGeneration), nullptr },
            { "", ShaderLibrary->getShader("Miss", ShaderType::Miss), nullptr }
        };

        rtPipelineDesc.hitGroups = {
            {
                "HitGroup",
                ShaderLibrary->getShader("ClosestHit", ShaderType::ClosestHit),
                ShaderLibrary->getShader("AnyHit", ShaderType::AnyHit),
                nullptr, // intersectionShader
                nullptr, // localBindingLayout
                false // isProceduralPrimitive
            },
        };

        rtPipelineDesc.maxAttributeSize = 8;
        rtPipelineDesc.maxPayloadSize = 40;
        rtPipelineDesc.maxRecursionDepth = 1;

        RayTracingPipeline = device->createRayTracingPipeline(rtPipelineDesc);
        if (!RayTracingPipeline)
            return false;

        ShaderTable = RayTracingPipeline->createShaderTable();
        if (!ShaderTable)
            return false;

        ShaderTable->setRayGenerationShader("RayGen");
        ShaderTable->addMissShader("Miss");
        ShaderTable->addHitGroup("HitGroup");

        return true;
	
	
	}

	void RayTraceRenderer::Render(ICommandList* commandList,
        int width,
        int height,
        IBindingSet* bindingSet,
        IBindingSet* extraBindingSet,
        IDescriptorTable* descriptorTable,
        const void* pushConstants,
        const size_t pushConstantSize)
	{
	
        if (ComputePipeline)
        {
            ComputeState state;
            state.bindings = { extraBindingSet };
            if (descriptorTable)
                state.bindings.push_back(descriptorTable);
            if (bindingSet)
                state.bindings.push_back(bindingSet);
            state.pipeline = ComputePipeline;
            commandList->setComputeState(state);

            if (pushConstants)
                commandList->setPushConstants(pushConstants, pushConstantSize);

            commandList->dispatch(math::div_ceil(width, ComputeGroupSize), math::div_ceil(height, ComputeGroupSize), 1);
        }
        else
        {
            RayTracingState state;
            state.bindings = { extraBindingSet };
            if (descriptorTable)
                state.bindings.push_back(descriptorTable);
            if (bindingSet)
                state.bindings.push_back(bindingSet);
            state.shaderTable = ShaderTable;
            commandList->setRayTracingState(state);

            if (pushConstants)
                commandList->setPushConstants(pushConstants, pushConstantSize);

            DispatchRaysArguments args;
            args.width = width;
            args.height = height;
            args.depth = 1;
            commandList->dispatchRays(args);
        }
	
	}
}
