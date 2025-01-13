#pragma once
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/RHIState.h"

namespace BlackPearl {
	class OpenGLBuffer: public RefCounter<IBuffer>, public BufferStateExtension
	{
	public:
		OpenGLBuffer(BufferDesc _desc)
			: BufferStateExtension(_desc) {
			desc = _desc;
		}
		BufferDesc desc;

		const BufferDesc& getDesc() const override { return desc; }

	};


	class OpenGLRenderBuffer : public OpenGLBuffer
	{
	public:
		OpenGLRenderBuffer(BufferDesc _desc);
		unsigned int rbo;
		unsigned int width;
		unsigned int height;

	};


	class OpenGLUniformBuffer : public OpenGLBuffer
	{
	public:
		/** The GL resource for this uniform buffer. */
		GLuint Resource;

		/** The offset of the uniform buffer's contents in the resource. */
		uint32_t Offset;

		/** When using a persistently mapped buffer this is a pointer to the CPU accessible data. */
		uint8_t* PersistentlyMappedBuffer;

		/** Unique ID for state shadowing purposes. */
		uint32_t UniqueID;

		/** Emulated uniform data for ES2. */
		FOpenGLEUniformBufferDataRef EmulatedBufferData;

		/** The size of the buffer allocated to hold the uniform buffer contents. May be larger than necessary. */
		uint32_t AllocatedSize;

		/** True if the uniform buffer is not used across frames. */
		bool bStreamDraw;

		/** True if the uniform buffer is emulated */
		bool bIsEmulatedUniformBuffer;

		/** Initialization constructor. */
		OpenGLUniformBuffer(const FRHIUniformBufferLayout* InLayout);

		void SetGLUniformBufferParams(GLuint InResource, uint32_t InOffset, uint8_t* InPersistentlyMappedBuffer, uint32_t InAllocatedSize, FOpenGLEUniformBufferDataRef InEmulatedBuffer, bool bInStreamDraw);

		/** Destructor. */
		~OpenGLUniformBuffer();

		// Provides public non-const access to ResourceTable.
		// @todo refactor uniform buffers to perform updates as a member function, so this isn't necessary.
		TArray<TRefCountPtr<FRHIResource>>& GetResourceTable() { return ResourceTable; }

		void SetLayoutTable(const void* Contents, EUniformBufferValidation Validation);
	};



}