//
// Created by DXT00 on 2025/4/9.
//
#pragma once


#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl31.h>
struct AndroidESPImpl;
struct ANativeWindow;

namespace BlackPearl {
    struct AndroidESPImpl;

    struct FPlatformOpenGLContext {
        EGLContext eglContext;
        EGLSurface eglSurface;
        GLuint ViewportFramebuffer;
        GLuint DefaultVertexArrayObject;
        GLuint BackBufferResource;
        GLenum BackBufferTarget;
        GLuint DummyFrameBuffer;

        FPlatformOpenGLContext() {
            Reset();
        }

        void Reset() {
            eglContext = EGL_NO_CONTEXT;
            eglSurface = EGL_NO_SURFACE;
            ViewportFramebuffer = 0;
            DefaultVertexArrayObject = 0;
            BackBufferResource = 0;
            BackBufferTarget = 0;
            DummyFrameBuffer = 0;
        }
    };


    class AndroidEGL {
    public:
        enum APIVariant {
            AV_OpenGLES,
            AV_OpenGLCore
        };

        static AndroidEGL *GetInstance();

        ~AndroidEGL();

        bool IsInitialized();

        void InitBackBuffer();

        void DestroyBackBuffer();

        void Init(APIVariant API, uint32_t MajorVersion, uint32_t MinorVersion, bool bDebug);

        void ReInit();

        void UnBind();

        void UnBindRender();

        void UnBindShared();

        bool SwapBuffers();

        void Terminate();

        void InitSurface(bool bUseSmallSurface, bool bCreateWndSurface);

        void InitRenderSurface(bool bUseSmallSurface, bool bCreateWndSurface);

        void InitSharedSurface(bool bUseSmallSurface);

        void UpdateBuffersTransform();

        bool IsOfflineSurfaceRequired();

        void GetDimensions(uint32_t &OutWidth, uint32_t &OutHeight);

        EGLDisplay GetDisplay() const;

        EGLSurface GetSurface() const;

        EGLConfig GetConfig() const;

        ANativeWindow *GetNativeWindow() const;

        void GetSwapIntervalRange(EGLint &OutMinSwapInterval, EGLint &OutMaxSwapInterval) const;

        EGLContext CreateContext(EGLContext InSharedContext = EGL_NO_CONTEXT);

        int32_t GetError();

        EGLBoolean SetCurrentContext(EGLContext InContext, EGLSurface InSurface);

        void AcquireCurrentRenderingContext();

        void ReleaseContextOwnership();

        GLuint GetResolveFrameBuffer();

        bool IsCurrentContextValid();

        EGLContext GetCurrentContext();

        void SetCurrentSharedContext();

        void SetCurrentRenderingContext();

        uint32_t GetCurrentContextType();

        FPlatformOpenGLContext *GetRenderingContext();

        FPlatformOpenGLContext *GetSharedContext();

        bool GetSupportsNoErrorContext();

        // recreate the EGL surface for the current hardware window.
        void SetRenderContextWindowSurface();

        // Called from game thread when a window is reinited.
        void RefreshWindowSize();

    protected:
        AndroidEGL();

        static AndroidEGL *Singleton;

    private:
        void InitEGL(APIVariant API);

        void TerminateEGL();

        void CreateEGLRenderSurface(ANativeWindow *InWindow, bool bCreateWndSurface);

        void CreateEGLSharedSurface();

        void DestroyRenderSurface();

        void DestroySharedSurface();

        bool InitContexts();

        void DestroyContext(EGLContext InContext);

        void ResetDisplay();

        AndroidESPImpl *PImplData;

        void ResetInternal();

        void LogConfigInfo(EGLConfig EGLConfigInfo);

        // Actual Update to the egl surface to match the GT's requested size.
        void ResizeRenderContextSurface();

        void ResizeSharedContextSurface();

        bool bSupportsKHRCreateContext;
        bool bSupportsKHRSurfacelessContext;
        bool bSupportsKHRNoErrorContext;

        int *ContextAttributes;
    };

}

