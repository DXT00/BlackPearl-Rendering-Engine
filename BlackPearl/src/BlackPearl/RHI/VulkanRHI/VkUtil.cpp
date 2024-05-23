/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
#include "pch.h"

#include <array>
#include <assert.h>

#include "VkUtil.h"
#include "VkEnum.h"
#include "VkShader.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/RHI/Common/FormatInfo.h"
#define ENABLE_SHORTCUT_CONVERSIONS 1

namespace BlackPearl
{


	VkSamplerAddressMode VkUtil::convertSamplerAddressMode(SamplerAddressMode mode)
	{
		switch (mode)
		{
		case SamplerAddressMode::ClampToEdge:
			return (VkSamplerAddressMode)vk::SamplerAddressMode::eClampToEdge;

		case SamplerAddressMode::Repeat:
			return  (VkSamplerAddressMode)vk::SamplerAddressMode::eRepeat;

		case SamplerAddressMode::ClampToBorder:
			return  (VkSamplerAddressMode)vk::SamplerAddressMode::eClampToBorder;

		case SamplerAddressMode::MirroredRepeat:
			return  (VkSamplerAddressMode)vk::SamplerAddressMode::eMirroredRepeat;

		case SamplerAddressMode::MirrorClampToEdge:
			return  (VkSamplerAddressMode)vk::SamplerAddressMode::eMirrorClampToEdge;


		default:
			GE_INVALID_ENUM()
				return  (VkSamplerAddressMode)vk::SamplerAddressMode(0);
		}
	}

	VkPipelineStageFlagBits2 VkUtil::convertShaderTypeToPipelineStageFlagBits(ShaderType shaderType)
	{
		if (shaderType == ShaderType::All)
			return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

		uint32_t result = 0;

		if ((shaderType & ShaderType::Compute) != 0)        result |= uint32_t(vk::PipelineStageFlagBits2::eComputeShader);
		if ((shaderType & ShaderType::Vertex) != 0)         result |= uint32_t(vk::PipelineStageFlagBits2::eVertexShader);
		if ((shaderType & ShaderType::Hull) != 0)           result |= uint32_t(vk::PipelineStageFlagBits2::eTessellationControlShader);
		if ((shaderType & ShaderType::Domain) != 0)         result |= uint32_t(vk::PipelineStageFlagBits2::eTessellationEvaluationShader);
		if ((shaderType & ShaderType::Geometry) != 0)       result |= uint32_t(vk::PipelineStageFlagBits2::eGeometryShader);
		if ((shaderType & ShaderType::Pixel) != 0)          result |= uint32_t(vk::PipelineStageFlagBits2::eFragmentShader);
		if ((shaderType & ShaderType::Amplification) != 0)  result |= uint32_t(vk::PipelineStageFlagBits2::eTaskShaderNV);
		if ((shaderType & ShaderType::Mesh) != 0)           result |= uint32_t(vk::PipelineStageFlagBits2::eMeshShaderNV);
		if ((shaderType & ShaderType::AllRayTracing) != 0)  result |= uint32_t(vk::PipelineStageFlagBits2::eRayTracingShaderKHR); // or eRayTracingShaderNV, they have the same value

		return VkPipelineStageFlagBits2(result);
	}

	VkShaderStageFlagBits VkUtil::convertShaderTypeToShaderStageFlagBits(ShaderType shaderType)
	{
		if (shaderType == ShaderType::All)
			return (VkShaderStageFlagBits)vk::ShaderStageFlagBits::eAll;

		if (shaderType == ShaderType::AllGraphics)
			return (VkShaderStageFlagBits)vk::ShaderStageFlagBits::eAllGraphics;

#if ENABLE_SHORTCUT_CONVERSIONS
		static_assert(uint32_t(ShaderType::Vertex) == uint32_t(VK_SHADER_STAGE_VERTEX_BIT));
		static_assert(uint32_t(ShaderType::Hull) == uint32_t(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT));
		static_assert(uint32_t(ShaderType::Domain) == uint32_t(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT));
		static_assert(uint32_t(ShaderType::Geometry) == uint32_t(VK_SHADER_STAGE_GEOMETRY_BIT));
		static_assert(uint32_t(ShaderType::Pixel) == uint32_t(VK_SHADER_STAGE_FRAGMENT_BIT));
		static_assert(uint32_t(ShaderType::Compute) == uint32_t(VK_SHADER_STAGE_COMPUTE_BIT));
		static_assert(uint32_t(ShaderType::Amplification) == uint32_t(VK_SHADER_STAGE_TASK_BIT_NV));
		static_assert(uint32_t(ShaderType::Mesh) == uint32_t(VK_SHADER_STAGE_MESH_BIT_NV));
		static_assert(uint32_t(ShaderType::RayGeneration) == uint32_t(VK_SHADER_STAGE_RAYGEN_BIT_KHR));
		static_assert(uint32_t(ShaderType::ClosestHit) == uint32_t(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
		static_assert(uint32_t(ShaderType::AnyHit) == uint32_t(VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
		static_assert(uint32_t(ShaderType::Miss) == uint32_t(VK_SHADER_STAGE_MISS_BIT_KHR));
		static_assert(uint32_t(ShaderType::Intersection) == uint32_t(VK_SHADER_STAGE_INTERSECTION_BIT_KHR));
		static_assert(uint32_t(ShaderType::Callable) == uint32_t(VK_SHADER_STAGE_CALLABLE_BIT_KHR));

		return (VkShaderStageFlagBits)vk::ShaderStageFlagBits(shaderType);
#else
		uint32_t result = 0;

		if ((shaderType & ShaderType::Compute) != 0)        result |= uint32_t(VkShaderStageFlagBits::eCompute);
		if ((shaderType & ShaderType::Vertex) != 0)         result |= uint32_t(VkShaderStageFlagBits::eVertex);
		if ((shaderType & ShaderType::Hull) != 0)           result |= uint32_t(VkShaderStageFlagBits::eTessellationControl);
		if ((shaderType & ShaderType::Domain) != 0)         result |= uint32_t(VkShaderStageFlagBits::eTessellationEvaluation);
		if ((shaderType & ShaderType::Geometry) != 0)       result |= uint32_t(VkShaderStageFlagBits::eGeometry);
		if ((shaderType & ShaderType::Pixel) != 0)          result |= uint32_t(VkShaderStageFlagBits::eFragment);
		if ((shaderType & ShaderType::Amplification) != 0)  result |= uint32_t(VkShaderStageFlagBits::eTaskNV);
		if ((shaderType & ShaderType::Mesh) != 0)           result |= uint32_t(VkShaderStageFlagBits::eMeshNV);
		if ((shaderType & ShaderType::RayGeneration) != 0)  result |= uint32_t(VkShaderStageFlagBits::eRaygenKHR); // or eRaygenNV, they have the same value
		if ((shaderType & ShaderType::Miss) != 0)           result |= uint32_t(VkShaderStageFlagBits::eMissKHR);   // same etc...
		if ((shaderType & ShaderType::ClosestHit) != 0)     result |= uint32_t(VkShaderStageFlagBits::eClosestHitKHR);
		if ((shaderType & ShaderType::AnyHit) != 0)         result |= uint32_t(VkShaderStageFlagBits::eAnyHitKHR);
		if ((shaderType & ShaderType::Intersection) != 0)   result |= uint32_t(VkShaderStageFlagBits::eIntersectionKHR);

		return vk::ShaderStageFlagBits(result);
#endif
	}


	struct ResourceStateMappingInternal
	{
		ResourceStates nvrhiState;
		VkPipelineStageFlags2 stageFlags;
		VkAccessFlags2 accessMask;
		VkImageLayout imageLayout;

		ResourceStateMapping AsResourceStateMapping() const
		{
			// It's safe to cast vk::AccessFlags2 -> vk::AccessFlags and vk::PipelineStageFlags2 -> vk::PipelineStageFlags (as long as the enum exist in both versions!),
			// synchronization2 spec says: "The new flags are identical to the old values within the 32-bit range, with new stages and bits beyond that."
			// The below stages are exclustive to synchronization2
			assert((stageFlags & VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT) != VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT);
			assert((accessMask & VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT) != VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT);
			return
				ResourceStateMapping(nvrhiState,
					reinterpret_cast<const VkPipelineStageFlags&>(stageFlags),
					reinterpret_cast<const VkAccessFlags&>(accessMask),
					imageLayout
				);
		}

		ResourceStateMapping2 AsResourceStateMapping2() const
		{
			return ResourceStateMapping2(nvrhiState, stageFlags, accessMask, imageLayout);
		}
	};

	static const ResourceStateMappingInternal g_ResourceStateMap[] =
	{
		{ ResourceStates::Common,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eTopOfPipe,
			VkAccessFlags2(),
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::ConstantBuffer,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eAllCommands,
		   (VkAccessFlags2)vk::AccessFlagBits2::eUniformRead,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::VertexBuffer,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eVertexInput,
		   (VkAccessFlags2)vk::AccessFlagBits2::eVertexAttributeRead,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::IndexBuffer,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eVertexInput,
		   (VkAccessFlags2)vk::AccessFlagBits2::eIndexRead,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::IndirectArgument,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eDrawIndirect,
		   (VkAccessFlags2)vk::AccessFlagBits2::eIndirectCommandRead,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::ShaderResource,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eAllCommands,
		   (VkAccessFlags2)vk::AccessFlagBits2::eShaderRead,
			(VkImageLayout)vk::ImageLayout::eShaderReadOnlyOptimal },
		{ ResourceStates::UnorderedAccess,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eAllCommands,
		   (VkAccessFlags2)vk::AccessFlagBits2::eShaderRead | (VkAccessFlags2)vk::AccessFlagBits2::eShaderWrite,
			(VkImageLayout)vk::ImageLayout::eGeneral },
		{ ResourceStates::RenderTarget,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		   (VkAccessFlags2)vk::AccessFlagBits2::eColorAttachmentRead | (VkAccessFlags2)vk::AccessFlagBits2::eColorAttachmentWrite,
			(VkImageLayout)vk::ImageLayout::eColorAttachmentOptimal },
		{ ResourceStates::DepthWrite,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eEarlyFragmentTests | (VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eLateFragmentTests,
		   (VkAccessFlags2)vk::AccessFlagBits2::eDepthStencilAttachmentRead | (VkAccessFlags2)vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
			(VkImageLayout)vk::ImageLayout::eDepthStencilAttachmentOptimal },
		{ ResourceStates::DepthRead,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eEarlyFragmentTests | (VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eLateFragmentTests,
		   (VkAccessFlags2)vk::AccessFlagBits2::eDepthStencilAttachmentRead,
			(VkImageLayout)vk::ImageLayout::eDepthStencilReadOnlyOptimal },
		{ ResourceStates::StreamOut,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eTransformFeedbackEXT,
		   (VkAccessFlags2)vk::AccessFlagBits2::eTransformFeedbackWriteEXT,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::CopyDest,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eTransfer,
		   (VkAccessFlags2)vk::AccessFlagBits2::eTransferWrite,
			(VkImageLayout)vk::ImageLayout::eTransferDstOptimal },
		{ ResourceStates::CopySource,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eTransfer,
		   (VkAccessFlags2)vk::AccessFlagBits2::eTransferRead,
			(VkImageLayout)vk::ImageLayout::eTransferSrcOptimal },
		{ ResourceStates::ResolveDest,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eTransfer,
		   (VkAccessFlags2)vk::AccessFlagBits2::eTransferWrite,
			(VkImageLayout)vk::ImageLayout::eTransferDstOptimal },
		{ ResourceStates::ResolveSource,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eTransfer,
		   (VkAccessFlags2)vk::AccessFlagBits2::eTransferRead,
			(VkImageLayout)vk::ImageLayout::eTransferSrcOptimal },
		{ ResourceStates::Present,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eAllCommands,
		   (VkAccessFlags2)vk::AccessFlagBits2::eMemoryRead,
			(VkImageLayout)vk::ImageLayout::ePresentSrcKHR },
		{ ResourceStates::AccelStructRead,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eRayTracingShaderKHR | (VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eComputeShader,
		   (VkAccessFlags2)vk::AccessFlagBits2::eAccelerationStructureReadKHR,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::AccelStructWrite,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR,
		   (VkAccessFlags2)vk::AccessFlagBits2::eAccelerationStructureWriteKHR,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::AccelStructBuildInput,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR,
		   (VkAccessFlags2)vk::AccessFlagBits2::eAccelerationStructureReadKHR,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::AccelStructBuildBlas,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR,
		   (VkAccessFlags2)vk::AccessFlagBits2::eAccelerationStructureReadKHR,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::ShadingRateSurface,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eFragmentShadingRateAttachmentKHR,
		   (VkAccessFlags2)vk::AccessFlagBits2::eFragmentShadingRateAttachmentReadKHR,
			(VkImageLayout)vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR },
		{ ResourceStates::OpacityMicromapWrite,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eMicromapBuildEXT,
		   (VkAccessFlags2)vk::AccessFlagBits2::eMicromapWriteEXT,
			(VkImageLayout)vk::ImageLayout::eUndefined },
		{ ResourceStates::OpacityMicromapBuildInput,
			(VkPipelineStageFlagBits2)vk::PipelineStageFlagBits2::eMicromapBuildEXT,
		   (VkAccessFlags2)vk::AccessFlagBits2::eShaderRead,
			(VkImageLayout)vk::ImageLayout::eUndefined },
	};
	ResourceStateMappingInternal convertResourceStateInternal(ResourceStates state)
	{
		ResourceStateMappingInternal result = {};

		constexpr uint32_t numStateBits = sizeof(g_ResourceStateMap) / sizeof(g_ResourceStateMap[0]);

		uint32_t stateTmp = uint32_t(state);
		uint32_t bitIndex = 0;

		while (stateTmp != 0 && bitIndex < numStateBits)
		{
			uint32_t bit = (1 << bitIndex);

			if (stateTmp & bit)
			{
				const ResourceStateMappingInternal& mapping = g_ResourceStateMap[bitIndex];

				assert(uint32_t(mapping.nvrhiState) == bit);
				assert(result.imageLayout == VK_IMAGE_LAYOUT_UNDEFINED || mapping.imageLayout == VK_IMAGE_LAYOUT_UNDEFINED || result.imageLayout == mapping.imageLayout);

				result.nvrhiState = ResourceStates(result.nvrhiState | mapping.nvrhiState);
				result.accessMask |= mapping.accessMask;
				result.stageFlags |= mapping.stageFlags;
				if (mapping.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
					result.imageLayout = mapping.imageLayout;

				stateTmp &= ~bit;
			}

			bitIndex++;
		}

		assert(result.nvrhiState == state);

		return result;
	}

	ResourceStateMapping VkUtil::convertResourceState(ResourceStates state)
	{
		const ResourceStateMappingInternal mapping = convertResourceStateInternal(state);
		return mapping.AsResourceStateMapping();
	}

	ResourceStateMapping2 VkUtil::convertResourceState2(ResourceStates state)
	{
		const ResourceStateMappingInternal mapping = convertResourceStateInternal(state);
		return mapping.AsResourceStateMapping2();
	}

	const char* resultToString(VkResult result)
	{
		switch (result)
		{
		case VK_SUCCESS:
			return "VK_SUCCESS";
		case VK_NOT_READY:
			return "VK_NOT_READY";
		case VK_TIMEOUT:
			return "VK_TIMEOUT";
		case VK_EVENT_SET:
			return "VK_EVENT_SET";
		case VK_EVENT_RESET:
			return "VK_EVENT_RESET";
		case VK_INCOMPLETE:
			return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED:
			return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST:
			return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED:
			return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT:
			return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT:
			return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS:
			return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL:
			return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_UNKNOWN:
			return "VK_ERROR_UNKNOWN";
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			return "VK_ERROR_OUT_OF_POOL_MEMORY";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
		case VK_ERROR_FRAGMENTATION:
			return "VK_ERROR_FRAGMENTATION";
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
		case VK_ERROR_SURFACE_LOST_KHR:
			return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR:
			return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR:
			return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT:
			return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV:
			return "VK_ERROR_INVALID_SHADER_NV";
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
		case VK_ERROR_NOT_PERMITTED_EXT:
			return "VK_ERROR_NOT_PERMITTED_EXT";
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
		case VK_THREAD_IDLE_KHR:
			return "VK_THREAD_IDLE_KHR";
		case VK_THREAD_DONE_KHR:
			return "VK_THREAD_DONE_KHR";
		case VK_OPERATION_DEFERRED_KHR:
			return "VK_OPERATION_DEFERRED_KHR";
		case VK_OPERATION_NOT_DEFERRED_KHR:
			return "VK_OPERATION_NOT_DEFERRED_KHR";
		case VK_PIPELINE_COMPILE_REQUIRED_EXT:
			return "VK_PIPELINE_COMPILE_REQUIRED_EXT";

		default: {
			// Print the value into a static buffer - this is not thread safe but that shouldn't matter
			static char buf[24];
			snprintf(buf, sizeof(buf), "Unknown (%d)", result);
			return buf;
		}
		}
	}

	/*const char* resultToString(VkResult result)
	{
		return resultToString(result);
	}*/

	VkPrimitiveTopology VkUtil::convertPrimitiveTopology(PrimitiveType topology)
	{
		switch (topology)
		{
		case PrimitiveType::PointList:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

		case PrimitiveType::LineList:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

		case PrimitiveType::TriangleList:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		case PrimitiveType::TriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

		case PrimitiveType::TriangleFan:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

		case PrimitiveType::TriangleListWithAdjacency:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

		case PrimitiveType::TriangleStripWithAdjacency:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

		case PrimitiveType::PatchList:
			return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

		default:
			assert(0);
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	VkPolygonMode VkUtil::convertFillMode(RasterFillMode mode)
	{
		switch (mode)
		{
		case RasterFillMode::Fill:
			return VK_POLYGON_MODE_FILL;

		case RasterFillMode::Line:
			return VK_POLYGON_MODE_LINE;

		default:
			assert(0);
			return VK_POLYGON_MODE_FILL;
		}
	}

	VkCullModeFlagBits VkUtil::convertCullMode(RasterCullMode mode)
	{
		switch (mode)
		{
		case RasterCullMode::Back:
			return VK_CULL_MODE_BACK_BIT;

		case RasterCullMode::Front:
			return VK_CULL_MODE_FRONT_BIT;

		case RasterCullMode::None:
			return VK_CULL_MODE_NONE;

		default:
			assert(0);
			return VK_CULL_MODE_NONE;
		}
	}

	VkCompareOp VkUtil::convertCompareOp(ComparisonFunc op)
	{
		switch (op)
		{
		case ComparisonFunc::Never:
			return VK_COMPARE_OP_NEVER;

		case ComparisonFunc::Less:
			return VK_COMPARE_OP_LESS;

		case ComparisonFunc::Equal:
			return VK_COMPARE_OP_EQUAL;

		case ComparisonFunc::LessOrEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;

		case ComparisonFunc::Greater:
			return VK_COMPARE_OP_GREATER;

		case ComparisonFunc::NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;

		case ComparisonFunc::GreaterOrEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;

		case ComparisonFunc::Always:
			return VK_COMPARE_OP_ALWAYS;

		default:
			GE_INVALID_ENUM();
			return VK_COMPARE_OP_ALWAYS;
		}
	}

	VkStencilOp VkUtil::convertStencilOp(StencilOp op)
	{
		switch (op)
		{
		case StencilOp::Keep:
			return VK_STENCIL_OP_KEEP;

		case StencilOp::Zero:
			return VK_STENCIL_OP_ZERO;

		case StencilOp::Replace:
			return VK_STENCIL_OP_REPLACE;

		case StencilOp::IncrementAndClamp:
			return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

		case StencilOp::DecrementAndClamp:
			return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

		case StencilOp::Invert:
			return VK_STENCIL_OP_INVERT;

		case StencilOp::IncrementAndWrap:
			return VK_STENCIL_OP_INCREMENT_AND_WRAP;

		case StencilOp::DecrementAndWrap:
			return VK_STENCIL_OP_DECREMENT_AND_WRAP;

		default:
			GE_INVALID_ENUM();
			return VK_STENCIL_OP_KEEP;
		}
	}

	VkStencilOpState VkUtil::convertStencilState(const DepthStencilState& depthStencilState, const DepthStencilState::StencilOpDesc& desc)
	{
		/*return VkStencilOpState()
			.setFailOp(convertStencilOp(desc.failOp))
			.setPassOp(convertStencilOp(desc.passOp))
			.setDepthFailOp(convertStencilOp(desc.depthFailOp))
			.setCompareOp(convertCompareOp(desc.stencilFunc))
			.setCompareMask(depthStencilState.stencilReadMask)
			.setWriteMask(depthStencilState.stencilWriteMask)
			.setReference(depthStencilState.stencilRefValue);*/
		VkStencilOpState stencilState;
		stencilState.failOp = convertStencilOp(desc.failOp);
		stencilState.passOp = convertStencilOp(desc.passOp);
		stencilState.depthFailOp = convertStencilOp(desc.depthFailOp);
		stencilState.compareOp = convertCompareOp(desc.stencilFunc);
		stencilState.compareMask = depthStencilState.stencilReadMask;
		stencilState.writeMask = depthStencilState.stencilWriteMask;
		stencilState.reference = depthStencilState.stencilRefValue;
			
	

		return stencilState;
	}

	VkBlendFactor VkUtil::convertBlendValue(BlendFactor value)
	{
		switch (value)
		{
		case BlendFactor::Zero:
			return VK_BLEND_FACTOR_ZERO;

		case BlendFactor::One:
			return VK_BLEND_FACTOR_ONE;

		case BlendFactor::SrcColor:
			return VK_BLEND_FACTOR_SRC_COLOR;

		case BlendFactor::OneMinusSrcColor:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

		case BlendFactor::SrcAlpha:
			return VK_BLEND_FACTOR_SRC_ALPHA;

		case BlendFactor::OneMinusSrcAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

		case BlendFactor::DstAlpha:
			return VK_BLEND_FACTOR_DST_ALPHA;

		case BlendFactor::OneMinusDstAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

		case BlendFactor::DstColor:
			return VK_BLEND_FACTOR_DST_COLOR;

		case BlendFactor::OneMinusDstColor:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

		case BlendFactor::SrcAlphaSaturate:
			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

		case BlendFactor::ConstantColor:
			return VK_BLEND_FACTOR_CONSTANT_COLOR;

		case BlendFactor::OneMinusConstantColor:
			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

		case BlendFactor::Src1Color:
			return VK_BLEND_FACTOR_SRC1_COLOR;

		case BlendFactor::OneMinusSrc1Color:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

		case BlendFactor::Src1Alpha:
			return VK_BLEND_FACTOR_SRC1_ALPHA;

		case BlendFactor::OneMinusSrc1Alpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

		default:
			assert(0);
			return VK_BLEND_FACTOR_ZERO;
		}
	}

	VkBlendOp VkUtil::convertBlendOp(BlendOp op)
	{
		switch (op)
		{
		case BlendOp::Add:
			return VK_BLEND_OP_ADD;

		case BlendOp::Subrtact:
			return VK_BLEND_OP_SUBTRACT;

		case BlendOp::ReverseSubtract:
			return VK_BLEND_OP_REVERSE_SUBTRACT;

		case BlendOp::Min:
			return VK_BLEND_OP_MIN;

		case BlendOp::Max:
			return VK_BLEND_OP_MAX;

		default:
			assert(0);
			return VK_BLEND_OP_ADD;
		}
	}

	VkColorComponentFlags VkUtil::convertColorMask(ColorMask mask)
	{
		return VkColorComponentFlags(uint8_t(mask));
	}

	VkPipelineColorBlendAttachmentState VkUtil::convertBlendState(const BlendState::RenderTarget& state)
	{
		VkPipelineColorBlendAttachmentState  colorBlendAttachment{};
		colorBlendAttachment.blendEnable = state.blendEnable;
		colorBlendAttachment.srcColorBlendFactor = convertBlendValue(state.srcBlend);
		colorBlendAttachment.dstColorBlendFactor = convertBlendValue(state.destBlend);
		colorBlendAttachment.colorBlendOp = convertBlendOp(state.blendOp);
		colorBlendAttachment.srcAlphaBlendFactor = convertBlendValue(state.srcBlendAlpha);
		colorBlendAttachment.dstAlphaBlendFactor = convertBlendValue(state.destBlendAlpha);
		colorBlendAttachment.alphaBlendOp = convertBlendOp(state.blendOpAlpha);
		colorBlendAttachment.colorWriteMask = convertColorMask(state.colorWriteMask);

		return colorBlendAttachment;
		/*return VkPipelineColorBlendAttachmentState()
			.setBlendEnable(state.blendEnable)
			.setSrcColorBlendFactor(convertBlendValue(state.srcBlend))
			.setDstColorBlendFactor(convertBlendValue(state.destBlend))
			.setColorBlendOp(convertBlendOp(state.blendOp))
			.setSrcAlphaBlendFactor(convertBlendValue(state.srcBlendAlpha))
			.setDstAlphaBlendFactor(convertBlendValue(state.destBlendAlpha))
			.setAlphaBlendOp(convertBlendOp(state.blendOpAlpha))
			.setColorWriteMask(convertColorMask(state.colorWriteMask));*/
	}

	/*VkBuildAccelerationStructureFlagsKHR VkUtil::convertAccelStructBuildFlags(rt::AccelStructBuildFlags buildFlags)
	{
#if ENABLE_SHORTCUT_CONVERSIONS
		static_assert(uint32_t(rt::AccelStructBuildFlags::AllowUpdate) == uint32_t(VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR));
		static_assert(uint32_t(rt::AccelStructBuildFlags::AllowCompaction) == uint32_t(VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR));
		static_assert(uint32_t(rt::AccelStructBuildFlags::PreferFastTrace) == uint32_t(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR));
		static_assert(uint32_t(rt::AccelStructBuildFlags::PreferFastBuild) == uint32_t(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR));
		static_assert(uint32_t(rt::AccelStructBuildFlags::MinimizeMemory) == uint32_t(VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR));

		return VkBuildAccelerationStructureFlagsKHR(uint32_t(buildFlags) & 0x1f);
#else
		VkBuildAccelerationStructureFlagsKHR flags = VkBuildAccelerationStructureFlagBitsKHR(0);
		if ((buildFlags & rt::AccelStructBuildFlags::AllowUpdate) != 0)
			flags |= VkBuildAccelerationStructureFlagBitsKHR::eAllowUpdate;
		if ((buildFlags & rt::AccelStructBuildFlags::AllowCompaction) != 0)
			flags |= VkBuildAccelerationStructureFlagBitsKHR::eAllowCompaction;
		if ((buildFlags & rt::AccelStructBuildFlags::PreferFastTrace) != 0)
			flags |= VkBuildAccelerationStructureFlagBitsKHR::ePreferFastTrace;
		if ((buildFlags & rt::AccelStructBuildFlags::PreferFastBuild) != 0)
			flags |= VkBuildAccelerationStructureFlagBitsKHR::ePreferFastBuild;
		if ((buildFlags & rt::AccelStructBuildFlags::MinimizeMemory) != 0)
			flags |= VkBuildAccelerationStructureFlagBitsKHR::eLowMemory;
		return flags;
#endif
	}

	VkGeometryInstanceFlagsKHR VkUtil::convertInstanceFlags(rt::InstanceFlags instanceFlags)
	{
#if ENABLE_SHORTCUT_CONVERSIONS
		static_assert(uint32_t(rt::InstanceFlags::TriangleCullDisable) == uint32_t(VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR));
		static_assert(uint32_t(rt::InstanceFlags::TriangleFrontCounterclockwise) == uint32_t(VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_KHR));
		static_assert(uint32_t(rt::InstanceFlags::ForceOpaque) == uint32_t(VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR));
		static_assert(uint32_t(rt::InstanceFlags::ForceNonOpaque) == uint32_t(VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR));
		static_assert(uint32_t(rt::InstanceFlags::ForceOMM2State) == uint32_t(VK_GEOMETRY_INSTANCE_FORCE_OPACITY_MICROMAP_2_STATE_EXT));
		static_assert(uint32_t(rt::InstanceFlags::DisableOMMs) == uint32_t(VK_GEOMETRY_INSTANCE_DISABLE_OPACITY_MICROMAPS_EXT));

		return VkGeometryInstanceFlagsKHR(uint32_t(instanceFlags));
#else
		VkGeometryInstanceFlagsKHR flags = VkGeometryInstanceFlagBitsKHR(0);
		if ((instanceFlags & rt::InstanceFlags::ForceNonOpaque) != 0)
			flags |= VkGeometryInstanceFlagBitsKHR::eForceNoOpaque;
		if ((instanceFlags & rt::InstanceFlags::ForceOpaque) != 0)
			flags |= VkGeometryInstanceFlagBitsKHR::eForceOpaque;
		if ((instanceFlags & rt::InstanceFlags::ForceOMM2State) != 0)
			flags |= VkGeometryInstanceFlagBitsKHR::eForceOpacityMicromap2StateEXT;
		if ((instanceFlags & rt::InstanceFlags::DisableOMMs) != 0)
			flags |= VkGeometryInstanceFlagBitsKHR::eDisableOpacityMicromapsEXT;
		if ((instanceFlags & rt::InstanceFlags::TriangleCullDisable) != 0)
			flags |= VkGeometryInstanceFlagBitsKHR::eTriangleCullDisable;
		if ((instanceFlags & rt::InstanceFlags::TriangleFrontCounterclockwise) != 0)
			flags |= VkGeometryInstanceFlagBitsKHR::eTriangleFrontCounterclockwise;
		return flags;
#endif
	}*/

	VkExtent2D VkUtil::convertFragmentShadingRate(VariableShadingRate shadingRate)
	{
		VkExtent2D extent2d{};
		switch (shadingRate)
		{
		case VariableShadingRate::e1x2: {
			extent2d.width = 1;
			extent2d.height = 2;
		}
		case VariableShadingRate::e2x1: {
			extent2d.width = 2;
			extent2d.height = 1;
		}
		case VariableShadingRate::e2x2: {
			extent2d.width = 2;
			extent2d.height = 2;
		}
		case VariableShadingRate::e2x4: {
			extent2d.width = 2;
			extent2d.height = 4;
		}
		case VariableShadingRate::e4x2: {
			extent2d.width = 4;
			extent2d.height = 2;
		}
		case VariableShadingRate::e4x4: {
			extent2d.width = 4;
			extent2d.height = 4;
		}
			//return VkExtent2D().setWidth(4).setHeight(4);
		case VariableShadingRate::e1x1: {
			extent2d.width = 1;
			extent2d.height = 1;
		}
		default: {
			extent2d.width = 1;
			extent2d.height = 1;
		}
		return extent2d;
		}
	}

	VkFragmentShadingRateCombinerOpKHR VkUtil::convertShadingRateCombiner(ShadingRateCombiner combiner)
	{
		switch (combiner)
		{
		case ShadingRateCombiner::Override:
			return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
		case ShadingRateCombiner::Min:
			return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;
		case ShadingRateCombiner::Max:
			return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
		case ShadingRateCombiner::ApplyRelative:
			return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
		case ShadingRateCombiner::Passthrough:
		default:
			return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
		}
	}

	VkBorderColor VkUtil::pickSamplerBorderColor(const SamplerDesc& d)
	{
		if (d.borderColor.r == 0.f && d.borderColor.g == 0.f && d.borderColor.b == 0.f)
		{
			if (d.borderColor.a == 0.f)
			{
				return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			}

			if (d.borderColor.a == 1.f)
			{
				return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
			}
		}

		if (d.borderColor.r == 1.f && d.borderColor.g == 1.f && d.borderColor.b == 1.f)
		{
			if (d.borderColor.a == 1.f)
			{
				return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			}
		}

		GE_INVALID_NOTSUPPORT();
		return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	}
	void VkUtil::countSpecializationConstants(
		EShader* shader,
		size_t& numShaders,
		size_t& numShadersWithSpecializations,
		size_t& numSpecializationConstants) {

		if (!shader)
			return;

		numShaders += 1;

		if (shader->specializationConstants.empty())
			return;

		numShadersWithSpecializations += 1;
		numSpecializationConstants += shader->specializationConstants.size();

	}

	VkPipelineShaderStageCreateInfo VkUtil::makeShaderStageCreateInfo(
		EShader* shader,
		std::vector<VkSpecializationInfo>& specInfos,
		std::vector<VkSpecializationMapEntry>& specMapEntries,
		std::vector<uint32_t>& specData)
	{
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo;

		shaderStageCreateInfo.stage = shader->stageFlagBits;
		shaderStageCreateInfo.module = shader->shaderModule;
		shaderStageCreateInfo.pName = shader->desc.entryName.c_str();

		if (!shader->specializationConstants.empty())
		{
			// For specializations, this functions allocates:
			//  - One entry in specInfos per shader
			//  - One entry in specMapEntries and specData each per constant
			// The vectors are pre-allocated, so it's safe to use .data() before writing the data

			assert(specInfos.data());
			assert(specMapEntries.data());
			assert(specData.data());

			//特化常量： https://zhuanlan.zhihu.com/p/599147627
			//https://zhuanlan.zhihu.com/p/624916570

			shaderStageCreateInfo.pSpecializationInfo = (specInfos.data() + specInfos.size());

			/*auto specInfo = vk::SpecializationInfo()
				.setPMapEntries(specMapEntries.data() + specMapEntries.size())
				.setMapEntryCount(static_cast<uint32_t>(shader->specializationConstants.size()))
				.setPData(specData.data() + specData.size())
				.setDataSize(shader->specializationConstants.size() * sizeof(uint32_t));*/

			VkSpecializationInfo specInfo;
			specInfo.pMapEntries = specMapEntries.data() + specMapEntries.size();
			specInfo.mapEntryCount = static_cast<uint32_t>(shader->specializationConstants.size());
			specInfo.pData = specData.data() + specData.size();
			specInfo.dataSize = shader->specializationConstants.size() * sizeof(uint32_t);


			size_t dataOffset = 0;
			for (const auto& constant : shader->specializationConstants)
			{
				/*auto specMapEntry = ;
					.setConstantID(constant.constantID)
					.setOffset(static_cast<uint32_t>(dataOffset))
					.setSize(sizeof(uint32_t));*/

					VkSpecializationMapEntry specMapEntry;
					specMapEntry.constantID = constant.constantID;
					specMapEntry.offset = static_cast<uint32_t>(dataOffset);
					specMapEntry.size = sizeof(uint32_t);

				specMapEntries.push_back(specMapEntry);
				specData.push_back(constant.value.u);
				dataOffset += specMapEntry.size;
			}

			specInfos.push_back(specInfo);
		}

		return shaderStageCreateInfo;
	}

	VkFormat VkUtil::convertFormat(Format format)
	{
		assert(format < Format::COUNT);
		assert(c_FormatMap[uint32_t(format)].rhiFormat == format);

		return c_FormatMap[uint32_t(format)].vkFormat;
	}

} // namespace nvrhi::vulkan
