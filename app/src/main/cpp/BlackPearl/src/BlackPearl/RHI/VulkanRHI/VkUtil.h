#pragma once
#if GE_API_VULKAN

#include <vulkan/vulkan.h>
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHISampler.h"
namespace BlackPearl {

    class ETexture;
    class StagingTexture;
    class InputLayout;
    class Buffer;
    class EShader;
    class Sampler;
    class Framebuffer;
    class GraphicsPipeline;
    class ComputePipeline;
    class BindingSet;
    class EvenetQuery;
    class TimerQuery;
    class Marker;
    class Device;


    struct ResourceStateMapping
    {
        ResourceStates nvrhiState;
        VkPipelineStageFlags stageFlags;
        VkAccessFlags accessMask;
        VkImageLayout imageLayout;
        ResourceStateMapping(ResourceStates nvrhiState, VkPipelineStageFlags stageFlags, VkAccessFlags accessMask, VkImageLayout imageLayout) :
            nvrhiState(nvrhiState), stageFlags(stageFlags), accessMask(accessMask), imageLayout(imageLayout) {}
    };

    struct ResourceStateMapping2 // for use with KHR_synchronization2
    {
        ResourceStates nvrhiState;
        VkPipelineStageFlags2 stageFlags;
        VkAccessFlags2 accessMask;
        VkImageLayout imageLayout;
        ResourceStateMapping2(ResourceStates nvrhiState, VkPipelineStageFlags2 stageFlags, VkAccessFlags2 accessMask, VkImageLayout imageLayout) :
            nvrhiState(nvrhiState), stageFlags(stageFlags), accessMask(accessMask), imageLayout(imageLayout) {}
    };

    class VkUtil
    {
    public:
        static VkSamplerAddressMode convertSamplerAddressMode(SamplerAddressMode mode);
        static VkPipelineStageFlagBits2 convertShaderTypeToPipelineStageFlagBits(ShaderType shaderType);
        static VkShaderStageFlagBits convertShaderTypeToShaderStageFlagBits(ShaderType shaderType);
        static ResourceStateMapping convertResourceState(ResourceStates state);
        static ResourceStateMapping2 convertResourceState2(ResourceStates state);
        static VkPrimitiveTopology convertPrimitiveTopology(PrimitiveType topology);
        static VkPolygonMode convertFillMode(RasterFillMode mode);
        static VkCullModeFlagBits convertCullMode(RasterCullMode mode);
        static VkCompareOp convertCompareOp(ComparisonFunc op);
        static VkStencilOp convertStencilOp(StencilOp op);
        static VkStencilOpState convertStencilState(const DepthStencilState& depthStencilState, const DepthStencilState::StencilOpDesc& desc);
        static VkBlendFactor convertBlendValue(BlendFactor value);
        static VkBlendOp convertBlendOp(BlendOp op);
        static VkColorComponentFlags convertColorMask(ColorMask mask);
        static VkPipelineColorBlendAttachmentState convertBlendState(const BlendState::RenderTarget& state);
        //static VkBuildAccelerationStructureFlagsKHR convertAccelStructBuildFlags(rt::AccelStructBuildFlags buildFlags);
       // static VkGeometryInstanceFlagsKHR convertInstanceFlags(rt::InstanceFlags instanceFlags);
        static VkExtent2D convertFragmentShadingRate(VariableShadingRate shadingRate);
        static VkFragmentShadingRateCombinerOpKHR convertShadingRateCombiner(ShadingRateCombiner combiner);
   
        static VkBorderColor pickSamplerBorderColor(const SamplerDesc& d);
        static VkFormat convertFormat(Format format);

        static void countSpecializationConstants(
            EShader* shader,
            size_t& numShaders,
            size_t& numShadersWithSpecializations,
            size_t& numSpecializationConstants);

        static VkPipelineShaderStageCreateInfo makeShaderStageCreateInfo(
            EShader* shader,
            std::vector<VkSpecializationInfo>& specInfos,
            std::vector<VkSpecializationMapEntry>& specMapEntries,
            std::vector<uint32_t>& specData);
    };

}

#endif