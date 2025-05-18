//
// Created by DXT00 on 2025/4/13.
//


#pragma once

#include "BlackPearl/Window.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
//#ifdef __cplusplus
//extern "C" {
//#endif
//struct ANativeWindow;
//#ifdef __cplusplus
//}
//#endif
#include "BlackPearl/ApplicationCore/Android/AndroidApplication.h"
#if USE_ANDROID_JNI
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>
#endif

namespace BlackPearl {


    /**
     * A platform specific implementation of FNativeWindow.
     * Native windows provide platform-specific backing for and are always owned by an SWindow.
     */
    class AndroidWindow : public Window {
    public:
        AndroidWindow();

        ~AndroidWindow();

        void Init() override;
        void OnUpdate() override;
        void SetCursorCallBack();
        bool ShouldClose() override;
        bool IsKeyPressed(int keycode) override;
        bool IsMouseButtonPressed(int button) override;
        std::pair<float, float> GetMousePosition() override;
        //void* GetNativeWindow() const override { return m_Window; }

        math::vector<int, 2> GetCurWindowSize() override;
        void* GetNativeWindow() const override {
            return nullptr;
        }
        virtual unsigned int GetHeight() override;
        virtual unsigned int GetWidth() override;
           // void*  m_WindowHandle = nullptr;

        /** Create a new AndroidWindow.
         *
         * @param OwnerWindow		The SlateWindow for which we are crating a backing AndroidWindow
         * @param InParent			Parent iOS window; usually NULL.
         */
        static  std::shared_ptr<AndroidWindow> Make();
        
         void *GetOSWindowHandle() const  { return nullptr; }

         void Initialize(class AndroidApplication *const Application,
                                           // const std::shared_ptr <FGenericWindowDefinition> &InDefinition,
                                            const std::shared_ptr <AndroidWindow> &InParent, const bool bShowImmediately);

        /** Returns the rectangle of the screen the window is associated with */
          bool GetFullScreenInfo(int32_t &X, int32_t &Y, int32_t &Width, int32_t &Height) const ;

          void SetOSWindowHandle(void *);

        static  RHIRect GetScreenRect(bool bUseEventThreadWindow = false);

        static  void InvalidateCachedScreenRect();

        // When bUseEventThreadWindow == false this uses dimensions cached when the game thread processes android events.
        // When bUseEventThreadWindow == true this uses dimensions directly from the android event thread, unless called from event thread this requires acquiring GAndroidWindowLock to use.
        static  void CalculateSurfaceSize(int32_t &SurfaceWidth, int32_t &SurfaceHeight, bool bUseEventThreadWindow = false);

        static  bool OnWindowOrientationChanged(int Orientation);

        static  int32_t GetDepthBufferPreference();

        static  void AcquireWindowRef(ANativeWindow *InWindow);

        static  void ReleaseWindowRef(ANativeWindow *InWindow);

        // This returns the current hardware window as set from the event thread.
        static  void *GetHardwareWindow_EventThread();

        static  void SetHardwareWindow_EventThread(void *InWindow);

        /** Waits on the current thread for a hardware window and returns it.
         *  May return nullptr if the application is shutting down.
         */
        static  void *WaitForHardwareWindow();

        static  bool IsPortraitOrientation();

        static  math::float4 GetSafezone(bool bPortrait);

        static  bool SafezoneUpdated();

        // called by the Android event thread to initially set the current window dimensions.
        static  void SetWindowDimensions_EventThread(ANativeWindow *DimensionWindow);

        // Called by the event manager to update the cached window dimensions to match the event it is processing.
        static  void EventManagerUpdateWindowDimensions(int32_t Width, int32_t Height);

    protected:
        /** @return true if the native window is currently in fullscreen mode, false otherwise */
      //  virtual EWindowMode::Type GetWindowMode() const override { return EWindowMode::Fullscreen; }

    private:
        /**
         * Protect the constructor; only TSharedRefs of this class can be made.
         */

        AndroidApplication *OwningApplication;

        /** Store the window region size for querying whether a point lies within the window */
        int32_t RegionX;
        int32_t RegionY;

        static  void *NativeWindow;

        // Waits for the event thread to report an initial window size.
        static  bool WaitForWindowDimensions();

        static  bool bAreCachedNativeDimensionsValid;
        static  int32_t
        CachedNativeWindowWidth;
        static  int32_t
        CachedNativeWindowHeight;

       // void AcquireWindowRef(ANativeWindow *InWindow);
    };
    typedef AndroidWindow OpenGLWindow;

}