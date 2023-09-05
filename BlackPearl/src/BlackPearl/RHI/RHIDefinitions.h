#pragma once
namespace BlackPearl {

/**
 *	Resource usage flags - for vertex and index buffers.
 */
	enum class EBufferUsageFlags: int
	{
		None = 0,

		/** The buffer will be written to once. */
		Static = 1 << 0,

		/** The buffer will be written to occasionally, GPU read only, CPU write only.  The data lifetime is until the next update, or the buffer is destroyed. */
		Dynamic = 1 << 1,

		/** The buffer's data will have a lifetime of one frame.  It MUST be written to each frame, or a new one created each frame. */
		Volatile = 1 << 2,

		/** Allows an unordered access view to be created for the buffer. */
		UnorderedAccess = 1 << 3,

		/** Create a byte address buffer, which is basically a structured buffer with a uint32 type. */
		ByteAddressBuffer = 1 << 4,

		/** Buffer that the GPU will use as a source for a copy. */
		SourceCopy = 1 << 5,

		/** Create a buffer that can be bound as a stream output target. */
		StreamOutput = 1 << 6,

		/** Create a buffer which contains the arguments used by DispatchIndirect or DrawIndirect. */
		DrawIndirect = 1 << 7,

		/**
		 * Create a buffer that can be bound as a shader resource.
		 * This is only needed for buffer types which wouldn't ordinarily be used as a shader resource, like a vertex buffer.
		 */
		 ShaderResource = 1 << 8,

		 /** Request that this buffer is directly CPU accessible. */
		 KeepCPUAccessible = 1 << 9,

		 /** Buffer should go in fast vram (hint only). Requires BUF_Transient */
		 FastVRAM = 1 << 10,

		 /** Buffer should be allocated from transient memory. */
		 Transient = None,

		 /** Create a buffer that can be shared with an external RHI or process. */
		 Shared = 1 << 12,

		 /**
		  * Buffer contains opaque ray tracing acceleration structure data.
		  * Resources with this flag can't be bound directly to any shader stage and only can be used with ray tracing APIs.
		  * This flag is mutually exclusive with all other buffer flags except BUF_Static.
		 */
		 AccelerationStructure = 1 << 13,

		 VertexBuffer = 1 << 14,
		 IndexBuffer = 1 << 15,
		 StructuredBuffer = 1 << 16,

		 /** Buffer memory is allocated independently for multiple GPUs, rather than shared via driver aliasing */
		 MultiGPUAllocate = 1 << 17,

		 /**
		  * Tells the render graph to not bother transferring across GPUs in multi-GPU scenarios.  Useful for cases where
		  * a buffer is read back to the CPU (such as streaming request buffers), or written to each frame by CPU (such
		  * as indirect arg buffers), and the other GPU doesn't actually care about the data.
		 */
		 MultiGPUGraphIgnore = 1 << 18,

		 /** Allows buffer to be used as a scratch buffer for building ray tracing acceleration structure,
		  * which implies unordered access. Only changes the buffer alignment and can be combined with other flags.
		 **/
		 RayTracingScratch = (1 << 19) | UnorderedAccess,

		 // Helper bit-masks
		 AnyDynamic = (Dynamic | Volatile),
	};
}
