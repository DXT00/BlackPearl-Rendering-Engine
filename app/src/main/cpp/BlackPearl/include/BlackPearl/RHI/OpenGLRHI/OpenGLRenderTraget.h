#pragma once
#include "Core/Codec/Crc.h"
//#include "RHI/OpenGLRHI/OpenGLTexture.h"
//#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#include "RHI/OpenGLRHI/OpenGLTexture.h"
#include "RHI/OpenGLRHI/OpenGLUtil.h"
#include "RHI/OpenGLRHI/OpenGLContext.h"
#include "Core/Templates/TypeHash.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"

#include <stdint.h>
namespace BlackPearl {

#define ALL_SLICES uint32_t(0xffffffff)

    class FOpenGLFramebufferKey
    {
        struct RenderTargetInfo
        {
            Texture* Texture;
            GLuint			Resource;
            uint32_t			MipmapLevel;
            uint32_t			ArrayIndex;
        };

    public:

        FOpenGLFramebufferKey(
                uint32_t InNumRenderTargets,
                Texture** InRenderTargets,
                const uint32_t* InRenderTargetArrayIndices,
                const uint32_t* InRenderTargetMipmapLevels,
                Texture* InDepthStencilTarget,
                int32_t InNumRenderingSamples,
                EOpenGLCurrentContext InContext
        )
                : DepthStencilTarget(InDepthStencilTarget)
                , NumRenderingSamples(InNumRenderingSamples)
                , Context(InContext)
        {
            uint32_t RenderTargetIndex;
            for (RenderTargetIndex = 0; RenderTargetIndex < InNumRenderTargets; ++RenderTargetIndex)
            {
                FMemory::Memzero(RenderTargets[RenderTargetIndex]); // since memcmp is used, we need to zero memory
                RenderTargets[RenderTargetIndex].Texture = InRenderTargets[RenderTargetIndex];
                RenderTargets[RenderTargetIndex].Resource = (InRenderTargets[RenderTargetIndex]) ? InRenderTargets[RenderTargetIndex]->GetRendererID() : 0;
                RenderTargets[RenderTargetIndex].MipmapLevel = InRenderTargetMipmapLevels[RenderTargetIndex];
                RenderTargets[RenderTargetIndex].ArrayIndex = (InRenderTargetArrayIndices == NULL || InRenderTargetArrayIndices[RenderTargetIndex] == -1) ? ALL_SLICES : InRenderTargetArrayIndices[RenderTargetIndex];
            }
            for (; RenderTargetIndex < c_MaxRenderTargets; ++RenderTargetIndex)
            {
                FMemory::Memzero(RenderTargets[RenderTargetIndex]); // since memcmp is used, we need to zero memory
                RenderTargets[RenderTargetIndex].ArrayIndex = ALL_SLICES;
            }
        }

        /**
        * Equality is based on render and depth stencil targets
        * @param Other - instance to compare against
        * @return true if equal
        */
        friend bool operator ==(const FOpenGLFramebufferKey& A, const FOpenGLFramebufferKey& B)
        {
            return
                    !FMemory::Memcmp(A.RenderTargets, B.RenderTargets, sizeof(A.RenderTargets)) &&
                    A.DepthStencilTarget == B.DepthStencilTarget &&
                    A.NumRenderingSamples == B.NumRenderingSamples &&
                    A.Context == B.Context;
        }

        /**
        * Get the hash for this type.
        * @param Key - struct to hash
        * @return uint32_t hash based on type
        */
        friend uint32_t GetTypeHash(const FOpenGLFramebufferKey& Key)
        {
            return FCrc::MemCrc_DEPRECATED(Key.RenderTargets, sizeof(Key.RenderTargets)) ^ GetTypeHash(Key.DepthStencilTarget) ^ GetTypeHash(Key.NumRenderingSamples) ^ GetTypeHash(Key.Context);
        }

        const Texture* GetRenderTarget(int32_t Index) const { return RenderTargets[Index].Texture; }
        const Texture* GetDepthStencilTarget(void) const { return DepthStencilTarget; }
        int32_t GetNumRenderingSamples(void) const { return NumRenderingSamples; }

    private:

        RenderTargetInfo RenderTargets[c_MaxRenderTargets];
        Texture* DepthStencilTarget;
        int32_t NumRenderingSamples; // MSAA on tile
        EOpenGLCurrentContext Context;
    };

    struct FOpenGLFramebufferKeyHash {
        size_t operator()(const BlackPearl::FOpenGLFramebufferKey& key) const noexcept {
            uint32_t hashVal = GetTypeHash(key);
            uint32_t hashVal1 = GetTypeHash(key);

            return GetTypeHash(key);

        }
    };

    class FOpenGLFramebufferCache
    {
    public:

        /** Lazily initialized framebuffer cache singleton. */
        static FOpenGLFramebufferCache& Get()
        {
            static FOpenGLFramebufferCache OpenGLFramebufferCache;
            return OpenGLFramebufferCache;
        }

        // return 0, if key is not found
        GLuint Find(const FOpenGLFramebufferKey& key) {
            if (keyMap.find(key) != keyMap.end()) {
                return keyMap[key];
            }
            return 0;
        }
        //framebuffer id shuld add 1 before add to map
        void Add(const FOpenGLFramebufferKey& key, GLuint id) {
            keyMap[key] = id;
        }

    private:
        FOpenGLFramebufferCache() {

        }
        std::unordered_map<FOpenGLFramebufferKey, GLuint, FOpenGLFramebufferKeyHash> keyMap;

    };



}

