//
// Created by DXT00 on 2025/4/14.
//

#pragma once
#include <string>
#include <vector>
#include <jni.h>
namespace BlackPearl
{

    namespace AndroidJavaEnv
    {
        // Returns the java environment
        void InitializeJavaEnv(JavaVM* VM, jint Version, jobject GlobalThis);
        jobject GetGameActivityThis();
        jobject GetClassLoader();
        JNIEnv* GetJavaEnv(bool bRequireGlobalThis = true);
        jclass FindJavaClass(const char* name);
        jclass FindJavaClassGlobalRef(const char* name);
        void DetachJavaEnv();
        bool CheckJavaException();
    }

// Helper class that automatically calls DeleteLocalRef on the passed-in Java object when goes out of scope
    template <typename T>
    class FScopedJavaObject
    {
    public:
        FScopedJavaObject(JNIEnv* InEnv, const T& InObjRef) :
                Env(InEnv),
                ObjRef(InObjRef)
        {}

        FScopedJavaObject(FScopedJavaObject&& Other) :
                Env(Other.Env),
                ObjRef(Other.ObjRef)
        {
            Other.Env = nullptr;
            Other.ObjRef = nullptr;
        }

        FScopedJavaObject(const FScopedJavaObject& Other) = delete;
        FScopedJavaObject& operator=(const FScopedJavaObject& Other) = delete;

        ~FScopedJavaObject()
        {
            if (*this)
            {
                if constexpr (std::is_same_v<T, jobjectArray>)
                {
                    const jsize Length = Env->GetArrayLength(ObjRef);
                    for(jsize Idx = 0; Idx < Length; ++Idx)
                    {
                        jobject Element = Env->GetObjectArrayElement(ObjRef, Idx);
                        if (Element && !Env->IsSameObject(Element, NULL))
                        {
                            Env->DeleteLocalRef(Element);
                        }
                    }
                }
                Env->DeleteLocalRef(ObjRef);
            }
        }

        // Returns the underlying JNI pointer
        T operator*() const { return ObjRef; }

        operator bool() const
        {
            if (!Env || !ObjRef || Env->IsSameObject(ObjRef, NULL))
            {
                return false;
            }

            return true;
        }

    private:
        JNIEnv* Env = nullptr;
        T ObjRef = nullptr;
    };

/**
 Helper function that allows template deduction on the java object type, for example:
 auto ScopeObject = NewScopedJavaObject(Env, JavaString);
 instead of FScopedJavaObject<jstring> ScopeObject(Env, JavaString);
 */
    template <typename T>
     FScopedJavaObject<T> NewScopedJavaObject(JNIEnv* InEnv, const T& InObjRef)
{
    return FScopedJavaObject<T>(InEnv, InObjRef);
}

class FJavaHelper
{
public:
    // Converts the java string to std::string and calls DeleteLocalRef on the passed-in java string reference
    static std::string FStringFromLocalRef(JNIEnv* Env, jstring JavaString);

    // Converts the java string to std::string and calls DeleteGlobalRef on the passed-in java string reference
    static std::string FStringFromGlobalRef(JNIEnv* Env, jstring JavaString);

    // Converts the java string to std::string, does NOT modify the passed-in java string reference
    static std::string FStringFromParam(JNIEnv* Env, jstring JavaString);

    // Converts std::string into a Java string wrapped in FScopedJavaObject
    static FScopedJavaObject<jstring> ToJavaString(JNIEnv* Env, const std::string& UnrealString);

    // Converts a TArray<FStringView> into a Java string array wrapped in FScopedJavaObject. FStringView content is expected to be null terminated
    static FScopedJavaObject<jobjectArray> ToJavaStringArray(JNIEnv* Env, const std::vector<std::string>& UnrealStrings);

    // Converts the java objectArray to an array of FStrings. jopbjectArray must be a String[] on the Java side
    static std::vector<std::string> ObjectArrayToFStringTArray(JNIEnv* Env, jobjectArray ObjectArray);
};
    
}
