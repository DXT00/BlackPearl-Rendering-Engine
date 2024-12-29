#include <jni.h>
#include <string>
#include "SandBoxAndriod.h"
#define NATIVE_RENDER_CLASS_NAME "com/example/blackpearl/SandBoxNativeRenderer"

//extern "C" JNIEXPORT jstring;
//
//JNIEXPORT jstring JNICALL stringFromJNI(
//        JNIEnv *env,
//jobject /* this */) {
//std::string hello = "Hello from xxx C++";
//return env->NewStringUTF(hello.c_str());
//}
#ifdef __cplusplus
extern "C" {
#endif





//JNICALL
//Java_com_example_blackpearl_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}

JNIEXPORT void JNICALL native_Init1(JNIEnv *env, jobject instance)
{
    int a = 1;
    SandBoxAndriod::GetInstance();
}

JNIEXPORT void JNICALL native_Run1(JNIEnv *env, jobject instance)
{
    SandBoxAndriod::GetInstance()->Run();
}


#ifdef __cplusplus
}
#endif

JNICALL


static JNINativeMethod g_RenderMethods[] = {
        {"native_Init",    "()V",       (void *)(native_Init1)},
        {"native_Run",      "()V",       (void *)(native_Run1)}

};


static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
   // GE_CORE_INFO("RegisterNativeMethods");
    //LOGCATE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        //GE_CORE_INFO("RegisterNativeMethods fail. clazz == NULL");
        //LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0)
    {
       // GE_CORE_INFO("RegisterNativeMethods fail");
        //LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className)
{
    //GE_CORE_INFO("UnregisterNativeMethods");
   // LOGCATE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
       // GE_CORE_INFO("UnregisterNativeMethods fail. clazz == NULL")
        //LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL)
    {
        env->UnregisterNatives(clazz);
    }
}

// call this func when loading lib
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p)
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

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p)
{
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return;
    }

    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);

}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_blackpearl_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from xxx C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_blackpearl_SandBoxNativeRenderer_native_1Init(JNIEnv *env, jobject thiz) {
//    SandBoxAndriod::GetInstance();
    SandBoxAndriod::add();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_blackpearl_SandBoxNativeRenderer_native_1Run(JNIEnv *env, jobject thiz) {
    SandBoxAndriod::GetInstance()->Run();
}