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
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "../Common/Containers.h"
#include "../Common/FormatInfo.h"
#include <vulkan/vulkan.h>
#include "BlackPearl/Core.h"

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
		VkPhysicalDeviceAccelerationStructurePropertiesKHR accelStructProperties;
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties;
		VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterizationProperties;
		VkPhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateProperties;
		VkPhysicalDeviceOpacityMicromapPropertiesEXT opacityMicromapProperties;
		VkPhysicalDeviceRayTracingInvocationReorderPropertiesNV nvRayTracingInvocationReorderProperties;
		
		VkPhysicalDeviceProperties2 deviceProperties2;

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
			VkPhysicalDeviceFeatures2 deviceFeatures2;
			VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures;
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

		VkPipelineCacheCreateInfo pipelineInfo;
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

	TextureHandle Device::createTexture(const TextureDesc& d)
	{
//		ETexture* texture = new ETexture(m_Context, m_Allocator);
//		assert(texture);
//		fillTextureInfo(texture, desc);
//
//		vk::Result res = m_Context.device.createImage(&texture->imageInfo, m_Context.allocationCallbacks, &texture->image);
//		ASSERT_VK_OK(res);
//		CHECK_VK_FAIL(res)
//
//			//m_Context.nameVKObject(texture->image, vk::DebugReportObjectTypeEXT::eImage, desc.debugName.c_str());
//
//		if (!desc.isVirtual)
//		{
//			res = m_Allocator.allocateTextureMemory(texture);
//			ASSERT_VK_OK(res);
//			CHECK_VK_FAIL(res)
//
//				if ((desc.sharedResourceFlags & SharedResourceFlags::Shared) != 0)
//				{
//#ifdef _WIN32
//					texture->sharedHandle = m_Context.device.getMemoryWin32HandleKHR({ texture->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 });
//#else
//					texture->sharedHandle = (void*)(size_t)m_Context.device.getMemoryFdKHR({ texture->memory, vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd });
//#endif
//				}
//
//			m_Context.nameVKObject(texture->memory, vk::DebugReportObjectTypeEXT::eDeviceMemory, desc.debugName.c_str());
//		}
//
//		return TextureHandle::Create(texture);
		return TextureHandle();
	}

	BufferHandle Device::createBuffer(const BufferDesc& d)
	{
		return BufferHandle();
	}

	FramebufferHandle Device::createFramebuffer(const FramebufferDesc& desc)
	{
		Framebuffer* fb = new Framebuffer(m_Context);
		fb->desc = desc;
		fb->framebufferInfo = FramebufferInfoEx(desc);

		attachment_vector<VkAttachmentDescription2> attachmentDescs(desc.colorAttachments.size());
		attachment_vector<VkAttachmentReference2> colorAttachmentRefs(desc.colorAttachments.size());
		VkAttachmentReference2 depthAttachmentRef;

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

			attachmentDescs[i].format = attachmentFormat;
			attachmentDescs[i].samples = t->imageInfo.samples;
			attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			//.setFormat(attachmentFormat)
			//.setSamples(t->imageInfo.samples)
			//.setLoadOp(VkAttachmentLoadOp::eLoad)
			//.setStoreOp(VkAttachmentStoreOp::eStore)
			//.setInitialLayout(VkImageLayout::eColorAttachmentOptimal)
			//.setFinalLayout(VkImageLayout::eColorAttachmentOptimal);

			colorAttachmentRefs[i] = VkAttachmentReference2{};
			colorAttachmentRefs[i].attachment = i;
			colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			/*.setAttachment(i)
			.setLayout(VkImageLayout::eColorAttachmentOptimal);*/

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

			/*	attachmentDescs.push_back(vk::AttachmentDescription2()
					.setFormat(texture->imageInfo.format)
					.setSamples(texture->imageInfo.samples)
					.setLoadOp(vk::AttachmentLoadOp::eLoad)
					.setStoreOp(vk::AttachmentStoreOp::eStore)
					.setInitialLayout(depthLayout)
					.setFinalLayout(depthLayout));*/

			VkAttachmentDescription2 depthAttachment;
			depthAttachment.format = texture->imageInfo.format;
			depthAttachment.samples = texture->imageInfo.samples;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.initialLayout = depthLayout;
			depthAttachment.finalLayout = depthLayout;

			attachmentDescs.push_back(depthAttachment);


			depthAttachmentRef = VkAttachmentReference2{};
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

		VkSubpassDescription2 subpass;
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
		VkAttachmentReference2 vrsAttachmentRef;
		VkFragmentShadingRateAttachmentInfoKHR shadingRateAttachmentInfo;

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


			VkAttachmentDescription2 vrsAttachmentDesc;
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

			VkAttachmentReference2 vrsAttachmentRef;
			/*		.setAttachment(uint32_t(attachmentDescs.size()) - 1)
					.setLayout(vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR);*/

			vrsAttachmentRef.attachment = uint32_t(attachmentDescs.size()) - 1;
			vrsAttachmentRef.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;

			VkFragmentShadingRateAttachmentInfoKHR shadingRateAttachmentInfo;
			shadingRateAttachmentInfo.pFragmentShadingRateAttachment = &vrsAttachmentRef;
			shadingRateAttachmentInfo.shadingRateAttachmentTexelSize = rateProps.minFragmentShadingRateAttachmentTexelSize;

			/*.setPFragmentShadingRateAttachment(&vrsAttachmentRef)
			.setShadingRateAttachmentTexelSize(rateProps.minFragmentShadingRateAttachmentTexelSize);*/
			subpass.pNext = &shadingRateAttachmentInfo;
		}

		VkRenderPassCreateInfo2&& renderPassInfo = {};
		//.setAttachmentCount(uint32_t(attachmentDescs.size()))
		//.setPAttachments(attachmentDescs.data())
		//.setSubpassCount(1)
		//.setPSubpasses(&subpass);

		renderPassInfo.attachmentCount = uint32_t(attachmentDescs.size());
		renderPassInfo.pAttachments = attachmentDescs.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;


		if (vkCreateRenderPass2(m_Context.device, &renderPassInfo, m_Context.allocationCallbacks,
			&fb->renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		//vk::Result res = m_Context.device.createRenderPass2(&renderPassInfo,
		//	m_Context.allocationCallbacks,
		//	&fb->renderPass);
		//CHECK_VK_FAIL(res)

			// set up the framebuffer object
			//auto framebufferInfo = vk::FramebufferCreateInfo()
			//.setRenderPass(fb->renderPass)
			//.setAttachmentCount(uint32_t(attachmentViews.size()))
			//.setPAttachments(attachmentViews.data())
			//.setWidth(fb->framebufferInfo.width)
			//.setHeight(fb->framebufferInfo.height)
			//.setLayers(numArraySlices);

		VkFramebufferCreateInfo framebufferInfo;
		framebufferInfo.renderPass = fb->renderPass;
		framebufferInfo.attachmentCount = uint32_t(attachmentViews.size());
		framebufferInfo.pAttachments = attachmentViews.data();
		framebufferInfo.width = fb->framebufferInfo.width;
		framebufferInfo.height = fb->framebufferInfo.height;
		framebufferInfo.layers = numArraySlices;


		/*	res = m_Context.device.createFramebuffer(&framebufferInfo, m_Context.allocationCallbacks,
				&fb->framebuffer);
			CHECK_VK_FAIL(res)*/
		if (vkCreateFramebuffer(m_Context.device, &framebufferInfo, m_Context.allocationCallbacks, &fb->framebuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}


		return FramebufferHandle::Create(fb);
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


		VkPipelineViewportStateCreateInfo viewportState;
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;


		//auto rasterizer = vk::PipelineRasterizationStateCreateInfo()
		//	// .setDepthClampEnable(??)
		//	// .setRasterizerDiscardEnable(??)
		//	.setPolygonMode(convertFillMode(rasterState.fillMode))
		//	.setCullMode(convertCullMode(rasterState.cullMode))
		//	.setFrontFace(rasterState.frontCounterClockwise ?
		//		vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise)
		//	.setDepthBiasEnable(rasterState.depthBias ? true : false)
		//	.setDepthBiasConstantFactor(float(rasterState.depthBias))
		//	.setDepthBiasClamp(rasterState.depthBiasClamp)
		//	.setDepthBiasSlopeFactor(rasterState.slopeScaledDepthBias)
		//	.setLineWidth(1.0f);

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
		conservativeRasterState.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;

		/*auto conservativeRasterState = vk::PipelineRasterizationConservativeStateCreateInfoEXT()
			.setConservativeRasterizationMode(vk::ConservativeRasterizationModeEXT::eOverestimate);*/
		if (rasterState.conservativeRasterEnable)
		{
			rasterizer.pNext = &conservativeRasterState;
		}

		VkPipelineMultisampleStateCreateInfo multisample{};
		multisample.rasterizationSamples = VkSampleCountFlagBits(fb->framebufferInfo.sampleCount);
		multisample.alphaToCoverageEnable = blendState.alphaToCoverageEnable;
		/*.setRasterizationSamples(vk::SampleCountFlagBits(fb->framebufferInfo.sampleCount))
		.setAlphaToCoverageEnable(blendState.alphaToCoverageEnable);*/

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		//.setDepthTestEnable(depthStencilState.depthTestEnable)
		//.setDepthWriteEnable(depthStencilState.depthWriteEnable)
		//.setDepthCompareOp(convertCompareOp(depthStencilState.depthFunc))
		//.setStencilTestEnable(depthStencilState.stencilEnable)
		//.setFront(convertStencilState(depthStencilState, depthStencilState.frontFaceStencil))
		//.setBack(convertStencilState(depthStencilState, depthStencilState.backFaceStencil));
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
		VkPipelineFragmentShadingRateStateCreateInfoKHR shadingRateState;
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
		/*	auto pushConstantRange = vk::PushConstantRange()
				.setOffset(0)
				.setSize(pushConstantSize)
				.setStageFlags(convertShaderTypeToShaderStageFlagBits(pso->shaderMask));*/

		pushConstantRange.offset = 0;
		pushConstantRange.size = pushConstantSize;
		pushConstantRange.stageFlags = VkUtil::convertShaderTypeToShaderStageFlagBits(pso->shaderMask);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo;
		pipelineLayoutInfo.setLayoutCount = uint32_t(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = pushConstantSize ? 1 : 0;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		/*auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(uint32_t(descriptorSetLayouts.size()))
			.setPSetLayouts(descriptorSetLayouts.data())
			.setPushConstantRangeCount(pushConstantSize ? 1 : 0)
			.setPPushConstantRanges(&pushConstantRange);*/



			//res = m_Context.device.createPipelineLayout(&pipelineLayoutInfo,
			//	m_Context.allocationCallbacks,
			//	&pso->pipelineLayout);
			//CHECK_VK_FAIL(res)

		if (vkCreatePipelineLayout(m_Context.device, &pipelineLayoutInfo,
			m_Context.allocationCallbacks,
			&pso->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
		/*	vkCreatePipelineLayout(m_Context.device, &pipelineLayoutInfo,
				m_Context.allocationCallbacks,
				&pso->pipelineLayout)*/

		attachment_vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(fb->desc.colorAttachments.size());

		for (uint32_t i = 0; i < uint32_t(fb->desc.colorAttachments.size()); i++)
		{
			colorBlendAttachments[i] = VkUtil::convertBlendState(blendState.targets[i]);
		}

		VkPipelineColorBlendStateCreateInfo colorBlend;

		colorBlend.attachmentCount = uint32_t(colorBlendAttachments.size());
		colorBlend.pAttachments = colorBlendAttachments.data();

		/*.setAttachmentCount(uint32_t(colorBlendAttachments.size()))
		.setPAttachments(colorBlendAttachments.data());*/

		pso->usesBlendConstants = blendState.usesConstantColor(uint32_t(fb->desc.colorAttachments.size()));

		VkDynamicState dynamicStates[4] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_BLEND_CONSTANTS,
			VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};

		dynamicStateInfo.dynamicStateCount = pso->usesBlendConstants ? 3 : 2;
		dynamicStateInfo.pDynamicStates = dynamicStates;


		//auto dynamicStateInfo = VkPipelineDynamicStateCreateInfo()
		//	.setDynamicStateCount(pso->usesBlendConstants ? 3 : 2)
		//	.setPDynamicStates(dynamicStates);
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

		//auto pipelineInfo = VkGraphicsPipelineCreateInfo()
		//	.setStageCount(uint32_t(shaderStages.size()))
		//	.setPStages(shaderStages.data())
		//	.setPVertexInputState(&vertexInput)
		//	.setPInputAssemblyState(&inputAssembly)
		//	.setPViewportState(&viewportState)
		//	.setPRasterizationState(&rasterizer)
		//	.setPMultisampleState(&multisample)
		//	.setPDepthStencilState(&depthStencil)
		//	.setPColorBlendState(&colorBlend)
		//	.setPDynamicState(&dynamicStateInfo)
		//	.setLayout(pso->pipelineLayout)
		//	.setRenderPass(fb->renderPass)
		//	.setSubpass(0)
		//	.setBasePipelineHandle(nullptr)
		//	.setBasePipelineIndex(-1)
		//	.setPTessellationState(nullptr)
		//	.setPNext(&shadingRateState);

		VkPipelineTessellationStateCreateInfo tessellationState{};

		if (desc.primType == PrimitiveType::PatchList)
		{
			tessellationState.patchControlPoints = desc.patchControlPoints;
			pipelineInfo.pTessellationState = &tessellationState;
		}
		if (vkCreateGraphicsPipelines(m_Context.device, m_Context.pipelineCache,
			1, &pipelineInfo,
			m_Context.allocationCallbacks,
			&pso->pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}
		/*vkCreateGraphicsPipelines(m_Context.device, m_Context.pipelineCache,
			1, &pipelineInfo,
			m_Context.allocationCallbacks,
			&pso->pipeline)*/


			//res = m_Context.device.createGraphicsPipelines(m_Context.pipelineCache,
			//	1, &pipelineInfo,
			//	m_Context.allocationCallbacks,
			//	&pso->pipeline);
			//ASSERT_VK_OK(res); // for debugging
			//CHECK_VK_FAIL(res);

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

		VkSamplerCreateInfo samplerInfo;
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

		VkSamplerReductionModeCreateInfoEXT samplerReductionCreateInfo;
		if (desc.reductionType == SamplerReductionType::Minimum || desc.reductionType == SamplerReductionType::Maximum)
		{
			VkSamplerReductionModeEXT reductionMode =
				desc.reductionType == SamplerReductionType::Maximum ? VK_SAMPLER_REDUCTION_MODE_MAX : VK_SAMPLER_REDUCTION_MODE_MIN;
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

		VkShaderModuleCreateInfo shaderInfo;
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

	FramebufferHandle Device::createHandleForNativeFramebuffer(VkRenderPass renderPass, VkFramebuffer framebuffer, const FramebufferDesc& desc, bool transferOwnership)
	{
		return FramebufferHandle();
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

	BindingSetHandle Device::createBindingSet(const BindingSetDesc& desc, IBindingLayout* _layout)
	{
		BindingLayout* layout = dynamic_cast<BindingLayout*>(_layout);

		BindingSet* ret = new BindingSet(m_Context);
		ret->desc = desc;
		ret->layout = layout;

		const auto& descriptorSetLayout = layout->descriptorSetLayout;
		const auto& poolSizes = layout->descriptorPoolSizeInfo;

		VkDescriptorPoolCreateInfo poolInfo;
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
		nvrhi::static_vector<VkDescriptorImageInfo, c_MaxBindingsPerLayout> descriptorImageInfo;
		nvrhi::static_vector<VkDescriptorBufferInfo, c_MaxBindingsPerLayout> descriptorBufferInfo;
		nvrhi::static_vector<VkWriteDescriptorSet, c_MaxBindingsPerLayout> descriptorWriteInfo;
		nvrhi::static_vector<VkWriteDescriptorSetAccelerationStructureKHR, c_MaxBindingsPerLayout> accelStructWriteInfo;

		auto generateWriteDescriptorData =
			// generates a vk::WriteDescriptorSet struct in descriptorWriteInfo
			[&](uint32_t bindingLocation,
				VkDescriptorType descriptorType,
				VkDescriptorImageInfo* imageInfo,
				VkDescriptorBufferInfo* bufferInfo,
				VkBufferView* bufferView,
				const void* pNext = nullptr)
			{

				VkWriteDescriptorSet&& descriptorWrites = {};
				descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites.descriptorType = descriptorType;
				descriptorWrites.dstSet = ret->descriptorSet;
				descriptorWrites.dstBinding = bindingLocation;
				descriptorWrites.dstArrayElement = 0;
				descriptorWrites.descriptorCount = 1;
				descriptorWrites.pImageInfo = imageInfo;
				descriptorWrites.pBufferInfo = bufferInfo;
				descriptorWrites.pTexelBufferView = bufferView;
				descriptorWrites.pNext = pNext;


				descriptorWriteInfo.push_back(std::move(descriptorWrites));

			};

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

				VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();

				//VkDescriptorImageInfo imageInfo;
				imageInfo.imageView = view.view;
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfo, nullptr, nullptr);

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

				VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();

				//VkDescriptorImageInfo imageInfo{};
				imageInfo.imageView = view.view;
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

				//imageInfo = vk::DescriptorImageInfo()
				//	.setImageView(view.view)
				//	.setImageLayout(vk::ImageLayout::eGeneral);

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfo, nullptr, nullptr);

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


					/*			auto bufferViewInfo = vk::BufferViewCreateInfo()
									.setBuffer(buffer->buffer)
									.setOffset(range.byteOffset)
									.setRange(range.byteSize)
									.setFormat(vkformat);*/

					VkBufferViewCreateInfo bufferViewInfo;
					bufferViewInfo.buffer = buffer->buffer;
					bufferViewInfo.offset = range.byteOffset;
					bufferViewInfo.range = range.byteSize;
					bufferViewInfo.format = vkformat;

					vkCreateBufferView(m_Context.device, &bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
					if (vkCreateBufferView(m_Context.device, &bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef) != VK_SUCCESS) {
						throw std::runtime_error("failed to create buffer view!");
					}
					/*		res = m_Context.device.createBufferView(&bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
							ASSERT_VK_OK(res);*/
				}

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					nullptr, nullptr, &bufferViewRef);

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
				const auto buffer = dynamic_cast<Buffer*>(binding.resourceHandle);

				if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
					assert(buffer->desc.canHaveUAVs);
				if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_StructuredBuffer_SRV)
					assert(buffer->desc.structStride != 0);
				if (binding.type == RHIResourceType::RT_RawBuffer_SRV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
					assert(buffer->desc.canHaveRawViews);

				const auto range = binding.range.resolve(buffer->desc);

				auto& bufferInfo = descriptorBufferInfo.emplace_back();
				//VkDescriptorBufferInfo bufferInfo;
				bufferInfo.buffer = buffer->buffer;
				bufferInfo.offset = range.byteOffset;
				bufferInfo.range = range.byteSize;

				/*           bufferInfo = vk::DescriptorBufferInfo()
							   .setBuffer(buffer->buffer)
							   .setOffset(range.byteOffset)
							   .setRange(range.byteSize);*/

				assert(buffer->buffer);
				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					nullptr, &bufferInfo, nullptr);

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

				auto& imageInfo = descriptorImageInfo.emplace_back();

				imageInfo.sampler = sampler->sampler;
				//imageInfo = vk::DescriptorImageInfo()
				//	.setSampler(sampler->sampler);

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfo, nullptr, nullptr);
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
		vkUpdateDescriptorSets(m_Context.device, uint32_t(descriptorWriteInfo.size()), descriptorWriteInfo.data(), 0, nullptr);


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

}
#endif
