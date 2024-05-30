#pragma once
#if GE_API_VULKAN

#include <vulkan/vulkan.h>
namespace BlackPearl {
    namespace vk {

        enum class BorderColor
        {
            eFloatTransparentBlack = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
            eIntTransparentBlack = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
            eFloatOpaqueBlack = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            eIntOpaqueBlack = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            eFloatOpaqueWhite = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            eIntOpaqueWhite = VK_BORDER_COLOR_INT_OPAQUE_WHITE,
            eFloatCustomEXT = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT,
            eIntCustomEXT = VK_BORDER_COLOR_INT_CUSTOM_EXT
        };


        enum class SamplerAddressMode 
        {
            eRepeat = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            eMirroredRepeat = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
            eClampToEdge = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            eClampToBorder = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            eMirrorClampToEdge = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
            eMirrorClampToEdgeKHR = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE_KHR
        };

        enum class AccessFlagBits2 : VkAccessFlags2
        {
            eNone = VK_ACCESS_2_NONE,
            eIndirectCommandRead = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,
            eIndexRead = VK_ACCESS_2_INDEX_READ_BIT,
            eVertexAttributeRead = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
            eUniformRead = VK_ACCESS_2_UNIFORM_READ_BIT,
            eInputAttachmentRead = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT,
            eShaderRead = VK_ACCESS_2_SHADER_READ_BIT,
            eShaderWrite = VK_ACCESS_2_SHADER_WRITE_BIT,
            eColorAttachmentRead = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT,
            eColorAttachmentWrite = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            eDepthStencilAttachmentRead = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
            eDepthStencilAttachmentWrite = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            eTransferRead = VK_ACCESS_2_TRANSFER_READ_BIT,
            eTransferWrite = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            eHostRead = VK_ACCESS_2_HOST_READ_BIT,
            eHostWrite = VK_ACCESS_2_HOST_WRITE_BIT,
            eMemoryRead = VK_ACCESS_2_MEMORY_READ_BIT,
            eMemoryWrite = VK_ACCESS_2_MEMORY_WRITE_BIT,
            eShaderSampledRead = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
            eShaderStorageRead = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
            eShaderStorageWrite = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
#ifdef VK_ENABLE_BETA_EXTENSIONS
            eVideoDecodeReadKHR = VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR,
            eVideoDecodeWriteKHR = VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR,
            eVideoEncodeReadKHR = VK_ACCESS_2_VIDEO_ENCODE_READ_BIT_KHR,
            eVideoEncodeWriteKHR = VK_ACCESS_2_VIDEO_ENCODE_WRITE_BIT_KHR,
#endif /*VK_ENABLE_BETA_EXTENSIONS*/
            eTransformFeedbackWriteEXT = VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT,
            eTransformFeedbackCounterReadEXT = VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT,
            eTransformFeedbackCounterWriteEXT = VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT,
            eConditionalRenderingReadEXT = VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT,
            eCommandPreprocessReadNV = VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV,
            eCommandPreprocessWriteNV = VK_ACCESS_2_COMMAND_PREPROCESS_WRITE_BIT_NV,
            eFragmentShadingRateAttachmentReadKHR = VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR,
            eAccelerationStructureReadKHR = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
            eAccelerationStructureWriteKHR = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
            eFragmentDensityMapReadEXT = VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT,
            eColorAttachmentReadNoncoherentEXT = VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT,
            eDescriptorBufferReadEXT = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT,
            eInvocationMaskReadHUAWEI = VK_ACCESS_2_INVOCATION_MASK_READ_BIT_HUAWEI,
            eShaderBindingTableReadKHR = VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR,
            eMicromapReadEXT = VK_ACCESS_2_MICROMAP_READ_BIT_EXT,
            eMicromapWriteEXT = VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT,
            eOpticalFlowReadNV = VK_ACCESS_2_OPTICAL_FLOW_READ_BIT_NV,
            eOpticalFlowWriteNV = VK_ACCESS_2_OPTICAL_FLOW_WRITE_BIT_NV,
            eAccelerationStructureReadNV = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_NV,
            eAccelerationStructureWriteNV = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_NV,
            eShadingRateImageReadNV = VK_ACCESS_2_SHADING_RATE_IMAGE_READ_BIT_NV
        };

         enum class PipelineStageFlagBits2 : VkPipelineStageFlags2
        {
            eNone = VK_PIPELINE_STAGE_2_NONE,
            eTopOfPipe = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            eDrawIndirect = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,
            eVertexInput = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
            eVertexShader = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            eTessellationControlShader = VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT,
            eTessellationEvaluationShader = VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT,
            eGeometryShader = VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT,
            eFragmentShader = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            eEarlyFragmentTests = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
            eLateFragmentTests = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            eColorAttachmentOutput = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            eComputeShader = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
            eAllTransfer = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
            eBottomOfPipe = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            eHost = VK_PIPELINE_STAGE_2_HOST_BIT,
            eAllGraphics = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            eAllCommands = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            eCopy = VK_PIPELINE_STAGE_2_COPY_BIT,
            eResolve = VK_PIPELINE_STAGE_2_RESOLVE_BIT,
            eBlit = VK_PIPELINE_STAGE_2_BLIT_BIT,
            eClear = VK_PIPELINE_STAGE_2_CLEAR_BIT,
            eIndexInput = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
            eVertexAttributeInput = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
            ePreRasterizationShaders = VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT,
#if defined( VK_ENABLE_BETA_EXTENSIONS )
            eVideoDecodeKHR = VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR,
            eVideoEncodeKHR = VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR,
#endif /*VK_ENABLE_BETA_EXTENSIONS*/
            eTransformFeedbackEXT = VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT,
            eConditionalRenderingEXT = VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT,
            eCommandPreprocessNV = VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV,
            eFragmentShadingRateAttachmentKHR = VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
            eAccelerationStructureBuildKHR = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            eRayTracingShaderKHR = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR,
            eFragmentDensityProcessEXT = VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT,
            eTaskShaderEXT = VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT,
            eMeshShaderEXT = VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT,
            eSubpassShadingHUAWEI = VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI,
            eInvocationMaskHUAWEI = VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI,
            eAccelerationStructureCopyKHR = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR,
            eMicromapBuildEXT = VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT,
            eOpticalFlowNV = VK_PIPELINE_STAGE_2_OPTICAL_FLOW_BIT_NV,
            eAccelerationStructureBuildNV = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_NV,
            eMeshShaderNV = VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_NV,
            eRayTracingShaderNV = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_NV,
            eShadingRateImageNV = VK_PIPELINE_STAGE_2_SHADING_RATE_IMAGE_BIT_NV,
            eTaskShaderNV = VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_NV,
            eTransfer = VK_PIPELINE_STAGE_2_TRANSFER_BIT
        };


         enum DescriptorType :uint32_t
         {
             eSampler = VK_DESCRIPTOR_TYPE_SAMPLER,
             eCombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
             eSampledImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
             eStorageImage = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
             eUniformTexelBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
             eStorageTexelBuffer = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
             eUniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             eStorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
             eUniformBufferDynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             eStorageBufferDynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
             eInputAttachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
             eInlineUniformBlock = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
             eAccelerationStructureKHR = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
             eAccelerationStructureNV = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV,
             eSampleWeightImageQCOM = VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM,
             eBlockMatchImageQCOM = VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM,
             eMutableEXT = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
             eInlineUniformBlockEXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,
             eMutableVALVE = VK_DESCRIPTOR_TYPE_MUTABLE_VALVE
         };

         enum class ShaderStageFlagBits : VkShaderStageFlags
         {
             eVertex = VK_SHADER_STAGE_VERTEX_BIT,
             eTessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
             eTessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
             eGeometry = VK_SHADER_STAGE_GEOMETRY_BIT,
             eFragment = VK_SHADER_STAGE_FRAGMENT_BIT,
             eCompute = VK_SHADER_STAGE_COMPUTE_BIT,
             eAllGraphics = VK_SHADER_STAGE_ALL_GRAPHICS,
             eAll = VK_SHADER_STAGE_ALL,
             eRaygenKHR = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
             eAnyHitKHR = VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
             eClosestHitKHR = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
             eMissKHR = VK_SHADER_STAGE_MISS_BIT_KHR,
             eIntersectionKHR = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
             eCallableKHR = VK_SHADER_STAGE_CALLABLE_BIT_KHR,
             eTaskEXT = VK_SHADER_STAGE_TASK_BIT_EXT,
             eMeshEXT = VK_SHADER_STAGE_MESH_BIT_EXT,
             eSubpassShadingHUAWEI = VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
             eAnyHitNV = VK_SHADER_STAGE_ANY_HIT_BIT_NV,
             eCallableNV = VK_SHADER_STAGE_CALLABLE_BIT_NV,
             eClosestHitNV = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV,
             eIntersectionNV = VK_SHADER_STAGE_INTERSECTION_BIT_NV,
             eMeshNV = VK_SHADER_STAGE_MESH_BIT_NV,
             eMissNV = VK_SHADER_STAGE_MISS_BIT_NV,
             eRaygenNV = VK_SHADER_STAGE_RAYGEN_BIT_NV,
             eTaskNV = VK_SHADER_STAGE_TASK_BIT_NV
         };

         enum class ImageLayout
         {
             eUndefined = VK_IMAGE_LAYOUT_UNDEFINED,
             eGeneral = VK_IMAGE_LAYOUT_GENERAL,
             eColorAttachmentOptimal = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
             eDepthStencilAttachmentOptimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
             eDepthStencilReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
             eShaderReadOnlyOptimal = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
             eTransferSrcOptimal = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
             eTransferDstOptimal = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             ePreinitialized = VK_IMAGE_LAYOUT_PREINITIALIZED,
             eDepthReadOnlyStencilAttachmentOptimal = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
             eDepthAttachmentStencilReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
             eDepthAttachmentOptimal = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
             eDepthReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
             eStencilAttachmentOptimal = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
             eStencilReadOnlyOptimal = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
             eReadOnlyOptimal = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
             eAttachmentOptimal = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
             ePresentSrcKHR = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
#ifdef VK_ENABLE_BETA_EXTENSIONS
             eVideoDecodeDstKHR = VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR,
             eVideoDecodeSrcKHR = VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR,
             eVideoDecodeDpbKHR = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR,
#endif
             eSharedPresentKHR = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
             eFragmentDensityMapOptimalEXT = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT,
             eFragmentShadingRateAttachmentOptimalKHR = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
#ifdef VK_ENABLE_BETA_EXTENSIONS
             eVideoEncodeDstKHR = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR,
             eVideoEncodeSrcKHR = VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR,
             eVideoEncodeDpbKHR = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR,
#endif /*VK_ENABLE_BETA_EXTENSIONS*/
             eAttachmentFeedbackLoopOptimalEXT = VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT,
             eAttachmentOptimalKHR = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
             eDepthAttachmentOptimalKHR = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR,
             eDepthAttachmentStencilReadOnlyOptimalKHR = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR,
             eDepthReadOnlyOptimalKHR = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR,
             eDepthReadOnlyStencilAttachmentOptimalKHR = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR,
             eReadOnlyOptimalKHR = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR,
             eShadingRateOptimalNV = VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV,
             eStencilAttachmentOptimalKHR = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR,
             eStencilReadOnlyOptimalKHR = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR
         };

         enum class Filter 
         {
             eNearest = VK_FILTER_NEAREST,
             eLinear = VK_FILTER_LINEAR,
             eCubicEXT = VK_FILTER_CUBIC_EXT,
             eCubicIMG = VK_FILTER_CUBIC_IMG
         };
         enum class CompareOp
         {
             eNever = VK_COMPARE_OP_NEVER,
             eLess = VK_COMPARE_OP_LESS,
             eEqual = VK_COMPARE_OP_EQUAL,
             eLessOrEqual = VK_COMPARE_OP_LESS_OR_EQUAL,
             eGreater = VK_COMPARE_OP_GREATER,
             eNotEqual = VK_COMPARE_OP_NOT_EQUAL,
             eGreaterOrEqual = VK_COMPARE_OP_GREATER_OR_EQUAL,
             eAlways = VK_COMPARE_OP_ALWAYS
         };

         enum class SampleCountFlagBits : VkSampleCountFlags
         {
             e1 = VK_SAMPLE_COUNT_1_BIT,
             e2 = VK_SAMPLE_COUNT_2_BIT,
             e4 = VK_SAMPLE_COUNT_4_BIT,
             e8 = VK_SAMPLE_COUNT_8_BIT,
             e16 = VK_SAMPLE_COUNT_16_BIT,
             e32 = VK_SAMPLE_COUNT_32_BIT,
             e64 = VK_SAMPLE_COUNT_64_BIT
         };

    }
}
#endif