#include <jni.h>
#include <string>
#include "SandBoxAndriod.h"
#define NATIVE_RENDER_CLASS_NAME "com/example/blackpearl/SandBoxNativeRenderer"


extern "C" JNIEXPORT jstring


JNICALL
Java_com_example_blackpearl_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL native_Init(JNIEnv *env, jobject instance, jstring layer)
{
    SandBoxAndriod::GetInstance();
}

JNIEXPORT void JNICALL native_Run(JNIEnv *env, jobject instance)
{
    SandBoxAndriod::GetInstance()->Run();
}

static JNINativeMethod g_RenderMethods[] = {
        {"native_Init",    "()V",       (void *)(native_Init)},
        {"native_Run",      "()V",       (void *)(native_Run)}

};


static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
    GE_CORE_INFO("RegisterNativeMethods");
    //LOGCATE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        GE_CORE_INFO("RegisterNativeMethods fail. clazz == NULL");
        //LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0)
    {
        GE_CORE_INFO("RegisterNativeMethods fail");
        //LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className)
{
    GE_CORE_INFO("UnregisterNativeMethods");
   // LOGCATE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        GE_CORE_INFO("UnregisterNativeMethods fail. clazz == NULL")
        //LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL)
    {
        env->UnregisterNatives(clazz);
    }
}

// call this func when loading lib
extern "C" jint JNI_OnLoad_SandBoxAndriod(JavaVM *jvm, void *p)
{
    //LOGCATE("===== JNI_OnLoad =====");
    jint jniRet = JNI_ERR;
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return jniRet;
    }

    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_RenderMethods,
                                        sizeof(g_RenderMethods) /
                                        sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE)
    {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload_SandBoxAndriod(JavaVM *jvm, void *p)
{
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return;
    }

    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);

}




