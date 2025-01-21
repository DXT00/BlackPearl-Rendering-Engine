#pragma once
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/RHIState.h"

namespace BlackPearl {

	class Buffer: public RefCounter<IBuffer>, public BufferStateExtension
	{
	public:
		Buffer(const BufferDesc& _desc)
			: BufferStateExtension(_desc) {
			desc = _desc;
		}
		BufferDesc desc;

		const BufferDesc& getDesc() const override { return desc; }
		

	};
	class OpenGLBufferFactory {
	public:
		static Buffer* createUniformBuffer(const BufferDesc& _desc);
		static Buffer* createIndexBuffer(const BufferDesc& _desc);
		static Buffer* createVertexBuffer(const BufferDesc& _desc);
		static Buffer* createIndirectBuffer(const BufferDesc& _desc);
		static Buffer* createShaderStorageBuffer(const BufferDesc& _desc);
		static Buffer* createTexelBuffer(const BufferDesc& _desc);
	};


	class OpenGLRenderBuffer : public Buffer
	{
	public:
		OpenGLRenderBuffer(const BufferDesc& _desc);
		unsigned int rbo;
		unsigned int width;
		unsigned int height;

	};

	/** The layout of a uniform buffer in memory. */
	struct FRHIUniformBufferLayout //: public FRHIResource
	{
		FRHIUniformBufferLayout() = default;

		 //explicit FRHIUniformBufferLayout(const FRHIUniformBufferLayoutInitializer& Initializer);

		inline const std::string& GetDebugName() const
		{
			return Name;
		}

		inline uint32_t GetHash() const
		{
			assert(Hash != 0);
			return Hash;
		}

		/*inline bool HasRenderTargets() const
		{
			return RenderTargetsOffset != kUniformBufferInvalidOffset;
		}*/

		inline bool HasExternalOutputs() const
		{
			return bHasNonGraphOutputs;
		}

		//inline bool HasStaticSlot() const
		//{
		//	return IsUniformBufferStaticSlotValid(StaticSlot);
		//}

		const std::string Name;

		///** The list of all resource inlined into the shader parameter structure. */
		//const std::vector<FRHIUniformBufferResource> Resources;

		///** The list of all RDG resource references inlined into the shader parameter structure. */
		//const std::vector<FRHIUniformBufferResource> GraphResources;

		///** The list of all RDG texture references inlined into the shader parameter structure. */
		//const std::vector<FRHIUniformBufferResource> GraphTextures;

		///** The list of all RDG buffer references inlined into the shader parameter structure. */
		//const std::vector<FRHIUniformBufferResource> GraphBuffers;

		///** The list of all RDG uniform buffer references inlined into the shader parameter structure. */
		//const std::vector<FRHIUniformBufferResource> GraphUniformBuffers;

		///** The list of all non-RDG uniform buffer references inlined into the shader parameter structure. */
		//const std::vector<FRHIUniformBufferResource> UniformBuffers;

		const uint32_t Hash;

		/** The size of the constant buffer in bytes. */
		const uint32_t ConstantBufferSize;

		/** The render target binding slots offset, if it exists. */
		const uint16_t RenderTargetsOffset;

		///** The static slot (if applicable). */
		//const FUniformBufferStaticSlot StaticSlot;

		///** The binding flags describing how this resource can be bound to the RHI. */
		//const EUniformBufferBindingFlags BindingFlags;

		/** Whether this layout may contain non-render-graph outputs (e.g. RHI UAVs). */
		const bool bHasNonGraphOutputs;

		/** Used for platforms which use emulated ub's, forces a real uniform buffer instead */
		const bool bNoEmulatedUniformBuffer;

		/** Compare two uniform buffer layouts. */
		/*friend inline bool operator==(const FRHIUniformBufferLayout& A, const FRHIUniformBufferLayout& B)
		{
			return A.ConstantBufferSize == B.ConstantBufferSize
				&& A.StaticSlot == B.StaticSlot
				&& A.BindingFlags == B.BindingFlags
				&& A.Resources == B.Resources;
		}*/
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
		OpenGLUniformBuffer(const FRHIUniformBufferLayout* InLayout, const BufferDesc& _desc);
		/** The GL resource for this uniform buffer. */
		GLuint ubo;

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
		OpenGLUniformBuffer(const FRHIUniformBufferLayout* InLayout);

		void SetGLUniformBufferParams(GLuint InResource, uint32_t InOffset, uint8_t* InPersistentlyMappedBuffer, uint32_t InAllocatedSize, FOpenGLEUniformBufferData* InEmulatedBuffer, bool bInStreamDraw);

		/** Destructor. */
		~OpenGLUniformBuffer();

		// Provides public non-const access to ResourceTable.
		// @todo refactor uniform buffers to perform updates as a member function, so this isn't necessary.
		//std::vector<TRefCountPtr<FRHIResource>>& GetResourceTable() { return ResourceTable; }

		//void SetLayoutTable(const void* Contents, EUniformBufferValidation Validation);
	};



}