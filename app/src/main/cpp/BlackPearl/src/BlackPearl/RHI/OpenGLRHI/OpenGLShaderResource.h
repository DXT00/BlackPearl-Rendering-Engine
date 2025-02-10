#pragma once
namespace BlackPearl{
	enum
	{
		OGL_MAX_UNIFORM_BUFFER_BINDINGS = 12,	// @todo-mobile: Remove me
		OGL_FIRST_UNIFORM_BUFFER = 0,			// @todo-mobile: Remove me
		OGL_UAV_NOT_SUPPORTED_FOR_GRAPHICS_UNIT = -1, // for now, only CS and PS supports UAVs/ images
	};


	/**
	 * Caching of OpenGL uniform parameters.
	 */
	class FOpenGLShaderParameterCache
	{
	public:
		/** Constructor. */
		FOpenGLShaderParameterCache();

		/** Destructor. */
		~FOpenGLShaderParameterCache();

		void InitializeResources(int32_t UniformArraySize);

		/**
		 * Marks all uniform arrays as dirty.
		 */
		void MarkAllDirty();

		/**
		 * Sets values directly into the packed uniform array
		 */
		void Set(uint32_t BufferIndex, uint32_t ByteOffset, uint32_t NumBytes, const void* NewValues);

		/**
		 * Commit shader parameters to the currently bound program.
		 * @param ParameterTable - Information on the bound uniform arrays for the program.
		 */
		/*void CommitPackedGlobals(const FOpenGLLinkedProgram* LinkedProgram, int32_t Stage);

		void CommitPackedUniformBuffers(FOpenGLLinkedProgram* LinkedProgram, int32_t Stage, FRHIUniformBuffer** UniformBuffers, const TArray<CrossCompiler::FUniformBufferCopyInfo>& UniformBuffersCopyInfo);*/

	private:

		/** CPU memory block for storing uniform values. */
		//uint8_t* PackedGlobalUniforms[CrossCompiler::PACKED_TYPEINDEX_MAX];

		struct FRange
		{
			uint32_t	StartVector;
			uint32_t	NumVectors;

			void MarkDirtyRange(uint32_t NewStartVector, uint32_t NewNumVectors);
		};
		///** Dirty ranges for each uniform array. */
		//FRange	PackedGlobalUniformDirty[CrossCompiler::PACKED_TYPEINDEX_MAX];

		///** Scratch CPU memory block for uploading packed uniforms. */
		//uint8_t* PackedUniformsScratch[CrossCompiler::PACKED_TYPEINDEX_MAX];

		/** in bytes */
		int32_t GlobalUniformArraySize;
	};


	
