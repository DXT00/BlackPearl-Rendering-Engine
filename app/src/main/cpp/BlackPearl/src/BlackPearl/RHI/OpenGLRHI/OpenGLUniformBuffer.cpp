#include "pch.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "OpenGLUniformBuffer.h"
#include "OpenGLDriver/OpenGLDrvPrivate.h"

namespace BlackPearl {

	//FUniformBufferRHIRef FOpenGLDynamicRHI::RHICreateUniformBuffer(const void* Contents, const UniformBufferLayout* Layout, EUniformBufferUsage Usage, EUniformBufferValidation Validation)
	//{
	//	
	//}
	OpenGLUniformBuffer::OpenGLUniformBuffer(const UniformBufferLayout* InLayout, const BufferDesc& _desc)
		:Buffer(_desc)
	{
		glGenBuffers(1, &rendererID);

		AllocatedSize = _desc.byteSize;
		//check(IsInRenderingThread());
		//FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();

		// This should really be synchronized, if there's a chance it'll be used from more than one buffer. Luckily, uniform buffers
		// are only used for drawing/shader usage, not for loading resources or framebuffer blitting, so no synchronization primitives for now.

		// Explicitly check that the size is nonzero before allowing CreateBuffer to opaquely fail.
		//assert(InLayout->Resources.Num() > 0 || Layout->ConstantBufferSize > 0);

		//if (Contents && Validation == EUniformBufferValidation::ValidateResources)
		//{
		//	ValidateShaderParameterResourcesRHI(Contents, *Layout);
		//}

		//bool bUseEmulatedUBs = GUseEmulatedUniformBuffers && !Layout->bNoEmulatedUniformBuffer;

		//bool bStreamDraw = //true(Usage == UniformBuffer_SingleDraw || Usage == UniformBuffer_SingleFrame);
		//	GLuint AllocatedResource = 0;
		//uint32_t OffsetInBuffer = 0;
		//uint8* PersistentlyMappedBuffer = NULL;
		//uint32_t AllocatedSize = 0;
		//FOpenGLEUniformBufferDataRef EmulatedUniformDataRef;

		//// If the uniform buffer contains constants, allocate a uniform buffer resource from GL.
		//if (Layout->ConstantBufferSize > 0)
		//{
		//	uint32_t SizeOfBufferToAllocate = 0;
		//	if (IsPoolingEnabled())
		//	{
		//		// Find the appropriate bucket based on size
		//		const uint32_t BucketIndex = GetPoolBucketIndex(Layout->ConstantBufferSize);
		//		int StreamedIndex = bStreamDraw ? 1 : 0;

		//		FPooledGLUniformBuffer FreeBufferEntry;
		//		FreeBufferEntry.Buffer = 0;
		//		FreeBufferEntry.CreatedSize = 0;
		//		bool bHasEntry = false;
		//		{
		//			FScopeLock Lock(&GGLUniformBufferPoolCS);

		//			TArray<FPooledGLUniformBuffer>* PoolBucket;

		//			if (bUseEmulatedUBs)
		//			{
		//				PoolBucket = &GLEmulatedUniformBufferPool[BucketIndex][StreamedIndex];
		//			}
		//			else
		//			{
		//				PoolBucket = &GLUniformBufferPool[BucketIndex][StreamedIndex];
		//			}

		//			if (PoolBucket->Num() > 0)
		//			{
		//				// Reuse the last entry in this size bucket
		//				FreeBufferEntry = PoolBucket->Pop();
		//				bHasEntry = true;
		//			}
		//		}
		//		if (bHasEntry)
		//		{
		//			DEC_DWORD_STAT(STAT_OpenGLNumFreeUniformBuffers);
		//			DEC_MEMORY_STAT_BY(STAT_OpenGLFreeUniformBufferMemory, FreeBufferEntry.CreatedSize);

		//			AllocatedResource = FreeBufferEntry.Buffer;
		//			AllocatedSize = FreeBufferEntry.CreatedSize;

		//			if (bUseEmulatedUBs)
		//			{
		//				EmulatedUniformDataRef = UniformBufferDataFactory.Get(AllocatedResource);
		//			}
		//			else
		//			{
		//				RHICmdList.EnqueueLambda([AllocatedResource](FRHICommandListImmediate&)
		//					{
		//						VERIFY_GL_SCOPE();
		//						::CachedBindUniformBuffer(AllocatedResource);
		//					});
		//			}
		//		}
		//		else
		//		{
		//			SizeOfBufferToAllocate = UniformBufferSizeBuckets[BucketIndex];
		//		}
		//	}
		//}

		//if (AllocatedSize == 0)
		//{
		//	return CreateUniformBuffer(Contents, Layout, Usage, Validation);
		//}

		//OpenGLUniformBuffer* NewUniformBuffer = new OpenGLUniformBuffer(Layout);
		//NewUniformBuffer->SetGLUniformBufferParams(AllocatedResource, OffsetInBuffer, PersistentlyMappedBuffer, AllocatedSize, EmulatedUniformDataRef, bStreamDraw);

		//check(!bUseEmulatedUBs || (IsValidRef(EmulatedUniformDataRef) && (EmulatedUniformDataRef->Data.Num() * EmulatedUniformDataRef->Data.GetTypeSize() == AllocatedSize)));
		//if (Contents)
		//{
		//	CopyDataToUniformBuffer(RHICmdList, NewUniformBuffer, Contents, Layout->ConstantBufferSize);
		//}

		//// Initialize the resource table for this uniform buffer.
		//NewUniformBuffer->SetLayoutTable(Contents, Validation);

		//return NewUniformBuffer;
	}
	void OpenGLUniformBuffer::SetGLUniformBufferParams(GLuint InResource, uint32_t InOffset, uint8_t* InPersistentlyMappedBuffer, uint32_t InAllocatedSize, FOpenGLEUniformBufferData* InEmulatedBuffer, bool bInStreamDraw)
	{
	}
	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
	}

	//replace by writeBUffer
	//void UpdateUniformBufferContents(OpenGLUniformBuffer* UniformBuffer, const void* Contents, uint32_t ConstantBufferSize)
	//{
	//	if (ConstantBufferSize > 0)
	//	{
	//		/*FOpenGLEUniformBufferDataRef EmulatedUniformDataRef = UniformBuffer->EmulatedBufferData;
	//		uint8_t* PersistentlyMappedBuffer = UniformBuffer->PersistentlyMappedBuffer;

	//		if (IsValidRef(EmulatedUniformDataRef))
	//		{
	//			FMemory::Memcpy(EmulatedUniformDataRef->Data.GetData(), Contents, ConstantBufferSize);
	//		}
	//		else if (PersistentlyMappedBuffer)
	//		{
	//			UE_LOG(LogRHI, Fatal, TEXT("RHIUpdateUniformBuffer doesn't support PersistentlyMappedBuffer yet!"));
	//		}
	//		else*/
	//		{
	//			//::CachedBindUniformBuffer(UniformBuffer->Resource);
	//			FOpenGL::BufferSubData(GL_UNIFORM_BUFFER, 0, ConstantBufferSize, Contents);
	//		}
	//	}
	//}

	//void FOpenGLDynamicRHI::RHIUpdateUniformBuffer(FRHICommandListBase& RHICmdList, FRHIUniformBuffer* UniformBufferRHI, const void* Contents)
	//{
	//	OpenGLUniformBuffer* UniformBuffer = ResourceCast(UniformBufferRHI);

	//	const FRHIUniformBufferLayout& Layout = UniformBufferRHI->GetLayout();
	//	ValidateShaderParameterResourcesRHI(Contents, Layout);

	//	const int32_t ConstantBufferSize = Layout.ConstantBufferSize;
	//	const int32_t NumResources = Layout.Resources.Num();

	//	check(UniformBuffer->GetResourceTable().Num() == NumResources);

	//	uint32_t NextUniqueID = UniqueUniformBufferID();

	//	if (RHICmdList.Bypass())
	//	{
	//		UpdateUniformBufferContents(UniformBuffer, Contents, ConstantBufferSize);

	//		for (int32_t Index = 0; Index < NumResources; ++Index)
	//		{
	//			UniformBuffer->GetResourceTable()[Index] = GetShaderParameterResourceRHI(Contents, Layout.Resources[Index].MemberOffset, Layout.Resources[Index].MemberType);
	//		}

	//		UniformBuffer->UniqueID = NextUniqueID;
	//	}
	//	//else
	//	//{
	//	//	FRHIResource** CmdListResources = nullptr;
	//	//	void* CmdListConstantBufferData = nullptr;

	//	//	if (NumResources > 0)
	//	//	{
	//	//		CmdListResources = (FRHIResource**)RHICmdList.Alloc(sizeof(FRHIResource*) * NumResources, alignof(FRHIResource*));

	//	//		for (int32_t Index = 0; Index < NumResources; ++Index)
	//	//		{
	//	//			const auto Parameter = Layout.Resources[Index];
	//	//			CmdListResources[Index] = GetShaderParameterResourceRHI(Contents, Parameter.MemberOffset, Parameter.MemberType);
	//	//		}
	//	//	}

	//	//	if (ConstantBufferSize > 0)
	//	//	{
	//	//		CmdListConstantBufferData = (void*)RHICmdList.Alloc(ConstantBufferSize, 16);
	//	//		FMemory::Memcpy(CmdListConstantBufferData, Contents, ConstantBufferSize);
	//	//	}

	//	//	RHICmdList.EnqueueLambda([UniformBuffer, CmdListResources, NumResources, CmdListConstantBufferData, ConstantBufferSize, NextUniqueID](FRHICommandListBase&)
	//	//		{
	//	//			UpdateUniformBufferContents(UniformBuffer, CmdListConstantBufferData, ConstantBufferSize);

	//	//			// Update resource table.
	//	//			for (int32_t ResourceIndex = 0; ResourceIndex < NumResources; ++ResourceIndex)
	//	//			{
	//	//				UniformBuffer->GetResourceTable()[ResourceIndex] = CmdListResources[ResourceIndex];
	//	//			}
	//	//			UniformBuffer->UniqueID = NextUniqueID;
	//	//		});
	//	//	RHICmdList.RHIThreadFence(true);
	//	//}
	//}



}