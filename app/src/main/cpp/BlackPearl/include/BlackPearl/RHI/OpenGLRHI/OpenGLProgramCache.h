// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLProgramBinaryFileCache.h: OpenGL program binary file cache stores/loads a set of binary ogl programs.
=============================================================================*/

#pragma once
#include <string>
#include <map>
#include "Core/Codec/Crc.h"
#include "RHI/RHIDefinitions.h"
#include "Core/Codec/CrcHash.h"

namespace BlackPearl {
	//class FOpenGLLinkedProgram;
	//class FOpenGLProgramBinary;
	//class FOpenGLProgramBinaryMapping;
	//struct FGLProgramBinaryFileCacheEntry;

        // unique identifier for a program. (composite of shader keys)
    class FOpenGLProgramKey
    {
    public:
        FOpenGLProgramKey() {
        
            for (int i = ShaderType::VertexShader; i < ShaderType::NUM_COMPILE_SHADER_STAGES; i++) {
                ShaderHashes[i] = CrcHash();
            }
        
        }


        friend bool operator == (const FOpenGLProgramKey& A, const FOpenGLProgramKey& B)
        {
            bool bHashMatch = true;
            for (uint32_t i = VertexShader; i < ShaderType::NUM_COMPILE_SHADER_STAGES && bHashMatch; ++i)
            {
                bHashMatch = A.ShaderHashes[i] == B.ShaderHashes[i];
            }
            return bHashMatch;
        }

        friend bool operator != (const FOpenGLProgramKey& A, const FOpenGLProgramKey& B)
        {
            return !(A == B);
        }

        // 重载小于运算符
        bool operator<(const FOpenGLProgramKey& other) const {
            
            for (uint32_t i = VertexShader; i < ShaderType::NUM_COMPILE_SHADER_STAGES ; ++i)
            {
                if (this->ShaderHashes[i].ToString() < other.ShaderHashes[i].ToString())
                    return true;
            }
            return false;
        }

        bool operator()(const FOpenGLProgramKey& a, const FOpenGLProgramKey& b) const {
            // 比较逻辑
            for (uint32_t i = VertexShader; i < ShaderType::NUM_COMPILE_SHADER_STAGES; ++i)
            {
                if (a.ShaderHashes[i].ToString() < b.ShaderHashes[i].ToString())
                    return true;
            }
            return false;
        }

        friend uint32_t GetTypeHash(const FOpenGLProgramKey& Key)
        {
            return FCrc::MemCrc32(Key.ShaderHashes, sizeof(Key.ShaderHashes));
        }

        /*friend FArchive& operator<<(FArchive& Ar, FOpenGLProgramKey& HashSet)
        {
            for (int32 Stage = 0; Stage < CrossCompiler::NUM_SHADER_STAGES; Stage++)
            {
                Ar << HashSet.ShaderHashes[Stage];
            }
            return Ar;
        }*/

        std::string ToString() const
            {
                std::string retme;
                if (ShaderHashes[ShaderType::VertexShader] != CrcHash()
                    && ShaderHashes[ShaderType::Pixel] != CrcHash())
                {
                    retme = ("Program V_") + ShaderHashes[ShaderType::VertexShader].ToString();
                    retme += ("_P_") + ShaderHashes[ShaderType::Pixel].ToString();
                    return retme;
                }
                else if (ShaderHashes[ShaderType::Compute] != CrcHash())
                {
                    retme = ("Program C_") + ShaderHashes[ShaderType::Compute].ToString();
                    return retme;
                }
                else
                {
                    retme = ("Program with unset key");
                    return retme;
                }
            }

        bool isValid() {
            for (int i = ShaderType::VertexShader; i < ShaderType::NUM_COMPILE_SHADER_STAGES; i++) {
                if (ShaderHashes[i] != CrcHash())
                    return true;
            }
            return false;
        }

        CrcHash ShaderHashes[ShaderType::NUM_COMPILE_SHADER_STAGES];
    };

    //todo:: save program to binary file
	class FOpenGLProgramBinaryCache
	{
	public:
		static void Initialize();

	};
    
    struct FOpenGLProgramKeyHash {
        size_t operator()(const BlackPearl::FOpenGLProgramKey& key) const noexcept {
            size_t crchash = CrcHash::Crc32((uint8_t*)key.ToString().c_str(), key.ToString().length());
            size_t crchash1 = CrcHash::Crc32((uint8_t*)key.ToString().c_str(), key.ToString().length());

            return CrcHash::Crc32((uint8_t*)key.ToString().c_str(), key.ToString().length());
          
        }
    };
    // cache for glprogram, prevent from multi link of shaders
    class FOpenGLLinkedProgram;
    class FGLProgramCache {
    public:

        static FGLProgramCache* Get();

        FOpenGLLinkedProgram* GetGLProgram(FOpenGLProgramKey key);
        void AddGLProgram(FOpenGLProgramKey key, FOpenGLLinkedProgram* program);
    private:
        FGLProgramCache();
        ~FGLProgramCache();

        std::unordered_map<FOpenGLProgramKey, FOpenGLLinkedProgram*, FOpenGLProgramKeyHash> m_ProgramMap;
    };
}


//namespace std {
//    template<>
//    struct hash<BlackPearl::FOpenGLProgramKey> {
//        size_t operator()(const BlackPearl::FOpenGLProgramKey& key) const noexcept {
//            // 实现特化
//        }
//    };
//}