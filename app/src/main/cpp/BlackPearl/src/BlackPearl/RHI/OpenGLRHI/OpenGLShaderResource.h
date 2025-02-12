#pragma once
#include "OpenGLDriver/OpenGLDrv.h"
#include "../RHIDefinitions.h"
namespace BlackPearl {

	// unique identifier for a program. (composite of shader keys)
	class FOpenGLProgramKey
	{
	public:
		FOpenGLProgramKey() {}

		/*friend bool operator == (const FOpenGLProgramKey& A, const FOpenGLProgramKey& B)
		{
			bool bHashMatch = true;
			for (uint32_t i = 0; i < ShaderType::NUM_COMPILE_SHADER_STAGES && bHashMatch; ++i)
			{
				bHashMatch = A.ShaderHashes[i] == B.ShaderHashes[i];
			}
			return bHashMatch;
		}

		friend bool operator != (const FOpenGLProgramKey& A, const FOpenGLProgramKey& B)
		{
			return !(A == B);
		}

		friend uint32_t GetTypeHash(const FOpenGLProgramKey& Key)
		{
			return FCrc::MemCrc32(Key.ShaderHashes, sizeof(Key.ShaderHashes));
		}*/

		/*friend FArchive& operator<<(FArchive& Ar, FOpenGLProgramKey& HashSet)
		{
			for (int32 Stage = 0; Stage < CrossCompiler::NUM_SHADER_STAGES; Stage++)
			{
				Ar << HashSet.ShaderHashes[Stage];
			}
			return Ar;
		}*/

	/*	FString ToString() const
		{
			FString retme;
			if (ShaderHashes[CrossCompiler::SHADER_STAGE_VERTEX] != FSHAHash())
			{
				retme = TEXT("Program V_") + ShaderHashes[CrossCompiler::SHADER_STAGE_VERTEX].ToString();
				retme += TEXT("_P_") + ShaderHashes[CrossCompiler::SHADER_STAGE_PIXEL].ToString();
				return retme;
			}
			else if (ShaderHashes[CrossCompiler::SHADER_STAGE_COMPUTE] != FSHAHash())
			{
				retme = TEXT("Program C_") + ShaderHashes[CrossCompiler::SHADER_STAGE_COMPUTE].ToString();
				return retme;
			}
			else
			{
				retme = TEXT("Program with unset key");
				return retme;
			}
		}

		FSHAHash ShaderHashes[CrossCompiler::NUM_SHADER_STAGES];*/
	};

	class FOpenGLCompiledShaderKey
	{
	public:
		FOpenGLCompiledShaderKey() = default;
		FOpenGLCompiledShaderKey(
			GLenum InTypeEnum,
			uint32_t InCodeSize,
			uint32_t InCodeCRC
		)
			: TypeEnum(InTypeEnum)
			, CodeSize(InCodeSize)
			, CodeCRC(InCodeCRC)
		{
		}

		friend bool operator == (const FOpenGLCompiledShaderKey& A, const FOpenGLCompiledShaderKey& B)
		{
			return A.TypeEnum == B.TypeEnum && A.CodeSize == B.CodeSize && A.CodeCRC == B.CodeCRC;
		}

		//TODO:: 
		friend uint32_t GetTypeHash(const FOpenGLCompiledShaderKey& Key)
		{
			assert(0);
			return 0;
			//return GetTypeHash(Key.TypeEnum) ^ GetTypeHash(Key.CodeSize) ^ GetTypeHash(Key.CodeCRC);
		}

		uint32_t GetCodeCRC() const { return CodeCRC; }

	private:
		GLenum TypeEnum = 0;
		uint32_t CodeSize = 0;
		uint32_t CodeCRC = 0;
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



}