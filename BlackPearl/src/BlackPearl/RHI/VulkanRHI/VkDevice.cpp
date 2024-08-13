#include "pch.h"
#if GE_API_VULKAN

#include "VkUtil.h"
#include "VkEnum.h"
#include "VkDevice.h"
#include "VkBindingLayout.h"
#include "VkBindingSet.h"
#include "VkSampler.h"
#include "VkPipeline.h"
#include "VkCommandList.h"
#include "VkFrameBuffer.h"
#include "VkInputLayout.h"
#include "VkShader.h"
#include "VkQuery.h"
#include "VkQueue.h"
#include "VkTexture.h"
#include "VkDescriptorTable.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "../Common/Containers.h"
#include "../Common/FormatInfo.h"
#include <vulkan/vulkan.h>
#include "BlackPearl/Core.h"
#include "vulkan/vulkan_core.h"

namespace BlackPearl {
	template <typename T>
	using attachment_vector = nvrhi::static_vector<T, c_MaxRenderTargets + 1>; // render targets + depth

	static TextureDimension getDimensionForFramebuffer(TextureDimension dimension, bool isArray)
	{
		// Can't render into cubes and 3D textures directly, convert them to 2D arrays
		if (dimension == TextureDimension::TextureCube || dimension == TextureDimension::TextureCubeArray || dimension == TextureDimension::Texture3D)
			dimension = TextureDimension::Texture2DArray;

		if (!isArray)
		{
			// Demote arrays to single textures if we just need one layer
			switch (dimension)  // NOLINT(clang-diagnostic-switch-enum)
			{
			case TextureDimension::Texture1DArray:
				dimension = TextureDimension::Texture1D;
				break;
			case TextureDimension::Texture2DArray:
				dimension = TextureDimension::Texture2D;
				break;
			case TextureDimension::Texture2DMSArray:
				dimension = TextureDimension::Texture2DMS;
				break;
			default:
				break;
			}
		}

		return dimension;
	}

	static ETexture::TextureSubresourceViewType getTextureViewType(Format bindingFormat, Format textureFormat)
	{
		Format format = (bindingFormat == Format::UNKNOWN) ? textureFormat : bindingFormat;

		const FormatInfo& formatInfo = getFormatInfo(format);

		if (formatInfo.hasDepth)
			return ETexture::TextureSubresourceViewType::DepthOnly;
		else if (formatInfo.hasStencil)
			return ETexture::TextureSubresourceViewType::StencilOnly;
		else
			return ETexture::TextureSubresourceViewType::AllAspects;
	}
	Device::Device(const DeviceDesc& desc)
		:m_Context(desc.instance, desc.physicalDevice, desc.device, reinterpret_cast<VkAllocationCallbacks*>(desc.allocationCallbacks))
		, m_Allocator(m_Context)
	{

		if (desc.graphicsQueue)
		{
			m_Queues[uint32_t(CommandQueue::Graphics)] = std::make_unique<Queue>(m_Context,
				CommandQueue::Graphics, desc.graphicsQueue, desc.graphicsQueueIndex);
		}

		if (desc.computeQueue)
		{
			m_Queues[uint32_t(CommandQueue::Compute)] = std::make_unique<Queue>(m_Context,
				CommandQueue::Compute, desc.computeQueue, desc.computeQueueIndex);
		}

		if (desc.transferQueue)
		{
			m_Queues[uint32_t(CommandQueue::Copy)] = std::make_unique<Queue>(m_Context,
				CommandQueue::Copy, desc.transferQueue, desc.transferQueueIndex);
		}

		// maps Vulkan extension strings into the corresponding boolean flags in Device
		const std::unordered_map<std::string, bool*> extensionStringMap = {
			{ VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, &m_Context.extensions.KHR_synchronization2 },
			{ VK_KHR_MAINTENANCE1_EXTENSION_NAME, &m_Context.extensions.KHR_maintenance1 },
			{ VK_EXT_DEBUG_REPORT_EXTENSION_NAME, &m_Context.extensions.EXT_debug_report },
			{ VK_EXT_DEBUG_MARKER_EXTENSION_NAME, &m_Context.extensions.EXT_debug_marker },
			{ VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, &m_Context.extensions.KHR_acceleration_structure },
			{ VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, &m_Context.extensions.buffer_device_address },
			{ VK_KHR_RAY_QUERY_EXTENSION_NAME,&m_Context.extensions.KHR_ray_query },
			{ VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, &m_Context.extensions.KHR_ray_tracing_pipeline },
			{ VK_NV_MESH_SHADER_EXTENSION_NAME, &m_Context.extensions.NV_mesh_shader },
			{ VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, &m_Context.extensions.EXT_conservative_rasterization},
			{ VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, &m_Context.extensions.KHR_fragment_shading_rate },
			{ VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME, &m_Context.extensions.EXT_opacity_micromap },
			{ VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME, &m_Context.extensions.NV_ray_tracing_invocation_reorder },
		};

		// parse the extension/layer lists and figure out which extensions are enabled
		for (size_t i = 0; i < desc.numInstanceExtensions; i++)
		{
			auto ext = extensionStringMap.find(desc.instanceExtensions[i]);
			if (ext != extensionStringMap.end()){
	
				*(ext->second) = true;
			}
		}

		for (size_t i = 0; i < desc.numDeviceExtensions; i++)
		{
			auto ext = extensionStringMap.find(desc.deviceExtensions[i]);
			if (ext != extensionStringMap.end())
			{
				*(ext->second) = true;
			}
		}

		// The Vulkan 1.2 way of enabling bufferDeviceAddress
		if (desc.bufferDeviceAddressSupported)
			m_Context.extensions.buffer_device_address = true;

		void* pNext = nullptr;
		VkPhysicalDeviceAccelerationStructurePropertiesKHR accelStructProperties{};
		accelStructProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};
		rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
		VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterizationProperties{};
		conservativeRasterizationProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT;
		VkPhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateProperties{};
		shadingRateProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
		VkPhysicalDeviceOpacityMicromapPropertiesEXT opacityMicromapProperties{};
		opacityMicromapProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_PROPERTIES_EXT;
		VkPhysicalDeviceRayTracingInvocationReorderPropertiesNV nvRayTracingInvocationReorderProperties{};
		nvRayTracingInvocationReorderProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_PROPERTIES_NV;
		VkPhysicalDeviceProperties2 deviceProperties2{};
		deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

		if (m_Context.extensions.KHR_acceleration_structure)
		{
			accelStructProperties.pNext = pNext;
			pNext = &accelStructProperties;
		}

		if (m_Context.extensions.KHR_ray_tracing_pipeline)
		{
			rayTracingPipelineProperties.pNext = pNext;
			pNext = &rayTracingPipelineProperties;
		}

		if (m_Context.extensions.KHR_fragment_shading_rate)
		{
			shadingRateProperties.pNext = pNext;
			pNext = &shadingRateProperties;
		}

		if (m_Context.extensions.EXT_conservative_rasterization)
		{
			conservativeRasterizationProperties.pNext = pNext;
			pNext = &conservativeRasterizationProperties;
		}

		if (m_Context.extensions.EXT_opacity_micromap)
		{
			opacityMicromapProperties.pNext = pNext;
			pNext = &opacityMicromapProperties;
		}

		if (m_Context.extensions.NV_ray_tracing_invocation_reorder)
		{
			nvRayTracingInvocationReorderProperties.pNext = pNext;
			pNext = &nvRayTracingInvocationReorderProperties;
		}

		deviceProperties2.pNext = pNext;

		vkGetPhysicalDeviceProperties2(m_Context.physicalDevice, &deviceProperties2);
		//m_Context.physicalDevice.getProperties2(&deviceProperties2);

		m_Context.physicalDeviceProperties = deviceProperties2.properties;
		m_Context.accelStructProperties = accelStructProperties;
		m_Context.rayTracingPipelineProperties = rayTracingPipelineProperties;
		m_Context.conservativeRasterizationProperties = conservativeRasterizationProperties;
		m_Context.shadingRateProperties = shadingRateProperties;
		m_Context.opacityMicromapProperties = opacityMicromapProperties;
		m_Context.nvRayTracingInvocationReorderProperties = nvRayTracingInvocationReorderProperties;
		m_Context.messageCallback = desc.errorCB;

		if (m_Context.extensions.EXT_opacity_micromap && !m_Context.extensions.KHR_synchronization2)
		{
			m_Context.warning(
				"EXT_opacity_micromap is used without KHR_synchronization2 which is nessesary for OMM Array state transitions. Feature::RayTracingOpacityMicromap will be disabled.");
		}

		if (m_Context.extensions.KHR_fragment_shading_rate)
		{
			VkPhysicalDeviceFeatures2 deviceFeatures2{};
			deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures{};
			shadingRateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
			deviceFeatures2.pNext = &shadingRateFeatures;
			vkGetPhysicalDeviceFeatures2(m_Context.physicalDevice, &deviceFeatures2);
			//m_Context.physicalDevice.getFeatures2(&deviceFeatures2);
			m_Context.shadingRateFeatures = shadingRateFeatures;
		}
#ifdef NVRHI_WITH_RTXMU
		if (m_Context.extensions.KHR_acceleration_structure)
		{
			m_Context.rtxMemUtil = std::make_unique<rtxmu::VkAccelStructManager>(desc.instance, desc.device, desc.physicalDevice);

			// Initialize suballocator blocks to 8 MB
			m_Context.rtxMemUtil->Initialize(8388608);

			m_Context.rtxMuResources = std::make_unique<RtxMuResources>();
		}

		if (m_Context.extensions.EXT_opacity_micromap)
		{
			m_Context.warning("Opacity micro-maps are not currently supported by RTXMU.");
		}
#endif
		//auto pipelineInfo = vk::PipelineCacheCreateInfo();
		//vk::Result res = m_Context.device.createPipelineCache(&pipelineInfo,
		//	m_Context.allocationCallbacks,
		//	&m_Context.pipelineCache);

		//if (res != vk::Result::eSuccess)
		//{
		//	m_Context.error("Failed to create the pipeline cache");
		//}

		VkPipelineCacheCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VkResult res =  vkCreatePipelineCache(m_Context.device, &pipelineInfo,
				m_Context.allocationCallbacks,
				&m_Context.pipelineCache);
		if (res != VK_SUCCESS)
		{
			m_Context.error("Failed to create the pipeline cache");
		}
	}

	Device::~Device()
	{
	}

	Queue* Device::getQueue(CommandQueue queue) const
	{
		return m_Queues[int(queue)].get(); 
	}

	TextureHandle Device::createTexture(TextureDesc& desc)
	{
		ETexture* texture = new ETexture(m_Context, m_Allocator);
		assert(texture);
		ETexture::fillTextureInfo(texture, desc);

		//VkResult res = m_Context.device.createImage(&texture->imageInfo, m_Context.allocationCallbacks, &texture->image);
		VkResult res = vkCreateImage(m_Context.device, &texture->imageInfo, m_Context.allocationCallbacks, &texture->image);
		assert(res == VkResult::VK_SUCCESS);
		//CHECK_VK_FAIL(res)

			//m_Context.nameVKObject(texture->image, vk::DebugReportObjectTypeEXT::eImage, desc.debugName.c_str());

		if (!desc.isVirtual)
		{
			res = m_Allocator.allocateTextureMemory(texture);
			assert(res == VkResult::VK_SUCCESS);

			//ASSERT_VK_OK(res);
			//CHECK_VK_FAIL(res)

				if ((desc.sharedResourceFlags & SharedResourceFlags::Shared) != 0)
				{
					assert(0);
//#ifdef _WIN32
					//vkGetMemoryWin32HandleKHR()
					//PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR = PFN_vkGetMemoryWin32HandleKHR(vkGetDeviceProcAddr(m_Context.device, "vkGetMemoryWin32HandleKHR"));
					//int fd;
					////Get a POSIX file descriptor for a memory object
					//vkGetMemoryFdKHR(m_Context.device, reinterpret_cast<const VkMemoryGetFdInfoKHR*>(texture->memory), &fd);
					//texture->sharedHandle = (void*)(size_t)fd;
					//texture->sharedHandle = m_Context.device.getMemoryWin32HandleKHR({ texture->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 });

//#else
					
					/*int fd;
					vkGetMemoryFdKHR(m_Context.device, reinterpret_cast<const VkMemoryGetFdInfoKHR*>(texture->memory), &fd);
					texture->sharedHandle = (void*)(size_t)fd;*/
					//texture->sharedHandle = (void*)(size_t)m_Context.device.getMemoryFdKHR({ texture->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd });
//#endif
				}

			//m_Context.nameVKObject(texture->memory, vk::DebugReportObjectTypeEXT::eDeviceMemory, desc.debugName.c_str());
		}

		return TextureHandle::Create(texture);
		//return TextureHandle();
	}

	BufferHandle Device::createBuffer(const BufferDesc& desc)
	{
		// Check some basic constraints first - the validation layer is expected to handle them too

		if (desc.isVolatile && desc.maxVersions == 0)
			return nullptr;

		if (desc.isVolatile && !desc.isConstantBuffer)
			return nullptr;

		if (desc.byteSize == 0)
			return nullptr;


		Buffer* buffer = new Buffer(m_Context, m_Allocator);
		buffer->desc = desc;

		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
			VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if (desc.isVertexBuffer)
			usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		if (desc.isIndexBuffer)
			usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		if (desc.isDrawIndirectArgs)
			usageFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

		if (desc.isConstantBuffer)
			usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		if (desc.structStride != 0 || desc.canHaveUAVs || desc.canHaveRawViews)
			usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		if (desc.canHaveTypedViews)
			usageFlags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

		if (desc.canHaveTypedViews && desc.canHaveUAVs)
			usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		if (desc.isAccelStructBuildInput)
			usageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

		if (desc.isAccelStructStorage)
			usageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;

		if (desc.isShaderBindingTable)
			usageFlags |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

		if (m_Context.extensions.buffer_device_address)
			usageFlags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;

		uint64_t size = desc.byteSize;

		if (desc.isVolatile)
		{
			assert(!desc.isVirtual);

			uint64_t alignment = m_Context.physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

			uint64_t atomSize = m_Context.physicalDeviceProperties.limits.nonCoherentAtomSize;
			alignment = std::max(alignment, atomSize);

			assert((alignment & (alignment - 1)) == 0); // check if it's a power of 2

			size = (size + alignment - 1) & ~(alignment - 1);
			buffer->desc.byteSize = size;

			size *= desc.maxVersions;

			buffer->versionTracking.resize(desc.maxVersions);
			std::fill(buffer->versionTracking.begin(), buffer->versionTracking.end(), 0);

			buffer->desc.cpuAccess = CpuAccessMode::Write; // to get the right memory type allocated
		}
		else if (desc.byteSize < 65536)
		{
			// vulkan allows for <= 64kb buffer updates to be done inline via vkCmdUpdateBuffer,
			// but the data size must always be a multiple of 4
			// enlarge the buffer slightly to allow for this
			size = (size + 3) & ~3ull;
		}

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		

#if _WIN32
		const auto handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
		const auto handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
#endif
		VkExternalMemoryBufferCreateInfo externalBuffer{  };
		externalBuffer.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO;
		externalBuffer.handleTypes = handleType;
		if (desc.sharedResourceFlags == SharedResourceFlags::Shared)
			bufferInfo.pNext = (&externalBuffer);
		VkResult res = vkCreateBuffer(m_Context.device, &bufferInfo, m_Context.allocationCallbacks, &buffer->buffer);
		//VkResult res = m_Context.device.createBuffer(&bufferInfo, m_Context.allocationCallbacks, &buffer->buffer);
		if (res != VkResult::VK_SUCCESS)
			assert(0);

		//m_Context.nameVKObject(VkBuffer(buffer->buffer), VkDebugReportObjectTypeEXT::eBuffer, desc.debugName.c_str());

		if (!desc.isVirtual)
		{
			res = m_Allocator.allocateBufferMemory(buffer, (usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0);
			if (res != VkResult::VK_SUCCESS)
				assert(0);

				//m_Context.nameVKObject(buffer->memory, VkDebugReportObjectTypeEXT::eDeviceMemory, desc.debugName.c_str());

			if (desc.isVolatile)
			{
				VkResult res = vkMapMemory(m_Context.device, buffer->memory, 0, size,0, &buffer->mappedMemory); //m_Context.device.mapMemory(buffer->memory, 0, size);
				assert(res == VkResult::VK_SUCCESS);
			}

			if (m_Context.extensions.buffer_device_address)
			{
				VkBufferDeviceAddressInfo addressInfo{};
				addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
				addressInfo.buffer = buffer->buffer;
				//auto addressInfo = VkBufferDeviceAddressInfo().setBuffer(buffer->buffer);
				buffer->deviceAddress = vkGetBufferDeviceAddress(m_Context.device, &addressInfo);
				//buffer->deviceAddress = m_Context.device.getBufferAddress(addressInfo);
			}

			if (desc.sharedResourceFlags == SharedResourceFlags::Shared)
			{
//#ifdef _WIN32
//				buffer->sharedHandle = m_Context.device.getMemoryWin32HandleKHR({ buffer->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 });
//#else
//				buffer->sharedHandle = (void*)(size_t)m_Context.device.getMemoryFdKHR({ buffer->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd });
//#endif
			}
		}

		return BufferHandle::Create(buffer);
	}

	FramebufferHandle Device::createFramebuffer(const FramebufferDesc& desc)
	{
		Framebuffer* fb = new Framebuffer(m_Context);
		fb->desc = desc;
		fb->framebufferInfo = FramebufferInfoEx(desc);

		attachment_vector<VkAttachmentDescription2> attachmentDescs(desc.colorAttachments.size());
		attachment_vector<VkAttachmentReference2> colorAttachmentRefs(desc.colorAttachments.size());
		VkAttachmentReference2 depthAttachmentRef{};

		nvrhi::static_vector<VkImageView, c_MaxRenderTargets + 1> attachmentViews;
		attachmentViews.resize(desc.colorAttachments.size());

		uint32_t numArraySlices = 0;

		for (uint32_t i = 0; i < desc.colorAttachments.size(); i++)
		{
			const auto& rt = desc.colorAttachments[i];
			ETexture* t = dynamic_cast<ETexture*>(rt.texture);

			assert(fb->framebufferInfo.width == std::max(t->desc.width >> rt.subresources.baseMipLevel, 1u));
			assert(fb->framebufferInfo.height == std::max(t->desc.height >> rt.subresources.baseMipLevel, 1u));

			const VkFormat attachmentFormat = (rt.format == Format::UNKNOWN ? t->imageInfo.format : VkFormat(VkUtil::convertFormat(rt.format)));

			attachmentDescs[i] = VkAttachmentDescription2{};
			attachmentDescs[i].sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			attachmentDescs[i].format = attachmentFormat;
			attachmentDescs[i].samples = t->imageInfo.samples;
			attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;// VK_ATTACHMENT_LOAD_OP_LOAD;
			attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			colorAttachmentRefs[i] = VkAttachmentReference2{};
			colorAttachmentRefs[i].sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			colorAttachmentRefs[i].attachment = i;
			colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			TextureSubresourceSet subresources = rt.subresources.resolve(t->desc, true);

			TextureDimension dimension = getDimensionForFramebuffer(t->desc.dimension, subresources.numArraySlices > 1);

			const auto& view = t->getSubresourceView(subresources, dimension, rt.format);
			attachmentViews[i] = view.view;

			fb->resources.push_back(rt.texture);

			if (numArraySlices)
				assert(numArraySlices == subresources.numArraySlices);
			else
				numArraySlices = subresources.numArraySlices;
		}

		// add depth/stencil attachment if present
		if (desc.depthAttachment.valid())
		{
			const auto& att = desc.depthAttachment;

			ETexture* texture = dynamic_cast<ETexture*>(att.texture);

			assert(fb->framebufferInfo.width == std::max(texture->desc.width >> att.subresources.baseMipLevel, 1u));
			assert(fb->framebufferInfo.height == std::max(texture->desc.height >> att.subresources.baseMipLevel, 1u));

			VkImageLayout depthLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			if (desc.depthAttachment.isReadOnly)
			{
				depthLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			}


			VkAttachmentDescription2 depthAttachment{};
			depthAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			depthAttachment.format = texture->imageInfo.format;
			depthAttachment.samples = texture->imageInfo.samples;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.initialLayout = depthLayout;
			depthAttachment.finalLayout = depthLayout;

			attachmentDescs.push_back(depthAttachment);


			depthAttachmentRef = VkAttachmentReference2{};
			depthAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			depthAttachmentRef.attachment = uint32_t(attachmentDescs.size()) - 1;
			depthAttachmentRef.layout = depthLayout;
			/*.setAttachment(uint32_t(attachmentDescs.size()) - 1)
			.setLayout(depthLayout);*/

			TextureSubresourceSet subresources = att.subresources.resolve(texture->desc, true);

			TextureDimension dimension = getDimensionForFramebuffer(texture->desc.dimension, subresources.numArraySlices > 1);

			const TextureSubresourceView& view = texture->getSubresourceView(subresources, dimension, att.format);
			attachmentViews.push_back(view.view);

			fb->resources.push_back(att.texture);

			if (numArraySlices)
				assert(numArraySlices == subresources.numArraySlices);
			else
				numArraySlices = subresources.numArraySlices;
		}

		VkSubpassDescription2 subpass{};
		subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
		/*auto subpass = VkSubpassDescription2()
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(uint32_t(desc.colorAttachments.size()))
			.setPColorAttachments(colorAttachmentRefs.data())
			.setPDepthStencilAttachment(desc.depthAttachment.valid() ? &depthAttachmentRef : nullptr);*/

		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = uint32_t(desc.colorAttachments.size());
		subpass.pColorAttachments = colorAttachmentRefs.data();
		subpass.pDepthStencilAttachment = desc.depthAttachment.valid() ? &depthAttachmentRef : nullptr;



		// add VRS attachment
		// declare the structures here to avoid using pointers to out-of-scope objects in renderPassInfo further
		VkAttachmentReference2 vrsAttachmentRef{};
		VkFragmentShadingRateAttachmentInfoKHR shadingRateAttachmentInfo{};

		if (desc.shadingRateAttachment.valid())
		{
			const auto& vrsAttachment = desc.shadingRateAttachment;
			ETexture* vrsTexture = dynamic_cast<ETexture*>(vrsAttachment.texture);
			assert(vrsTexture->imageInfo.format == VK_FORMAT_R8_UINT);
			assert(vrsTexture->imageInfo.samples == VK_SAMPLE_COUNT_1_BIT);
			//auto vrsAttachmentDesc = vk::AttachmentDescription2()
			//	.setFormat(vk::Format::eR8Uint)
			//	.setSamples(vk::SampleCountFlagBits::e1)
			//	.setLoadOp(vk::AttachmentLoadOp::eLoad)
			//	.setStoreOp(vk::AttachmentStoreOp::eStore)
			//	.setInitialLayout(vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR)
			//	.setFinalLayout(vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR);


			VkAttachmentDescription2 vrsAttachmentDesc{};
			vrsAttachmentDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			vrsAttachmentDesc.format = VK_FORMAT_R8_UINT;
			vrsAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			vrsAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			vrsAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			vrsAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
			vrsAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

			attachmentDescs.push_back(vrsAttachmentDesc);

			TextureSubresourceSet subresources = vrsAttachment.subresources.resolve(vrsTexture->desc, true);
			TextureDimension dimension = getDimensionForFramebuffer(vrsTexture->desc.dimension, subresources.numArraySlices > 1);

			const auto& view = vrsTexture->getSubresourceView(subresources, dimension, vrsAttachment.format);
			attachmentViews.push_back(view.view);

			fb->resources.push_back(vrsAttachment.texture);

			if (numArraySlices)
				assert(numArraySlices == subresources.numArraySlices);
			else
				numArraySlices = subresources.numArraySlices;

			VkPhysicalDeviceFragmentShadingRatePropertiesKHR rateProps{};
			VkPhysicalDeviceProperties2 props{};
			props.pNext = &rateProps;
			vkGetPhysicalDeviceProperties2(m_Context.physicalDevice, &props);
			/*m_Context.physicalDevice.getProperties2(&props);*/

			VkAttachmentReference2 vrsAttachmentRef{};
			/*		.setAttachment(uint32_t(attachmentDescs.size()) - 1)
					.setLayout(vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR);*/
			vrsAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			vrsAttachmentRef.attachment = uint32_t(attachmentDescs.size()) - 1;
			vrsAttachmentRef.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

			VkFragmentShadingRateAttachmentInfoKHR shadingRateAttachmentInfo{};
			shadingRateAttachmentInfo.pFragmentShadingRateAttachment = &vrsAttachmentRef;
			shadingRateAttachmentInfo.shadingRateAttachmentTexelSize = rateProps.minFragmentShadingRateAttachmentTexelSize;

			/*.setPFragmentShadingRateAttachment(&vrsAttachmentRef)
			.setShadingRateAttachmentTexelSize(rateProps.minFragmentShadingRateAttachmentTexelSize);*/
			subpass.pNext = &shadingRateAttachmentInfo;
		}

		VkRenderPassCreateInfo2&& renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
		renderPassInfo.attachmentCount = uint32_t(attachmentDescs.size());
		renderPassInfo.pAttachments = attachmentDescs.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;


		if (vkCreateRenderPass2(m_Context.device, &renderPassInfo, m_Context.allocationCallbacks,
			&fb->renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}


		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = fb->renderPass;
		framebufferInfo.attachmentCount = uint32_t(attachmentViews.size());
		framebufferInfo.pAttachments = attachmentViews.data();
		framebufferInfo.width = fb->framebufferInfo.width;
		framebufferInfo.height = fb->framebufferInfo.height;
		framebufferInfo.layers = numArraySlices;

		if (vkCreateFramebuffer(m_Context.device, &framebufferInfo, m_Context.allocationCallbacks, &fb->framebuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}


		return FramebufferHandle::Create(fb);
	}

	void* Device::mapBuffer(IBuffer* b, CpuAccessMode mapFlags)
	{
		Buffer* buffer = static_cast<Buffer*>(b);

		return mapBuffer(buffer, mapFlags, 0, buffer->desc.byteSize);

	}

	void Device::unmapBuffer(IBuffer* b)
	{
	}

	MemoryRequirements Device::getBufferMemoryRequirements(IBuffer* _buffer)
	{
		Buffer* buffer = static_cast<Buffer*>(_buffer);

		VkMemoryRequirements vulkanMemReq;
		vkGetBufferMemoryRequirements(m_Context.device, buffer->buffer, &vulkanMemReq);
		//m_Context.device.getBufferMemoryRequirements(buffer->buffer, &vulkanMemReq);

		MemoryRequirements memReq;
		memReq.alignment = vulkanMemReq.alignment;
		memReq.size = vulkanMemReq.size;
		return memReq;
	}

	GraphicsPipelineHandle Device::createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* _fb)
	{
		if (desc.renderState.singlePassStereo.enabled)
		{
			m_Context.error("Single-pass stereo is not supported by the Vulkan backend");
			return nullptr;
		}

		VkResult res;

		Framebuffer* fb = dynamic_cast<Framebuffer*>(_fb);

		InputLayout* inputLayout = dynamic_cast<InputLayout*>(desc.inputLayout.Get());

		GraphicsPipeline* pso = new GraphicsPipeline(m_Context);
		pso->desc = desc;
		pso->framebufferInfo = fb->framebufferInfo;

		for (const BindingLayoutHandle& _layout : desc.bindingLayouts)
		{
			BindingLayout* layout = dynamic_cast<BindingLayout*>(_layout.Get());
			pso->pipelineBindingLayouts.push_back(layout);
		}

		EShader* VS = dynamic_cast<EShader*>(desc.VS.Get());
		EShader* HS = dynamic_cast<EShader*>(desc.HS.Get());
		EShader* DS = dynamic_cast<EShader*>(desc.DS.Get());
		EShader* GS = dynamic_cast<EShader*>(desc.GS.Get());
		EShader* PS = dynamic_cast<EShader*>(desc.PS.Get());

		size_t numShaders = 0;
		size_t numShadersWithSpecializations = 0;
		size_t numSpecializationConstants = 0;

		// Count the spec constants for all stages
		VkUtil::countSpecializationConstants(VS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		VkUtil::countSpecializationConstants(HS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		VkUtil::countSpecializationConstants(DS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		VkUtil::countSpecializationConstants(GS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		VkUtil::countSpecializationConstants(PS, numShaders, numShadersWithSpecializations, numSpecializationConstants);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		std::vector<VkSpecializationInfo> specInfos;
		std::vector<VkSpecializationMapEntry> specMapEntries;
		std::vector<uint32_t> specData;

		// Allocate buffers for specialization constants and related structures
		// so that shaderStageCreateInfo(...) can directly use pointers inside the vectors
		// because the vectors won't reallocate their buffers
		shaderStages.reserve(numShaders);
		specInfos.reserve(numShadersWithSpecializations);
		specMapEntries.reserve(numSpecializationConstants);
		specData.reserve(numSpecializationConstants);

		// Set up shader stages
		if (desc.VS)
		{
			shaderStages.push_back(VkUtil::makeShaderStageCreateInfo(VS,
				specInfos, specMapEntries, specData));
			pso->shaderMask = pso->shaderMask | ShaderType::Vertex;
		}

		if (desc.HS)
		{
			shaderStages.push_back(VkUtil::makeShaderStageCreateInfo(HS,
				specInfos, specMapEntries, specData));
			pso->shaderMask = pso->shaderMask | ShaderType::Hull;
		}

		if (desc.DS)
		{
			shaderStages.push_back(VkUtil::makeShaderStageCreateInfo(DS,
				specInfos, specMapEntries, specData));
			pso->shaderMask = pso->shaderMask | ShaderType::Domain;
		}

		if (desc.GS)
		{
			shaderStages.push_back(VkUtil::makeShaderStageCreateInfo(GS,
				specInfos, specMapEntries, specData));
			pso->shaderMask = pso->shaderMask | ShaderType::Geometry;
		}

		if (desc.PS)
		{
			shaderStages.push_back(VkUtil::makeShaderStageCreateInfo(PS,
				specInfos, specMapEntries, specData));
			pso->shaderMask = pso->shaderMask | ShaderType::Pixel;
		}

		// set up vertex input state
		VkPipelineVertexInputStateCreateInfo vertexInput{};
		if (inputLayout)
		{
			vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInput.vertexBindingDescriptionCount = uint32_t(inputLayout->bindingDesc.size());
			vertexInput.pVertexBindingDescriptions = inputLayout->bindingDesc.data();
			vertexInput.vertexAttributeDescriptionCount = uint32_t(inputLayout->attributeDesc.size());
			vertexInput.pVertexAttributeDescriptions = inputLayout->attributeDesc.data();
		}

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VkUtil::convertPrimitiveTopology(desc.primType);
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// fixed function state
		const auto& rasterState = desc.renderState.rasterState;
		const auto& depthStencilState = desc.renderState.depthStencilState;
		const auto& blendState = desc.renderState.blendState;


		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.polygonMode = VkUtil::convertFillMode(rasterState.fillMode);
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VkUtil::convertCullMode(rasterState.cullMode);
		rasterizer.frontFace = rasterState.frontCounterClockwise ?
			VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;// VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = rasterState.depthBias ? true : false;
		rasterizer.depthBiasConstantFactor = float(rasterState.depthBias);
		rasterizer.depthBiasClamp = rasterState.depthBiasClamp;
		rasterizer.depthBiasSlopeFactor = rasterState.slopeScaledDepthBias;

		// Conservative raster state
		VkPipelineRasterizationConservativeStateCreateInfoEXT conservativeRasterState{};
		conservativeRasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT;
		conservativeRasterState.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;

		if (rasterState.conservativeRasterEnable)
		{
			rasterizer.pNext = &conservativeRasterState;
		}

		VkPipelineMultisampleStateCreateInfo multisample{};
		multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample.rasterizationSamples = VkSampleCountFlagBits(fb->framebufferInfo.sampleCount);
		multisample.alphaToCoverageEnable = blendState.alphaToCoverageEnable;


		VkPipelineDepthStencilStateCreateInfo depthStencil{};

		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = depthStencilState.depthTestEnable;
		depthStencil.depthWriteEnable = depthStencilState.depthWriteEnable;
		depthStencil.depthCompareOp = VkUtil::convertCompareOp(depthStencilState.depthFunc);
		depthStencil.stencilTestEnable = depthStencilState.stencilEnable;
		depthStencil.front = VkUtil::convertStencilState(depthStencilState, depthStencilState.frontFaceStencil);
		depthStencil.back = VkUtil::convertStencilState(depthStencilState, depthStencilState.backFaceStencil);
		// VRS state

		VkFragmentShadingRateCombinerOpKHR combiners[2] =
		{ VkUtil::convertShadingRateCombiner(desc.shadingRateState.pipelinePrimitiveCombiner),
			VkUtil::convertShadingRateCombiner(desc.shadingRateState.imageCombiner)
		};
		VkPipelineFragmentShadingRateStateCreateInfoKHR shadingRateState{};
		shadingRateState.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR;
		shadingRateState.combinerOps[0] = combiners[0];
		shadingRateState.combinerOps[1] = combiners[1];
		shadingRateState.fragmentSize = VkUtil::convertFragmentShadingRate(desc.shadingRateState.shadingRate);

		/*.setCombinerOps(combiners)
		.setFragmentSize(convertFragmentShadingRate(desc.shadingRateState.shadingRate));*/
		nvrhi::static_vector<VkDescriptorSetLayout, c_MaxBindingLayouts> descriptorSetLayouts;
		uint32_t pushConstantSize = 0;
		for (const BindingLayoutHandle& _layout : desc.bindingLayouts)
		{
			BindingLayout* layout = dynamic_cast<BindingLayout*>(_layout.Get());
			descriptorSetLayouts.push_back(layout->descriptorSetLayout);

			if (!layout->isBindless)
			{
				for (const RHIBindingLayoutItem& item : layout->desc.bindings)
				{
					if (item.type == RHIResourceType::RT_PushConstants)
					{
						pushConstantSize = item.size;
						// assume there's only one push constant item in all layouts -- the validation layer makes sure of that
						break;
					}
				}
			}
		}
		VkPushConstantRange pushConstantRange{};

		pushConstantRange.offset = 0;
		pushConstantRange.size = pushConstantSize;
		pushConstantRange.stageFlags = VkUtil::convertShaderTypeToShaderStageFlagBits(pso->shaderMask);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = uint32_t(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = pushConstantSize ? 1 : 0;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Context.device, &pipelineLayoutInfo,
			m_Context.allocationCallbacks,
			&pso->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		attachment_vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(fb->desc.colorAttachments.size());

		for (uint32_t i = 0; i < uint32_t(fb->desc.colorAttachments.size()); i++)
		{
			colorBlendAttachments[i] = VkUtil::convertBlendState(blendState.targets[i]);
		}

		VkPipelineColorBlendStateCreateInfo colorBlend{};
		colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlend.attachmentCount = uint32_t(colorBlendAttachments.size());
		colorBlend.pAttachments = colorBlendAttachments.data();

		pso->usesBlendConstants = blendState.usesConstantColor(uint32_t(fb->desc.colorAttachments.size()));

		VkDynamicState dynamicStates[4] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_BLEND_CONSTANTS,
			VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = pso->usesBlendConstants ? 3 : 2;
		dynamicStateInfo.pDynamicStates = dynamicStates;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = uint32_t(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisample;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlend;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = pso->pipelineLayout;
		pipelineInfo.renderPass = fb->renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.pTessellationState = VK_NULL_HANDLE;
		pipelineInfo.pNext = &shadingRateState;

		VkPipelineTessellationStateCreateInfo tessellationState{};

		if (desc.primType == PrimitiveType::PatchList)
		{
			tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			tessellationState.patchControlPoints = desc.patchControlPoints;
			pipelineInfo.pTessellationState = &tessellationState;
		}
		if (vkCreateGraphicsPipelines(m_Context.device, m_Context.pipelineCache,
			1, &pipelineInfo,
			m_Context.allocationCallbacks,
			&pso->pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}
	
		return GraphicsPipelineHandle::Create(pso);
	}

	ComputePipelineHandle Device::createComputePipeline(const ComputePipelineDesc& desc)
	{
		return ComputePipelineHandle();
	}

	MeshletPipelineHandle Device::createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb)
	{
		return MeshletPipelineHandle();
	}

	CommandListHandle Device::createCommandList(const CommandListParameters& params)
	{
		if (!m_Queues[uint32_t(params.queueType)])
			return nullptr;

		CommandList* cmdList = new CommandList(this, m_Context, params);

		return CommandListHandle::Create(cmdList);
	}

	uint64_t Device::executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue)
	{
		Queue& queue = *m_Queues[uint32_t(executionQueue)];

		uint64_t submissionID = queue.submit(pCommandLists, numCommandLists);

		for (size_t i = 0; i < numCommandLists; i++)
		{
			dynamic_cast<CommandList*>(pCommandLists[i])->executed(queue, submissionID);
		}

		return submissionID;
	}

	SamplerHandle Device::createSampler(const SamplerDesc& desc)
	{
		Sampler* sampler = new Sampler(m_Context);

		const bool anisotropyEnable = desc.maxAnisotropy > 1.0f;


		sampler->desc = desc;
		//sampler->samplerInfo = vk::SamplerCreateInfo()
		//	.setMagFilter(desc.magFilter ? VK_FILTER_LINEAR : VK_FILTER_NEAREST)
		//	.setMinFilter(desc.minFilter ? VK_FILTER_LINEAR : VK_FILTER_NEAREST)
		//	.setMipmapMode(desc.mipFilter ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST)
		//	.setAddressModeU(VkUtil::convertSamplerAddressMode(desc.addressU))
		//	.setAddressModeV(VkUtil::convertSamplerAddressMode(desc.addressV))
		//	.setAddressModeW(VkUtil::convertSamplerAddressMode(desc.addressW))
		//	.setMipLodBias(desc.mipBias)
		//	.setAnisotropyEnable(anisotropyEnable)
		//	.setMaxAnisotropy(anisotropyEnable ? desc.maxAnisotropy : 1.f)
		//	.setCompareEnable(desc.reductionType == SamplerReductionType::Comparison)
		//	.setCompareOp(vk::CompareOp::eLess)
		//	.setMinLod(0.f)
		//	.setMaxLod(std::numeric_limits<float>::max())
		//	.setBorderColor(pickSamplerBorderColor(desc));

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = desc.magFilter ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		samplerInfo.minFilter = desc.minFilter ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
		samplerInfo.mipmapMode = desc.mipFilter ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerInfo.addressModeU = VkUtil::convertSamplerAddressMode(desc.addressU);
		samplerInfo.addressModeV = VkUtil::convertSamplerAddressMode(desc.addressV);
		samplerInfo.addressModeW = VkUtil::convertSamplerAddressMode(desc.addressW);
		samplerInfo.mipLodBias = desc.mipBias;
		samplerInfo.anisotropyEnable = anisotropyEnable;
		samplerInfo.maxAnisotropy = anisotropyEnable ? desc.maxAnisotropy : 1.f;
		samplerInfo.compareEnable = desc.reductionType == SamplerReductionType::Comparison;
		samplerInfo.compareOp = VK_COMPARE_OP_LESS;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = std::numeric_limits<float>::max();
		samplerInfo.borderColor = (VkBorderColor)VkUtil::pickSamplerBorderColor(desc);

		sampler->samplerInfo = samplerInfo;

		VkSamplerReductionModeCreateInfoEXT samplerReductionCreateInfo{};
		if (desc.reductionType == SamplerReductionType::Minimum || desc.reductionType == SamplerReductionType::Maximum)
		{
			VkSamplerReductionModeEXT reductionMode =
				desc.reductionType == SamplerReductionType::Maximum ? VK_SAMPLER_REDUCTION_MODE_MAX : VK_SAMPLER_REDUCTION_MODE_MIN;
			samplerReductionCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT;
			samplerReductionCreateInfo.reductionMode = reductionMode;

			sampler->samplerInfo.pNext = &samplerReductionCreateInfo;
		}
		if (vkCreateSampler(m_Context.device, &samplerInfo, m_Context.allocationCallbacks, &sampler->sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}


		return SamplerHandle::Create(sampler);
	}

	ShaderHandle Device::createShader(const ShaderDesc& d, const void* binary, size_t binarySize)
	{
		EShader* shader = new EShader(m_Context);

		shader->desc = d;
		shader->stageFlagBits = VkUtil::convertShaderTypeToShaderStageFlagBits(d.shaderType);

		VkShaderModuleCreateInfo shaderInfo{};
		shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderInfo.codeSize = binarySize;
		shaderInfo.pCode = (const uint32_t*)binary;

		/*auto shaderInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(binarySize)
			.setPCode((const uint32_t*)binary);*/
		//vkCreateShaderModule(m_Context.device, &shaderInfo, m_Context.allocationCallbacks, &shader->shaderModule);

		if (vkCreateShaderModule(m_Context.device, &shaderInfo, m_Context.allocationCallbacks, &shader->shaderModule) != VK_SUCCESS) {
			GE_CORE_ERROR("failed to create shader module!");
		}


		//const vk::Result res = m_Context.device.createShaderModule(&shaderInfo, m_Context.allocationCallbacks, &shader->shaderModule);
		//CHECK_VK_FAIL(res)

		const std::string debugName = d.debugName + ":" + d.entryName;
		
		m_Context.nameVKObject(VkShaderModule(shader->shaderModule), VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT, debugName.c_str());

		return ShaderHandle::Create(shader);
	}

	ShaderLibraryHandle Device::createShaderLibrary(const void* binary, size_t binarySize)
	{
		GE_ASSERT(0, "not complete yet");
		return ShaderLibraryHandle();
	}

	InputLayoutHandle Device::createInputLayout(const VertexAttributeDesc* attributeDesc, uint32_t attributeCount, IShader* vertexShader)
	{
		(void)vertexShader;

		InputLayout* layout = new InputLayout();

		int total_attribute_array_size = 0;

		// collect all buffer bindings
		std::unordered_map<uint32_t, VkVertexInputBindingDescription> bindingMap;
		for (uint32_t i = 0; i < attributeCount; i++)
		{
			const VertexAttributeDesc& desc = attributeDesc[i];

			assert(desc.arraySize > 0);

			total_attribute_array_size += desc.arraySize;

			if (bindingMap.find(desc.bufferIndex) == bindingMap.end())
			{
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = desc.bufferIndex;
				bindingDescription.stride = desc.elementStride;
				bindingDescription.inputRate = desc.isInstanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;

				bindingMap[desc.bufferIndex] = bindingDescription;
				
			}
			else {
				assert(bindingMap[desc.bufferIndex].stride == desc.elementStride);
				assert(bindingMap[desc.bufferIndex].inputRate == (desc.isInstanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX));
			}
		}

		for (const auto& b : bindingMap)
		{
			layout->bindingDesc.push_back(b.second);
		}

		// build attribute descriptions
		layout->inputDesc.resize(attributeCount);
		layout->attributeDesc.resize(total_attribute_array_size);

		uint32_t attributeLocation = 0;
		for (uint32_t i = 0; i < attributeCount; i++)
		{
			const VertexAttributeDesc& in = attributeDesc[i];
			layout->inputDesc[i] = in;

			uint32_t element_size_bytes = getFormatInfo(in.format).bytesPerBlock;

			uint32_t bufferOffset = 0;

			for (uint32_t slot = 0; slot < in.arraySize; ++slot)
			{
				auto& outAttrib = layout->attributeDesc[attributeLocation];

				outAttrib.location = attributeLocation;
				outAttrib.binding = in.bufferIndex;
				outAttrib.format = VkUtil::convertFormat(in.format);
				outAttrib.offset = bufferOffset + in.offset;
				bufferOffset += element_size_bytes;

				++attributeLocation;
			}
		}

		return InputLayoutHandle::Create(layout);
	}

	VkSemaphore Device::getQueueSemaphore(CommandQueue queueID)
	{
		Queue& queue = *m_Queues[uint32_t(queueID)];

		return queue.trackingSemaphore;
	}

	void Device::queueWaitForSemaphore(CommandQueue waitQueueID, VkSemaphore semaphore, uint64_t value)
	{
		Queue& waitQueue = *m_Queues[uint32_t(waitQueueID)];

		waitQueue.addWaitSemaphore(semaphore, value);
	}

	void Device::queueSignalSemaphore(CommandQueue executionQueueID, VkSemaphore semaphore, uint64_t value)
	{
		Queue& executionQueue = *m_Queues[uint32_t(executionQueueID)];

		executionQueue.addSignalSemaphore(semaphore, value);
	}

	uint64_t Device::queueGetCompletedInstance(CommandQueue queue)
	{
		uint64_t value;
		vkGetSemaphoreCounterValue(m_Context.device, getQueueSemaphore(queue), &value);
		return value;//m_Context.device.getSemaphoreCounterValue(getQueueSemaphore(queue));
	}

	bool Device::queryFeatureSupport(Feature feature, void* pInfo, size_t infoSize)
	{
		switch (feature)  // NOLINT(clang-diagnostic-switch-enum)
		{
		case Feature::DeferredCommandLists:
			return true;
		case Feature::RayTracingAccelStruct:
			return m_Context.extensions.KHR_acceleration_structure;
		case Feature::RayTracingPipeline:
			return m_Context.extensions.KHR_ray_tracing_pipeline;
		case Feature::RayTracingOpacityMicromap:
#ifdef NVRHI_WITH_RTXMU
			return false; // RTXMU does not support OMMs
#else
			return m_Context.extensions.EXT_opacity_micromap && m_Context.extensions.KHR_synchronization2;
#endif
		case Feature::RayQuery:
			return m_Context.extensions.KHR_ray_query;
		case Feature::ShaderExecutionReordering:
		{
			if (m_Context.extensions.NV_ray_tracing_invocation_reorder)
			{
				return VK_RAY_TRACING_INVOCATION_REORDER_MODE_REORDER_NV == m_Context.nvRayTracingInvocationReorderProperties.rayTracingInvocationReorderReorderingHint;
			}
			return false;
		}
		case Feature::ShaderSpecializations:
			return true;
		case Feature::Meshlets:
			return m_Context.extensions.NV_mesh_shader;
		case Feature::VariableRateShading:
			if (pInfo)
			{
				if (infoSize == sizeof(VariableRateShadingFeatureInfo))
				{
					auto* pVrsInfo = reinterpret_cast<VariableRateShadingFeatureInfo*>(pInfo);
					const auto& tileExtent = m_Context.shadingRateProperties.minFragmentShadingRateAttachmentTexelSize;
					pVrsInfo->shadingRateImageTileSize = std::max(tileExtent.width, tileExtent.height);
				}
				else
					RHIUtils::NotSupported();
			}
			return m_Context.extensions.KHR_fragment_shading_rate && m_Context.shadingRateFeatures.attachmentFragmentShadingRate;
		case Feature::ConservativeRasterization:
			return m_Context.extensions.EXT_conservative_rasterization;
		case Feature::VirtualResources:
			return true;
		case Feature::ComputeQueue:
			return (m_Queues[uint32_t(CommandQueue::Compute)] != nullptr);
		case Feature::CopyQueue:
			return (m_Queues[uint32_t(CommandQueue::Copy)] != nullptr);
		case Feature::ConstantBufferRanges:
			return true;
		default:
			return false;
		}
	}

	FramebufferHandle Device::createHandleForNativeFramebuffer(VkRenderPass renderPass, VkFramebuffer framebuffer, const FramebufferDesc& desc, bool transferOwnership)
	{
		return FramebufferHandle();
	}

	void Device::resizeDescriptorTable(IDescriptorTable* _descriptorTable, uint32_t newSize, bool keepContents)
	{
		assert(newSize <= static_cast<DescriptorTable*>(_descriptorTable)->layout->getBindlessDesc()->maxCapacity);
		(void)_descriptorTable;
		(void)newSize;
		(void)keepContents;
	}

	bool Device::writeDescriptorTable(IDescriptorTable* _descriptorTable, const BindingSetItem& binding)
	{
		DescriptorTable* descriptorTable = static_cast<DescriptorTable*>(_descriptorTable);
		BindingLayout* layout = static_cast<BindingLayout*>(descriptorTable->layout.Get());

		if (binding.slot >= descriptorTable->capacity)
			return false;

		VkResult res;

		// collect all of the descriptor write data
		nvrhi::static_vector<VkDescriptorImageInfo, c_MaxBindingsPerLayout> descriptorImageInfo;
		nvrhi::static_vector<VkDescriptorBufferInfo, c_MaxBindingsPerLayout> descriptorBufferInfo;
		nvrhi::static_vector<VkWriteDescriptorSet, c_MaxBindingsPerLayout> descriptorWriteInfo;

		auto generateWriteDescriptorData =
			// generates a vk::WriteDescriptorSet struct in descriptorWriteInfo
			[&](uint32_t bindingLocation,
				VkDescriptorType descriptorType,
				VkDescriptorImageInfo* imageInfo,
				VkDescriptorBufferInfo* bufferInfo,
				VkBufferView* bufferView)
			{
				/*descriptorWriteInfo.push_back(
					VkWriteDescriptorSet()
					.setDstSet(descriptorTable->descriptorSet)
					.setDstBinding(bindingLocation)
					.setDstArrayElement(binding.slot)
					.setDescriptorCount(1)
					.setDescriptorType(descriptorType)
					.setPImageInfo(imageInfo)
					.setPBufferInfo(bufferInfo)
					.setPTexelBufferView(bufferView)
				);*/



				VkWriteDescriptorSet descriptorWrite{};
				descriptorWrite.dstSet = descriptorTable->descriptorSet;
				descriptorWrite.dstBinding = bindingLocation;
				descriptorWrite.dstArrayElement = binding.slot;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.descriptorType = descriptorType;
				descriptorWrite.pImageInfo = imageInfo;
				descriptorWrite.pBufferInfo = bufferInfo;
				descriptorWrite.pTexelBufferView = bufferView;

				descriptorWriteInfo.push_back(descriptorWrite);
				
			};

		for (uint32_t bindingLocation = 0; bindingLocation < uint32_t(layout->bindlessDesc.registerSpaces.size()); bindingLocation++)
		{
			if (layout->bindlessDesc.registerSpaces[bindingLocation].type == binding.type)
			{
				const VkDescriptorSetLayoutBinding& layoutBinding = layout->vulkanLayoutBindings[bindingLocation];

				switch (binding.type)
				{
				case RHIResourceType::RT_Texture_SRV:
				{
					const auto& texture = static_cast<ETexture*>(binding.resourceHandle);

					const auto subresource = binding.subresources.resolve(texture->desc, false);
					const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
					auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);

					VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();


					//VkDescriptorImageInfo imageInfo;
					imageInfo.imageView = view.view;
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			/*		imageInfo = VkDescriptorImageInfo()
						.setImageView(view.view)
						.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);*/

					generateWriteDescriptorData(layoutBinding.binding,
						layoutBinding.descriptorType,
						&imageInfo, nullptr, nullptr);
				}

				break;

				case RHIResourceType::RT_Texture_UAV:
				{
					const auto texture = static_cast<ETexture*>(binding.resourceHandle);

					const auto subresource = binding.subresources.resolve(texture->desc, true);
					const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
					auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);

					VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();
					//imageInfo = vk::DescriptorImageInfo()
					//	.setImageView(view.view)
					//	.setImageLayout(vk::ImageLayout::eGeneral);


					//VkDescriptorImageInfo imageInfo;
					imageInfo.imageView = view.view;
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

					generateWriteDescriptorData(layoutBinding.binding,
						layoutBinding.descriptorType,
						&imageInfo, nullptr, nullptr);
				}

				break;

				case RHIResourceType::RT_TypedBuffer_SRV:
				case RHIResourceType::RT_TypedBuffer_UAV:
				{
					const auto& buffer = static_cast<Buffer*>(binding.resourceHandle);

					auto vkformat = VkUtil::convertFormat(binding.format);

					const auto range = binding.range.resolve(buffer->desc);
					uint64_t viewInfoHash = 0;
					hash_combine(viewInfoHash, range.byteOffset);
					hash_combine(viewInfoHash, range.byteSize);
					hash_combine(viewInfoHash, (uint64_t)vkformat);

					const auto& bufferViewFound = buffer->viewCache.find(viewInfoHash);
					auto& bufferViewRef = (bufferViewFound != buffer->viewCache.end()) ? bufferViewFound->second : buffer->viewCache[viewInfoHash];
					if (bufferViewFound == buffer->viewCache.end())
					{
						assert(binding.format != Format::UNKNOWN);

						VkBufferViewCreateInfo bufferViewInfo{};
						bufferViewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
						bufferViewInfo.buffer = buffer->buffer;
						bufferViewInfo.offset = range.byteOffset;
						bufferViewInfo.range = range.byteSize;
						bufferViewInfo.format = vkformat;

						//vkCreateBufferView(m_Context.device, &bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
						if (vkCreateBufferView(m_Context.device, &bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef) != VK_SUCCESS) {
							throw std::runtime_error("failed to create buffer view!");
						}
					/*	 VkBufferViewCreateInfo bufferViewInfo{}
							.setBuffer(buffer->buffer)
							.setOffset(range.byteOffset)
							.setRange(range.byteSize)
							.setFormat(vk::Format(vkformat));

						res = m_Context.device.createBufferView(&bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
						ASSERT_VK_OK(res);*/
					}

					generateWriteDescriptorData(layoutBinding.binding,
						layoutBinding.descriptorType,
						nullptr, nullptr, &bufferViewRef);
				}
				break;

				case RHIResourceType::RT_StructuredBuffer_SRV:
				case RHIResourceType::RT_StructuredBuffer_UAV:
				case RHIResourceType::RT_RawBuffer_SRV:
				case RHIResourceType::RT_RawBuffer_UAV:
				case RHIResourceType::RT_ConstantBuffer:
				case RHIResourceType::RT_VolatileConstantBuffer:
				{
					const auto buffer = static_cast<Buffer*>(binding.resourceHandle);

					const auto range = binding.range.resolve(buffer->desc);

					auto& bufferInfo = descriptorBufferInfo.emplace_back();
					//VkDescriptorBufferInfo bufferInfo{};
					
					bufferInfo.buffer = buffer->buffer;
					bufferInfo.offset = range.byteOffset;
					bufferInfo.range = range.byteSize;

					assert(buffer->buffer);
					generateWriteDescriptorData(layoutBinding.binding,
						layoutBinding.descriptorType,
						nullptr, &bufferInfo, nullptr);
				}

				break;

				case RHIResourceType::RT_Sampler:
				{
					const auto& sampler = static_cast<Sampler*>(binding.resourceHandle);

					VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();
					imageInfo.sampler = sampler->sampler;
				/*	imageInfo = vk::DescriptorImageInfo()
						.setSampler(sampler->sampler);*/

					generateWriteDescriptorData(layoutBinding.binding,
						layoutBinding.descriptorType,
						&imageInfo, nullptr, nullptr);
				}

				break;

				case RHIResourceType::RT_RayTracingAccelStruct:
					RHIUtils::NotImplemented();
					break;

				case RHIResourceType::RT_PushConstants:
					RHIUtils::NotSupported();
					break;

				case RHIResourceType::RT_None:
				case RHIResourceType::RT_Count:
				default:
					RHIUtils::InvalidEnum();
				}
			}
		}
		vkUpdateDescriptorSets(m_Context.device, uint32_t(descriptorWriteInfo.size()), descriptorWriteInfo.data(), 0, nullptr);
		//m_Context.device.updateDescriptorSets(uint32_t(descriptorWriteInfo.size()), descriptorWriteInfo.data(), 0, nullptr);

		return true;
	}

	FormatSupport Device::queryFormatSupport(Format format)
	{
		VkFormat vulkanFormat = VkUtil::convertFormat(format);

		VkFormatProperties props;
		//m_Context.physicalDevice.getFormatProperties(vk::Format(vulkanFormat), &props);
		//TODO::
		FormatSupport result = FormatSupport::None;

		//if (props.bufferFeatures)
		//	result = result | FormatSupport::Buffer;

		//if (format == Format::R32_UINT || format == Format::R16_UINT) {
		//	// There is no explicit bit in vk::FormatFeatureFlags for index buffers
		//	result = result | FormatSupport::IndexBuffer;
		//}

		//if (props.bufferFeatures & vk::FormatFeatureFlagBits::eVertexBuffer)
		//	result = result | FormatSupport::VertexBuffer;

		//if (props.optimalTilingFeatures)
		//	result = result | FormatSupport::Texture;

		//if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		//	result = result | FormatSupport::DepthStencil;

		//if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment)
		//	result = result | FormatSupport::RenderTarget;

		//if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachmentBlend)
		//	result = result | FormatSupport::Blendable;

		//if ((props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImage) ||
		//	(props.bufferFeatures & vk::FormatFeatureFlagBits::eUniformTexelBuffer))
		//{
		//	result = result | FormatSupport::ShaderLoad;
		//}

		//if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)
		//	result = result | FormatSupport::ShaderSample;

		//if ((props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eStorageImage) ||
		//	(props.bufferFeatures & vk::FormatFeatureFlagBits::eStorageTexelBuffer))
		//{
		//	result = result | FormatSupport::ShaderUavLoad;
		//	result = result | FormatSupport::ShaderUavStore;
		//}

		//if ((props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eStorageImageAtomic) ||
		//	(props.bufferFeatures & vk::FormatFeatureFlagBits::eStorageTexelBufferAtomic))
		//{
		//	result = result | FormatSupport::ShaderAtomic;
		//}

		return result;
	}

	EventQueryHandle Device::createEventQuery()
	{
		EventQuery* query = new EventQuery();
		return EventQueryHandle::Create(query);
	}

	void Device::setEventQuery(IEventQuery* _query, CommandQueue queue)
	{
		EventQuery* query = static_cast<EventQuery*>(_query);

		assert(query->commandListID == 0);

		query->queue = queue;
		query->commandListID = m_Queues[uint32_t(queue)]->getLastSubmittedID();
	}

	bool Device::pollEventQuery(IEventQuery* _query)
	{
		EventQuery* query = static_cast<EventQuery*>(_query);

		auto& queue = *m_Queues[uint32_t(query->queue)];

		return queue.pollCommandList(query->commandListID);
	}

	void Device::waitEventQuery(IEventQuery* _query)
	{
		EventQuery* query = static_cast<EventQuery*>(_query);

		if (query->commandListID == 0)
			return;

		auto& queue = *m_Queues[uint32_t(query->queue)];

		bool success = queue.waitCommandList(query->commandListID, ~0ull);
		assert(success);
		(void)success;
	}

	void Device::resetEventQuery(IEventQuery* _query)
	{
		EventQuery* query = static_cast<EventQuery*>(_query);

		query->commandListID = 0;
	}

	void generateWriteDescriptorData(uint32_t bindingLocation,
		VkDescriptorType descriptorType,
		VkDescriptorImageInfo* imageInfo,
		VkDescriptorBufferInfo* bufferInfo,
		VkBufferView* bufferView,
		VkWriteDescriptorSet* descriptorWriteInfo, 
		BindingSet* ret,int bindingIndex,
		const void* pNext = nullptr) 
	{
		//VkWriteDescriptorSet& descriptorWrites = descriptorWriteInfo[bindingIndex];
		descriptorWriteInfo[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWriteInfo[bindingIndex].descriptorType = descriptorType;
		descriptorWriteInfo[bindingIndex].dstSet = ret->descriptorSet;
		descriptorWriteInfo[bindingIndex].dstBinding = bindingLocation;
		descriptorWriteInfo[bindingIndex].dstArrayElement = 0;
		descriptorWriteInfo[bindingIndex].descriptorCount = 1;
		descriptorWriteInfo[bindingIndex].pImageInfo = imageInfo;
		descriptorWriteInfo[bindingIndex].pBufferInfo = bufferInfo;
		descriptorWriteInfo[bindingIndex].pTexelBufferView = bufferView;
		descriptorWriteInfo[bindingIndex].pNext = pNext;

	}
	

	BindingSetHandle Device::createBindingSet(const BindingSetDesc& desc, IBindingLayout* _layout)
	{
		BindingLayout* layout = dynamic_cast<BindingLayout*>(_layout);

		BindingSet* ret = new BindingSet(m_Context);
		ret->desc = desc;
		ret->layout = layout;

		const auto& descriptorSetLayout = layout->descriptorSetLayout;
		const auto& poolSizes = layout->descriptorPoolSizeInfo;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = uint32_t(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;

		if (vkCreateDescriptorPool(m_Context.device, &poolInfo, nullptr, &ret->descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = ret->descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;


		if (vkAllocateDescriptorSets(m_Context.device, &allocInfo, &ret->descriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		// collect all of the descriptor write data
		std::vector<VkDescriptorImageInfo> descriptorImageInfo;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfo;
		VkWriteDescriptorSet descriptorWriteInfo[200];
		VkDescriptorBufferInfo bufferinfos[100];
		VkDescriptorImageInfo imageInfos[100];
		VkBufferViewCreateInfo  bufferViewinfos[100];
		int descriptorCnt = 0;
		int bufferDescCnt = 0;
		int bufferViewDescCnt = 0;
		int ImageDescCnt = 0;

		std::vector<VkWriteDescriptorSetAccelerationStructureKHR> accelStructWriteInfo;

		for (size_t bindingIndex = 0; bindingIndex < desc.bindings.size(); bindingIndex++)
		{
			const BindingSetItem& binding = desc.bindings[bindingIndex];
			const VkDescriptorSetLayoutBinding& layoutBinding = layout->vulkanLayoutBindings[bindingIndex];

			if (binding.resourceHandle == nullptr)
			{
				continue;
			}

			ret->resources.push_back(binding.resourceHandle); // keep a strong reference to the resource

			switch (binding.type)
			{
			case RHIResourceType::RT_Texture_SRV:
			{
				ETexture* texture = dynamic_cast<ETexture*>(binding.resourceHandle);

				const TextureSubresourceSet subresource = binding.subresources.resolve(texture->desc, false);
				const ETexture::TextureSubresourceViewType textureViewType = getTextureViewType(binding.format, texture->desc.format);
				auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);

				imageInfos[ImageDescCnt].imageView = view.view;
				imageInfos[ImageDescCnt].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfos[ImageDescCnt], nullptr,nullptr, descriptorWriteInfo, ret, bindingIndex);

				ImageDescCnt++;
				descriptorCnt++;

				if (!static_cast<bool>(texture->permanentState))
					ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
				else
					verifyPermanentResourceState(texture->permanentState,
						ResourceStates::ShaderResource,
						true, texture->desc.debugName, m_Context.messageCallback);
			}

			break;

			case RHIResourceType::RT_Texture_UAV:
			{
				const auto texture = dynamic_cast<ETexture*>(binding.resourceHandle);

				const auto subresource = binding.subresources.resolve(texture->desc, true);
				const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
				auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);


				imageInfos[ImageDescCnt].imageView = view.view;
				imageInfos[ImageDescCnt].imageLayout = VK_IMAGE_LAYOUT_GENERAL;


				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfos[ImageDescCnt], nullptr, nullptr, descriptorWriteInfo, ret, bindingIndex);
				ImageDescCnt++;
				descriptorCnt++;

				if (!static_cast<bool>(texture->permanentState))
					ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
				else
					verifyPermanentResourceState(texture->permanentState,
						ResourceStates::UnorderedAccess,
						true, texture->desc.debugName, m_Context.messageCallback);
			}
			break;
			case RHIResourceType::RT_TypedBuffer_SRV:
			case RHIResourceType::RT_TypedBuffer_UAV:
			{
				const auto buffer = dynamic_cast<Buffer*>(binding.resourceHandle);

				assert(buffer->desc.canHaveTypedViews);
				const bool isUAV = (binding.type == RHIResourceType::RT_TypedBuffer_UAV);
				if (isUAV)
					assert(buffer->desc.canHaveUAVs);

				Format format = binding.format;

				if (format == Format::UNKNOWN)
				{
					format = buffer->desc.format;
				}

				auto vkformat = VkUtil::convertFormat(format);
				const auto range = binding.range.resolve(buffer->desc);

				uint64_t viewInfoHash = 0;
				hash_combine(viewInfoHash, range.byteOffset);
				hash_combine(viewInfoHash, range.byteSize);
				hash_combine(viewInfoHash, (uint64_t)vkformat);

				const auto& bufferViewFound = buffer->viewCache.find(viewInfoHash);
				auto& bufferViewRef = (bufferViewFound != buffer->viewCache.end()) ? bufferViewFound->second : buffer->viewCache[viewInfoHash];
				if (bufferViewFound == buffer->viewCache.end())
				{
					assert(format != Format::UNKNOWN);

					bufferViewinfos[bufferViewDescCnt].sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
					bufferViewinfos[bufferViewDescCnt].buffer = buffer->buffer;
					bufferViewinfos[bufferViewDescCnt].offset = range.byteOffset;
					bufferViewinfos[bufferViewDescCnt].range = range.byteSize;
					bufferViewinfos[bufferViewDescCnt].format = vkformat;

					if (vkCreateBufferView(m_Context.device, &bufferViewinfos[bufferViewDescCnt], m_Context.allocationCallbacks, &bufferViewRef) != VK_SUCCESS) {
						throw std::runtime_error("failed to create buffer view!");
					}
				}

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					nullptr, nullptr, &bufferViewRef, descriptorWriteInfo, ret, bindingIndex);

				bufferViewDescCnt++;
				descriptorCnt++;


				if (!static_cast<bool>(buffer->permanentState))
					ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
				else
					verifyPermanentResourceState(buffer->permanentState,
						isUAV ? ResourceStates::UnorderedAccess : ResourceStates::ShaderResource,
						false, buffer->desc.debugName, m_Context.messageCallback);
			}
			break;

			case RHIResourceType::RT_StructuredBuffer_SRV:
			case RHIResourceType::RT_StructuredBuffer_UAV:
			case RHIResourceType::RT_RawBuffer_SRV:
			case RHIResourceType::RT_RawBuffer_UAV:
			case RHIResourceType::RT_ConstantBuffer:
			case RHIResourceType::RT_VolatileConstantBuffer:
			{
				Buffer* buffer = static_cast<Buffer*>(binding.resourceHandle);

				if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
					assert(buffer->desc.canHaveUAVs);
				if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_StructuredBuffer_SRV)
					assert(buffer->desc.structStride != 0);
				if (binding.type == RHIResourceType::RT_RawBuffer_SRV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
					assert(buffer->desc.canHaveRawViews);

				const auto range = binding.range.resolve(buffer->desc);

				bufferinfos[bufferDescCnt].buffer = buffer->buffer;
				bufferinfos[bufferDescCnt].offset = range.byteOffset;
				bufferinfos[bufferDescCnt].range = range.byteSize;
			
				assert(buffer->buffer);

				descriptorWriteInfo[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWriteInfo[bindingIndex].descriptorType = layoutBinding.descriptorType;
				descriptorWriteInfo[bindingIndex].dstSet = ret->descriptorSet;
				descriptorWriteInfo[bindingIndex].dstBinding = layoutBinding.binding;
				descriptorWriteInfo[bindingIndex].dstArrayElement = 0;
				descriptorWriteInfo[bindingIndex].descriptorCount = 1;
				descriptorWriteInfo[bindingIndex].pImageInfo = nullptr;
				descriptorWriteInfo[bindingIndex].pBufferInfo = &bufferinfos[bufferDescCnt];
				descriptorWriteInfo[bindingIndex].pTexelBufferView = nullptr;
				descriptorWriteInfo[bindingIndex].pNext = nullptr;

				bufferDescCnt++;
				descriptorCnt++;


				if (binding.type == RHIResourceType::RT_VolatileConstantBuffer)
				{
					assert(buffer->desc.isVolatile);
					ret->volatileConstantBuffers.push_back(buffer);
				}
				else
				{
					if (!static_cast<bool>(buffer->permanentState))
						ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
					else
					{
						ResourceStates requiredState;
						if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
							requiredState = ResourceStates::UnorderedAccess;
						else if (binding.type == RHIResourceType::RT_ConstantBuffer)
							requiredState = ResourceStates::ConstantBuffer;
						else
							requiredState = ResourceStates::ShaderResource;

						verifyPermanentResourceState(buffer->permanentState, requiredState,
							false, buffer->desc.debugName, m_Context.messageCallback);
					}
				}
			}

			break;

			case RHIResourceType::RT_Sampler:
			{
				const auto sampler = dynamic_cast<Sampler*>(binding.resourceHandle);

				//VkDescriptorImageInfo imageInfo{};

				//imageInfo.sampler = sampler->sampler;
				////imageInfo = vk::DescriptorImageInfo()
				////	.setSampler(sampler->sampler);
				//descriptorImageInfo.push_back(imageInfo);

				imageInfos[ImageDescCnt].sampler = sampler->sampler;




				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfos[ImageDescCnt], nullptr, nullptr, descriptorWriteInfo, ret, bindingIndex);

				ImageDescCnt++;
				descriptorCnt++;

			}

			break;

			case RHIResourceType::RT_RayTracingAccelStruct:
			{
				/*const auto as = dynamic_cast<AccelStruct*>(binding.resourceHandle);

				auto& accelStructWrite = accelStructWriteInfo.emplace_back();
				accelStructWrite.accelerationStructureCount = 1;
				accelStructWrite.pAccelerationStructures = &as->accelStruct;

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					nullptr, nullptr, nullptr, &accelStructWrite);

				ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));*/
			}

			break;

			case RHIResourceType::RT_PushConstants:
				break;

			case RHIResourceType::RT_None:
			case RHIResourceType::RT_Count:
			default:
				assert(!"Invalid Enumeration Value");
				break;
			}
		}

		//m_Context.device.updateDescriptorSets(uint32_t(descriptorWriteInfo.size()), descriptorWriteInfo.data(), 0, nullptr);
		vkUpdateDescriptorSets(m_Context.device, uint32_t(descriptorCnt), descriptorWriteInfo, 0, nullptr);


		return BindingSetHandle::Create(ret);
	}

	BindingLayoutHandle Device::createBindingLayout(const RHIBindingLayoutDesc& desc)
	{
		BindingLayout* ret = new BindingLayout(m_Context, desc);

		ret->bake();

		return BindingLayoutHandle::Create(ret);
	}

	BindingLayoutHandle Device::createBindlessLayout(const RHIBindlessLayoutDesc& desc)
	{
		BindingLayout* ret = new BindingLayout(m_Context, desc);

		ret->bake();

		return BindingLayoutHandle::Create(ret);
	}

	DeviceHandle Device::createDevice(const DeviceDesc& desc)
	{
#if defined(NVRHI_SHARED_LIBRARY_BUILD)
		const vk::DynamicLoader dl;
		const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =   // NOLINT(misc-misplaced-const)
			dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(desc.instance);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(desc.device);
#endif

		Device* device = new Device(desc);
		return DeviceHandle::Create(device);
	}

	void* Device::mapBuffer(IBuffer* b, CpuAccessMode flags, uint64_t offset, size_t size) const
	{
		Buffer* buffer = static_cast<Buffer*>(b);

		assert(flags != CpuAccessMode::None);

		// If the buffer has been used in a command list before, wait for that CL to complete
		if (buffer->lastUseCommandListID != 0)
		{
			auto& queue = m_Queues[uint32_t(buffer->lastUseQueue)];
			queue->waitCommandList(buffer->lastUseCommandListID, ~0ull);
		}

		VkAccessFlags accessFlags;

		switch (flags)
		{
		case CpuAccessMode::Read:
			accessFlags = VK_ACCESS_HOST_READ_BIT;
			break;

		case CpuAccessMode::Write:
			accessFlags = VK_ACCESS_MEMORY_WRITE_BIT;
			break;

		case CpuAccessMode::None:
		default:
			RHIUtils::InvalidEnum();
			break;
		}

		// TODO: there should be a barrier... But there can't be a command list here
		// buffer->barrier(cmd, vk::PipelineStageFlagBits::eHost, accessFlags);

		void* ptr = nullptr;
		VkResult res = vkMapMemory(m_Context.device, buffer->memory, 0, size, 0, &ptr);
		//[[maybe_unused]] const VkResult res = m_Context.device.mapMemory(buffer->memory, offset, size, vk::MemoryMapFlags(), &ptr);
		assert(res == VkResult::VK_SUCCESS);

		return ptr;
	}

}
#endif
