//
// Created by DXT00 on 2025/4/14.
//
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/Android/AndroidJavaEnv.h"
#include "Core.h"
#include "Core/PlatformMisc.h"
#include "Core/PlatformProc.h"
namespace BlackPearl{
    

//////////////////////////////////////////////////////////////////////////
// FJNIHelper
static JavaVM* CurrentJavaVM = nullptr;
static jint CurrentJavaVersion;
static jobject GlobalObjectRef;
static jobject ClassLoader;
static jmethodID FindClassMethod;


// Caches access to the environment, attached to the current thread
class FJNIHelper
{
public:
    static JNIEnv* GetEnvironment()
    {
        return Get()->CachedEnv;
    }
    static FJNIHelper* Get()  {
        static FJNIHelper singleton;
        return &singleton;
    }
private:
    JNIEnv* CachedEnv = NULL;

private:
    //friend class TThreadSingleton<FJNIHelper>;
    static FJNIHelper _instance;


    FJNIHelper()
            : CachedEnv(nullptr)
    {
        GE_ASSERT(CurrentJavaVM);
        CurrentJavaVM->GetEnv((void **)&CachedEnv, CurrentJavaVersion);

        const jint AttachResult = CurrentJavaVM->AttachCurrentThread(&CachedEnv, nullptr);
        if (AttachResult == JNI_ERR)
        {
           // FPlatformMisc::LowLevelOutputDebugString(TEXT("FJNIHelper failed to attach thread to Java VM!"));
            GE_ASSERT(false);
        }
    }

    ~FJNIHelper()
    {
        GE_ASSERT(CurrentJavaVM);
        const jint DetachResult = CurrentJavaVM->DetachCurrentThread();
        if (DetachResult == JNI_ERR)
        {
            //FPlatformMisc::LowLevelOutputDebugString(TEXT("FJNIHelper failed to detach thread from Java VM!"));
            GE_ASSERT(false);
        }

        CachedEnv = nullptr;
    }
};

void AndroidJavaEnv::InitializeJavaEnv( JavaVM* VM, jint Version, jobject GlobalThis )
{
    if (CurrentJavaVM == nullptr)
    {
        CurrentJavaVM = VM;
        CurrentJavaVersion = Version;

        JNIEnv* Env = GetJavaEnv(false);
        jclass MainClass = Env->FindClass("com/epicgames/unreal/GameActivity");
        jclass classClass = Env->FindClass("java/lang/Class");
        jclass classLoaderClass = Env->FindClass("java/lang/ClassLoader");
        jmethodID getClassLoaderMethod = Env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
        auto classLoader = NewScopedJavaObject(Env, Env->CallObjectMethod(MainClass, getClassLoaderMethod));
        ClassLoader = Env->NewGlobalRef(*classLoader);
        FindClassMethod = Env->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    }
    GlobalObjectRef = GlobalThis;
}

jobject AndroidJavaEnv::GetGameActivityThis()
{
    return GlobalObjectRef;
}

jobject AndroidJavaEnv::GetClassLoader()
{
    return ClassLoader;
}

static void JavaEnvDestructor(void*)
{
    //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("*** JavaEnvDestructor: %d"), FPlatformTLS::GetCurrentThreadId());
    AndroidJavaEnv::DetachJavaEnv();
}

JNIEnv* AndroidJavaEnv::GetJavaEnv( bool bRequireGlobalThis /*= true*/ )
{
    //@TODO: ANDROID: Remove the other version if the helper works well
#if 0
    if (!bRequireGlobalThis || (GlobalObjectRef != nullptr))
	{
		return FJNIHelper::GetEnvironment();
	}
	else
	{
		return nullptr;
	}
#endif
#if 0
    // not reliable at the moment.. revisit later

	// Magic static - *should* be thread safe
	//Android & pthread specific, bind a destructor for thread exit
	static uint32 TlsSlot = 0;
	if (TlsSlot == 0)
	{
		pthread_key_create((pthread_key_t*)&TlsSlot, &JavaEnvDestructor);
	}
	JNIEnv* Env = (JNIEnv*)FPlatformTLS::GetTlsValue(TlsSlot);
	if (Env == nullptr)
	{
		CurrentJavaVM->GetEnv((void **)&Env, CurrentJavaVersion);
		JavaVMAttachArgs Args;
		Args.version = CurrentJavaVersion;
		Args.group = nullptr;
		const FString& ThreadName = FThreadManager::GetThreadName(FPlatformTLS::GetCurrentThreadId());
		auto Name = StringCast<ANSICHAR>(*ThreadName);
		Args.name = Name.Get();

		jint AttachResult = CurrentJavaVM->AttachCurrentThread(&Env, &Args);
		if (AttachResult == JNI_ERR)
		{
			FPlatformMisc::LowLevelOutputDebugString(L"UNIT TEST -- Failed to get the JNI environment!");
			GE_ASSERT(false);
			return nullptr;
		}
		FPlatformTLS::SetTlsValue(TlsSlot, (void*)Env);
	}

	return (!bRequireGlobalThis || (GlobalObjectRef != nullptr)) ? Env : nullptr;
#else
    // register a destructor to detach this thread
    static uint32_t TlsSlot = 0;
    if (TlsSlot == 0)
    {
        pthread_key_create((pthread_key_t*)&TlsSlot, &JavaEnvDestructor);
    }

    JNIEnv* Env = nullptr;
    jint GetEnvResult = CurrentJavaVM->GetEnv((void **)&Env, CurrentJavaVersion);
    if (GetEnvResult == JNI_EDETACHED)
    {
        // attach to this thread
        JavaVMAttachArgs Args;
        Args.version = CurrentJavaVersion;
        Args.group = nullptr;
        const uint32_t ThreadId = FPlatformTLS::GetCurrentThreadId();
//        const std::string& ThreadName = FThreadManager::GetThreadName(ThreadId);
//        auto Name = StringCast<char>(*ThreadName);
        //Args.name = Name.Get();
        if (!Args.name)
        {
            Args.name = FAndroidMisc::GetThreadName(ThreadId);
        }

        jint AttachResult = CurrentJavaVM->AttachCurrentThread(&Env, &Args);
        if (AttachResult == JNI_ERR)
        {
           // FPlatformMisc::LowLevelOutputDebugString(TEXT("UNIT TEST -- Failed to attach thread to get the JNI environment!"));
            GE_ASSERT(false);
            return nullptr;
        }
        FPlatformTLS::SetTlsValue(TlsSlot, (void*)Env);
    }
    else if (GetEnvResult != JNI_OK)
    {
        //FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UNIT TEST -- Failed to get the JNI environment! Result = %d"), GetEnvResult);
        GE_ASSERT(false);
        return nullptr;

    }
    return Env;
#endif
}

jclass AndroidJavaEnv::FindJavaClass(const char* name)
{
    JNIEnv* Env = GetJavaEnv();
    if (!Env)
    {
        return nullptr;
    }
    jstring ClassNameObj = Env->NewStringUTF(name);
    jclass FoundClass = static_cast<jclass>(Env->CallObjectMethod(ClassLoader, FindClassMethod, ClassNameObj));
    CheckJavaException();
    Env->DeleteLocalRef(ClassNameObj);
    return FoundClass;
}

jclass AndroidJavaEnv::FindJavaClassGlobalRef(const char* name)
{
    JNIEnv* Env = GetJavaEnv();
    if (!Env)
    {
        return nullptr;
    }
    auto ClassNameObj = FJavaHelper::ToJavaString(Env, std::string((name)));
    auto FoundClass = NewScopedJavaObject(Env, static_cast<jclass>(Env->CallObjectMethod(ClassLoader, FindClassMethod, *ClassNameObj)));
    CheckJavaException();
    auto GlobalClass = (jclass)Env->NewGlobalRef(*FoundClass);
    return GlobalClass;
}

void AndroidJavaEnv::DetachJavaEnv()
{
    CurrentJavaVM->DetachCurrentThread();
}

bool AndroidJavaEnv::CheckJavaException()
{
    JNIEnv* Env = GetJavaEnv();
    if (!Env)
    {
        return true;
    }
    if (Env->ExceptionCheck())
    {
        Env->ExceptionDescribe();
        Env->ExceptionClear();
        //verify(false && "Java JNI call failed with an exception.");
        return true;
    }
    return false;
}

std::string FJavaHelper::FStringFromLocalRef(JNIEnv* Env, jstring JavaString)
{
    std::string ReturnString = FStringFromParam(Env, JavaString);

    if (Env && JavaString)
    {
        Env->DeleteLocalRef(JavaString);
    }

    return ReturnString;
}

std::string FJavaHelper::FStringFromGlobalRef(JNIEnv* Env, jstring JavaString)
{
    std::string ReturnString = FStringFromParam(Env, JavaString);

    if (Env && JavaString)
    {
        Env->DeleteGlobalRef(JavaString);
    }

    return ReturnString;
}

std::string FJavaHelper::FStringFromParam(JNIEnv* Env, jstring JavaString)
{
    if (!Env || !JavaString || Env->IsSameObject(JavaString, NULL))
    {
        return {};
    }

    const auto chars = Env->GetStringUTFChars(JavaString, 0);
    std::string ReturnString(chars);
    Env->ReleaseStringUTFChars(JavaString, chars);
    return ReturnString;
}

FScopedJavaObject<jstring> FJavaHelper::ToJavaString(JNIEnv* Env, const std::string& UnrealString)
{
    GE_ASSERT(Env);
    return NewScopedJavaObject(Env, Env->NewStringUTF(UnrealString.c_str()));
}

FScopedJavaObject<jobjectArray> FJavaHelper::ToJavaStringArray(JNIEnv* Env, const std::vector<std::string>& UnrealStrings)
{
    jclass JavaStringClass = AndroidJavaEnv::FindJavaClass("java/lang/String");
    jobjectArray ObjectArray = Env->NewObjectArray((jsize)UnrealStrings.size(), JavaStringClass, NULL);
    for (int32_t Idx = 0; Idx < UnrealStrings.size(); ++Idx)
    {
        // FStringView of an empty std::string contains a null pointer as data
        if (UnrealStrings[Idx].data())
        {
            Env->SetObjectArrayElement(ObjectArray, Idx, Env->NewStringUTF((UnrealStrings[Idx].c_str())));
        }
        else
        {
            Env->SetObjectArrayElement(ObjectArray, Idx, Env->NewStringUTF(""));
        }
    }
    return NewScopedJavaObject(Env, ObjectArray);
}

std::vector<std::string> FJavaHelper::ObjectArrayToFStringTArray(JNIEnv* Env, jobjectArray ObjectArray)
{
    std::vector<std::string> ArrayOfStrings;
    if (Env && ObjectArray && !Env->IsSameObject(ObjectArray, NULL))
    {
        jsize Size = Env->GetArrayLength(ObjectArray);

        ArrayOfStrings.reserve(Size);

        for (jsize Idx = 0; Idx < Size; ++Idx)
        {
            std::string Entry = FStringFromLocalRef(Env, (jstring)Env->GetObjectArrayElement(ObjectArray, Idx));
            ArrayOfStrings.push_back(std::move(Entry));
        }
    }
    return ArrayOfStrings;
}

}