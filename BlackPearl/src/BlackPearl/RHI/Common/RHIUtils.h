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

#pragma once

#include <mutex>
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/RHIDevice.h"
namespace BlackPearl
{
	class RHIUtils {
	public:
		static BlendState::RenderTarget CreateAddBlendState(
			BlendFactor srcBlend,
			BlendFactor dstBlend);


		static BufferDesc CreateStaticConstantBufferDesc(
			uint32_t byteSize,
			const char* debugName);

		static BufferDesc CreateVolatileConstantBufferDesc(
			uint32_t byteSize,
			const char* debugName,
			uint32_t maxVersions);

		static bool CreateBindingSetAndLayout(
			IDevice* device,
			ShaderType visibility,
			uint32_t registerSpace,
			const BindingSetDesc& bindingSetDesc,
			BindingLayoutHandle& bindingLayout,
			BindingSetHandle& bindingSet);

		static void ClearColorAttachment(
			ICommandList* commandList,
			IFramebuffer* framebuffer,
			uint32_t attachmentIndex,
			Color color
		);

		static void ClearDepthStencilAttachment(
			ICommandList* commandList,
			IFramebuffer* framebuffer,
			float depth,
			uint32_t stencil
		);

	/*	void BuildBottomLevelAccelStruct(
			ICommandList* commandList,
			rt::IAccelStruct* as,
			const rt::AccelStructDesc& desc
		);*/

		// Places a UAV barrier on the provided texture.
		// Useful when doing multiple consecutive dispatch calls with the same resources but different constants.
		// Ignored if there was a call to setEnableUavBarriersForTexrure(..., false) on this texture.
		static void TextureUavBarrier(
			ICommandList* commandList,
			ITexture* texture);

		// Places a UAV barrier on the provided buffer.
		// Useful when doing multiple consecutive dispatch calls with the same resources but different constants.
		// Ignored if there was a call to setEnableUavBarriersForBuffer(..., false) on this buffer.
		static void BufferUavBarrier(
			ICommandList* commandList,
			IBuffer* buffer);

		// Selects a format from the supplied list that supports all the required features on the given device.
		// The formats are tested in the same order they're provided, and the first matching one is returned.
		// If no formats are matching, Format::UNKNOWN is returned.
		static Format ChooseFormat(
			IDevice* device,
			FormatSupport requiredFeatures,
			const Format* requestedFormats,
			size_t requestedFormatCount);

		static const char* GraphicsAPIToString(GraphicsAPI api);
		static const char* TextureDimensionToString(TextureDimension dimension);
		static const char* DebugNameToString(const std::string& debugName);
		static const char* ShaderStageToString(ShaderType stage);
		static const char* ResourceTypeToString(RHIResourceType type);
		static const char* FormatToString(Format format);
		static const char* CommandQueueToString(CommandQueue queue);

		static std::string GenerateHeapDebugName(const HeapDesc& desc);
		static std::string GenerateTextureDebugName(const TextureDesc& desc);
		static std::string GenerateBufferDebugName(const BufferDesc& desc);

		static void NotImplemented();
		static void NotSupported();
		static void InvalidEnum();

		
	};

	class BitSetAllocator
	{
	public:
		explicit BitSetAllocator(size_t capacity, bool multithreaded);

		int allocate();
		void release(int index);
		[[nodiscard]] size_t getCapacity() const { return m_Allocated.size(); }

	private:
		int m_NextAvailable = 0;
		std::vector<bool> m_Allocated;
		bool m_MultiThreaded;
		std::mutex m_Mutex;
	};
}
