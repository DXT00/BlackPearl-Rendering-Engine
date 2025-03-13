#pragma once
#include "OpenGLBuffer.h"
#include "BlackPearl/Math/vector.h"
namespace BlackPearl {
	using namespace math;

	enum EUniformBufferUsage
	{
		// the uniform buffer is temporary, used for a single draw call then discarded
		UniformBuffer_SingleDraw = 0,
		// the uniform buffer is used for multiple draw calls but only for the current frame
		UniformBuffer_SingleFrame,
		// the uniform buffer is used for multiple draw calls, possibly across multiple frames
		UniformBuffer_MultiFrame,
	};



	struct FRHIUniformBufferResource
	{
		/** Byte offset to each resource in the uniform buffer memory. */
		uint16_t MemberOffset;

		/** Type of the member that allow (). */
		EUniformBufferBaseType MemberType;

		/** Compare two uniform buffer layout resources. */
		friend inline bool operator==(const FRHIUniformBufferResource& A, const FRHIUniformBufferResource& B)
		{
			return A.MemberOffset == B.MemberOffset
				&& A.MemberType == B.MemberType;
		}
	};



	struct FOpenGLEUniformBufferData //: public FRefCountedObject
	{
		FOpenGLEUniformBufferData(uint32_t SizeInBytes)
		{
			uint32_t SizeInUint32s = (SizeInBytes + 3) / 4;
			Data.resize(SizeInUint32s);
			/*Data.Empty(SizeInUint32s);
			Data.AddUninitialized(SizeInUint32s);*/
		}

		~FOpenGLEUniformBufferData()
		{
		}

		std::vector<uint32_t> Data;
	};


	class OpenGLUniformBuffer : public Buffer
	{
	public:
		OpenGLUniformBuffer(const UniformBufferLayout* InLayout, const BufferDesc& _desc);


		/** The offset of the uniform buffer's contents in the resource. */
		uint32_t Offset;

		/** When using a persistently mapped buffer this is a pointer to the CPU accessible data. */
		uint8_t* PersistentlyMappedBuffer;

		/** Unique ID for state shadowing purposes. */
		uint32_t UniqueID;

		/** Emulated uniform data for ES2.  多个uniformbuffers 合并提交*/
		/*static FAutoConsoleVariable CVarOpenGLUseEmulatedUBs(
		TEXT("OpenGL.UseEmulatedUBs"),
		1,
		TEXT("If true, enable using emulated uniform buffers on OpenGL Mobile mode."),
		ECVF_ReadOnly
		);
		*/
		FOpenGLEUniformBufferData* EmulatedBufferData;

		/** The size of the buffer allocated to hold the uniform buffer contents. May be larger than necessary. */
		uint32_t AllocatedSize;

		/** True if the uniform buffer is not used across frames. */
		bool bStreamDraw;

		/** True if the uniform buffer is emulated */
		bool bIsEmulatedUniformBuffer;

		/** Initialization constructor. */
		//OpenGLUniformBuffer(const UniformBufferLayout* InLayout);

		void SetGLUniformBufferParams(GLuint InResource, uint32_t InOffset, uint8_t* InPersistentlyMappedBuffer, uint32_t InAllocatedSize, FOpenGLEUniformBufferData* InEmulatedBuffer, bool bInStreamDraw);

		/** Destructor. */
		~OpenGLUniformBuffer();

		// Provides public non-const access to ResourceTable.
		// @todo refactor uniform buffers to perform updates as a member function, so this isn't necessary.
		//std::vector<TRefCountPtr<FRHIResource>>& GetResourceTable() { return ResourceTable; }

		//void SetLayoutTable(const void* Contents, EUniformBufferValidation Validation);
	};





}
