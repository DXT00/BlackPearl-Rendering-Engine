#pragma once
#include <vector>
#include <string>
#include "OpenGLDriver/OpenGLDrv.h"
#include "../RHIDefinitions.h"
#include "../RHIShader.h"
#include "BlackPearl/Renderer/Shader/CrossCompilerCommon.h"
#include "BlackPearl/Renderer/Shader/ShaderParameters.h"
#include "OpenGLProgramCache.h"

namespace BlackPearl {


    class BindingSet;
	struct FPackedArrayInfo
	{
		uint16_t	Size;		// Bytes
		uint8_t	TypeName;	// PACKED_TYPENAME
		uint8_t	TypeIndex;	// PACKED_TYPE
	};


	struct FOpenGLShaderVarying
	{
		std::vector<std::string> Varying;
		int32_t Location;

		/*friend bool operator==(const FOpenGLShaderVarying& A, const FOpenGLShaderVarying& B)
		{
			if (&A != &B)
			{
				return (A.Location == B.Location) && (A.Varying.size() == B.Varying.size()) && (FMemory::Memcmp(A.Varying.GetData(), B.Varying.GetData(), A.Varying.Num() * sizeof(ANSICHAR)) == 0);
			}
			return true;
		}*/

		//TODO::
		friend uint32_t GetTypeHash(const FOpenGLShaderVarying& Var)
		{
			return 0;
			/*uint32 Hash = GetTypeHash(Var.Location);
			Hash ^= FCrc::MemCrc32(Var.Varying.GetData(), Var.Varying.Num() * sizeof(ANSICHAR));
			return Hash;*/

		}
	};

	/**
 * Shader binding information. for shader bindingSet
 */
	struct FOpenGLShaderBindings
	{
		std::vector<std::vector<FPackedArrayInfo>>	PackedUniformBuffers;
		std::vector<FPackedArrayInfo>			PackedGlobalArrays;
		std::vector<FOpenGLShaderVarying>					InputVaryings;
		std::vector<FOpenGLShaderVarying>					OutputVaryings;
		//	FShaderResourceTable							ShaderResourceTable;
			//CrossCompiler::FShaderBindingInOutMask			InOutMask;

		uint8_t	NumSamplers;
		uint8_t	NumUniformBuffers;
		uint8_t	NumUAVs;
		bool	bFlattenUB;

		//FSHAHash VaryingHash; // Not serialized, built during load to allow us to diff varying info but avoid the memory overhead.

		FOpenGLShaderBindings() :
			NumSamplers(0),
			NumUniformBuffers(0),
			NumUAVs(0),
			bFlattenUB(false)
		{
		}

		friend bool operator==(const FOpenGLShaderBindings& A, const FOpenGLShaderBindings& B)
		{
			assert(0);
			return false;

			//bool bEqual = true;

			////bEqual &= A.InOutMask == B.InOutMask;
			//bEqual &= A.NumSamplers == B.NumSamplers;
			//bEqual &= A.NumUniformBuffers == B.NumUniformBuffers;
			//bEqual &= A.NumUAVs == B.NumUAVs;
			//bEqual &= A.bFlattenUB == B.bFlattenUB;
			//bEqual &= A.PackedGlobalArrays.size() == B.PackedGlobalArrays.size();
			//bEqual &= A.PackedUniformBuffers.size() == B.PackedUniformBuffers.size();
			//bEqual &= A.InputVaryings.size() == B.InputVaryings.size();
			//bEqual &= A.OutputVaryings.size() == B.OutputVaryings.size();
			////bEqual &= A.ShaderResourceTable == B.ShaderResourceTable;
			////bEqual &= A.VaryingHash == B.VaryingHash;

			//if (!bEqual)
			//{
			//	return bEqual;
			//}

			//bEqual &= FMemory::Memcmp(A.PackedGlobalArrays.data(), B.PackedGlobalArrays.data(), A.PackedGlobalArrays.GetTypeSize() * A.PackedGlobalArrays.Num()) == 0;

			//for (int32_t Item = 0; bEqual && Item < A.PackedUniformBuffers.size(); Item++)
			//{
			//	const TArray<CrossCompiler::FPackedArrayInfo>& ArrayA = A.PackedUniformBuffers[Item];
			//	const TArray<CrossCompiler::FPackedArrayInfo>& ArrayB = B.PackedUniformBuffers[Item];

			//	bEqual = bEqual && (ArrayA.Num() == ArrayB.Num()) && (FMemory::Memcmp(ArrayA.GetData(), ArrayB.GetData(), ArrayA.GetTypeSize() * ArrayA.Num()) == 0);
			//}


			//for (int32 Item = 0; bEqual && Item < A.InputVaryings.Num(); Item++)
			//{
			//	bEqual &= A.InputVaryings[Item] == B.InputVaryings[Item];
			//}

			//for (int32 Item = 0; bEqual && Item < A.OutputVaryings.Num(); Item++)
			//{
			//	bEqual &= A.OutputVaryings[Item] == B.OutputVaryings[Item];
			//}

			//return bEqual;
		}

		friend uint32_t GetTypeHash(const FOpenGLShaderBindings& Binding)
		{
			//uint32 Hash = 0;
			//Hash = Binding.InOutMask.Bitmask;
			//Hash ^= Binding.NumSamplers << 16;
			//Hash ^= Binding.NumUniformBuffers << 24;
			//Hash ^= Binding.NumUAVs;
			//Hash ^= Binding.bFlattenUB << 8;
			//Hash ^= FCrc::MemCrc_DEPRECATED(Binding.PackedGlobalArrays.GetData(), Binding.PackedGlobalArrays.GetTypeSize() * Binding.PackedGlobalArrays.Num());

			////@todo-rco: Do we need to calc Binding.ShaderResourceTable.GetTypeHash()?

			//for (int32 Item = 0; Item < Binding.PackedUniformBuffers.Num(); Item++)
			//{
			//	const TArray<CrossCompiler::FPackedArrayInfo>& Array = Binding.PackedUniformBuffers[Item];
			//	Hash ^= FCrc::MemCrc_DEPRECATED(Array.GetData(), Array.GetTypeSize() * Array.Num());
			//}

			//for (int32 Item = 0; Item < Binding.InputVaryings.Num(); Item++)
			//{
			//	Hash ^= GetTypeHash(Binding.InputVaryings[Item]);
			//}

			//for (int32 Item = 0; Item < Binding.OutputVaryings.Num(); Item++)
			//{
			//	Hash ^= GetTypeHash(Binding.OutputVaryings[Item]);
			//}

			//Hash ^= GetTypeHash(Binding.VaryingHash);

			//return Hash;
			return 0;
		}
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


    class FOpenGLLinkedProgramConfiguration
    {
    public:

        struct ShaderInfo
        {
            FOpenGLShaderBindings Bindings;
            GLuint Resource;
            FOpenGLCompiledShaderKey ShaderKey; // This is the key to the shader within FOpenGLCompiledShader container
            bool bValid; // To mark that stage is valid for this program, even when shader Resource could be zero
        } Shaders[ShaderType::NUM_COMPILE_SHADER_STAGES];


        FOpenGLProgramKey ProgramKey;
        std::vector<BindingSet*> bindingSet;

        FOpenGLLinkedProgramConfiguration()
        {
            for (int32_t Stage = 0; Stage < ShaderType::NUM_COMPILE_SHADER_STAGES; Stage++)
            {
                Shaders[Stage].Resource = 0;
                Shaders[Stage].bValid = false;
            }
        }

        friend bool operator ==(const FOpenGLLinkedProgramConfiguration& A, const FOpenGLLinkedProgramConfiguration& B)
        {
            bool bEqual = true;
            for (int32_t Stage = 0; Stage < ShaderType::NUM_COMPILE_SHADER_STAGES && bEqual; Stage++)
            {
                bEqual &= A.Shaders[Stage].Resource == B.Shaders[Stage].Resource;
                bEqual &= A.Shaders[Stage].bValid == B.Shaders[Stage].bValid;
                bEqual &= A.Shaders[Stage].Bindings == B.Shaders[Stage].Bindings;
            }
            return bEqual;
        }
        //TODO::
        friend uint32_t GetTypeHash(const FOpenGLLinkedProgramConfiguration& Config)
        {
            assert(0);
            return 0;
            //return GetTypeHash(Config.ProgramKey);
        }
    };

	/**
	 * Caching of OpenGL uniform parameters.
	 */
	class FOpenGLLinkedProgram;
	class FOpenGLShaderParameterCache :public ShaderParameters
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
		 void CommitPackedGlobals(const FOpenGLLinkedProgram* LinkedProgram, int32_t Stage);

		 //void CommitPackedUniformBuffers(FOpenGLLinkedProgram* LinkedProgram, int32_t Stage, FRHIUniformBuffer** UniformBuffers, const TArray<CrossCompiler::FUniformBufferCopyInfo>& UniformBuffersCopyInfo);
		 void CommitPackedUniformBuffers(FOpenGLLinkedProgram* LinkedProgram, int32_t Stage, const std::vector<BindingSetHandle>& bindings);

	private:

		/** CPU memory block for storing uniform values. */
		uint8_t* PackedGlobalUniforms[CrossCompiler::PACKED_TYPEINDEX_MAX];

		struct FRange
		{
			uint32_t	StartVector;
			uint32_t	NumVectors;

			void MarkDirtyRange(uint32_t NewStartVector, uint32_t NewNumVectors);
		};
		///** Dirty ranges for each uniform array. */
		FRange	PackedGlobalUniformDirty[CrossCompiler::PACKED_TYPEINDEX_MAX];

		/** Scratch CPU memory block for uploading packed uniforms. */
		uint8_t* PackedUniformsScratch[CrossCompiler::PACKED_TYPEINDEX_MAX];

		/** in bytes */
		int32_t GlobalUniformArraySize;
	};



}