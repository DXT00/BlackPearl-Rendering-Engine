#pragma once
#include "RHIResources.h"
#include "RHIDefinitions.h"
namespace BlackPearl {

	class RHIBuffer : public RHIResource
#if ENABLE_RHI_VALIDATION
		, public RHIValidation::FBufferResource
#endif
	{
	public:
		RHIBuffer() : RHIResource(RRT_Buffer) {}

		/** Initialization constructor. */
		RHIBuffer(uint32_t InSize, EBufferUsageFlags InUsage, uint32_t InStride)
			: RHIResource(RRT_Buffer)
			, Size(InSize)
			, Stride(InStride)
			, Usage(InUsage)
		{
		}
		/** @return The number of bytes in the buffer. */
		uint32_t GetSize() const { return Size; }

		/** @return The stride in bytes of the buffer. */
		uint32_t GetStride() const { return Stride; }

		/** @return The usage flags used to create the buffer. */
		EBufferUsageFlags GetUsage() const { return Usage; }

		void SetName(const std::string& InName) { BufferName = InName; }

		std::string GetName() const { return BufferName; }

		virtual uint32_t GetParentGPUIndex() const { return 0; }

	protected:
		//void Swap(FRHIBuffer& Other)
		//{
		//	::Swap(Stride, Other.Stride);
		//	::Swap(Size, Other.Size);
		//	::Swap(Usage, Other.Usage);
		//}

		// Used by RHI implementations that may adjust internal usage flags during object construction.
		void SetUsage(EBufferUsageFlags InUsage)
		{
			Usage = InUsage;
		}

		void ReleaseUnderlyingResource()
		{
			Stride = Size = 0;
			Usage = EBufferUsageFlags::None;
		}

	private:
		uint32_t Size{};
		uint32_t Stride{};
		EBufferUsageFlags Usage{};
		std::string BufferName;
	};

}


