#pragma once
#include"Log.h"



#ifdef GE_PLATFORM_WINDOWS
#define PLATFORM_HEADER_NAME Windows
#elif defined(GE_PLATFORM_ANDROID)
#define PLATFORM_HEADER_NAME Android

#endif

//#define PREPROCESSOR_TO_STRING(Token) #Token
#define GE_STRINGIZE(Token) GE_PRIVATE_STRINGIZE(Token)
#define GE_PRIVATE_STRINGIZE(Token) #Token
#define PREPROCESSOR_TO_STRING(Token)   GE_STRINGIZE(Token)

#define GE_JOIN(TokenA, TokenB) TokenA##TokenB

#define GE_PRIVATE_JOIN(TokenA, TokenB) 
#define PREPROCESSOR_JOIN(TokenA, TokenB)             GE_JOIN(TokenA, TokenB)

#define COMPILED_PLATFORM_HEADER(Suffix) PREPROCESSOR_TO_STRING(PREPROCESSOR_JOIN(PLATFORM_HEADER_NAME/PLATFORM_HEADER_NAME, Suffix))

#define COMPILED_PLATFORM_CLASS(class) PREPROCESSOR_JOIN(PLATFORM_HEADER_NAME, class)
#ifdef GE_DEBUG
#define GE_ENABLE_ASSERTS
#endif

#define GE_CURRENT_CONTEXT(DC,RC) \
	GE_CORE_WARN("Make current context ,DC:"+ #DC+"RC:" #RC)

#ifdef GE_ENABLE_ASSERTS
#define GE_ASSERT(x,...) {if(!(x)){GE_CORE_ERROR("Assertion Failed:{0}",__VA_ARGS__);DEBUG_BREAK();}}
#define GE_ASSERT(x) {if(!(x)){GE_CORE_ERROR("Assertion Failed");DEBUG_BREAK();}}
#else
//#define GE_ASSERT(x,msg)// {if(!(x)){LOGI("Assertion Failed: %s",msg);DEBUG_BREAK();}}
#define GE_ASSERT(x,...) //{if(!(x)){GE_CORE_INFO(msg); DEBUG_BREAK();}}
#define GE_ASSERT(...)
#endif




#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define _CRTDBG_MAP_ALLOC


#ifdef GE_STENCIL_TEST
#define GE_ENABLE_STENCIL_TEST
#endif // GE_STENCIL_TEST


/*LightProbes*/
/*use spherical harmonics to calculate diffuse IrradianceMap*/
#define _SH


#define GE_SAVE_DELETE(p) {if(p!=nullptr){delete p;p = nullptr;}}
#define GE_SAVE_DELETE_ARRAY(p) {if(p!=nullptr){delete [] p;p = nullptr;}}

#define GE_SAVE_FREE(p) {if(p!=nullptr){free(p);p = nullptr;}}
#define GE_SAVE_FREE_ARRAY(p) {if(p!=nullptr){free(p);p = nullptr;}}

#ifdef GE_PLATFORM_WINDOWS
#ifdef GE_API_OPENGL
#define GE_ERROR_JUDGE()\
{\
	GLenum err = glGetError();\
	if (err == GL_NO_ERROR){ \
	}\
	else if (err == GL_INVALID_ENUM){ \
		GE_CORE_ERROR("GL_INVALID_ENUM");}\
	else if (err == GL_INVALID_VALUE){ \
		GE_CORE_ERROR("GL_INVALID_VALUE");}\
	else if (err == GL_INVALID_OPERATION){\
		GE_CORE_ERROR("GL_INVALID_OPERATION");}\
	else if (err == GL_INVALID_FRAMEBUFFER_OPERATION){ \
		GE_CORE_ERROR("GL_INVALID_FRAMEBUFFER_OPERATION");}\
	else if (err == GL_OUT_OF_MEMORY){ \
		GE_CORE_ERROR("GL_OUT_OF_MEMORY");}\
	else if (err == GL_STACK_UNDERFLOW){ \
		GE_CORE_ERROR("GL_STACK_UNDERFLOW");}\
	else if (err == GL_STACK_OVERFLOW){ \
		GE_CORE_ERROR("GL_STACK_OVERFLOW");}\
	else{\
		GE_CORE_ERROR("no such err type!"); }\
}
#endif

#elif defined GE_PLATFORM_ANDROID
#define GE_ERROR_JUDGE()\
{\
	GLenum err = glGetError();\
	if (err == GL_NO_ERROR){ \
	}\
	else if (err == GL_INVALID_ENUM){ \
		GE_CORE_ERROR("GL_INVALID_ENUM");}\
	else if (err == GL_INVALID_VALUE){ \
		GE_CORE_ERROR("GL_INVALID_VALUE");}\
	else if (err == GL_INVALID_OPERATION){\
		GE_CORE_ERROR("GL_INVALID_OPERATION");}\
	else if (err == GL_INVALID_FRAMEBUFFER_OPERATION){ \
		GE_CORE_ERROR("GL_INVALID_FRAMEBUFFER_OPERATION");}\
	else if (err == GL_OUT_OF_MEMORY){ \
		GE_CORE_ERROR("GL_OUT_OF_MEMORY");}\
	else{\
		GE_CORE_ERROR("no such err type!"); }\
}


#define GE_ERROR_JUDGE_EGL()\
{\
    GLenum err = eglGetError();\
    if (err == EGL_SUCCESS){ \
    }\
    else if (err == EGL_NOT_INITIALIZED){ \
        GE_CORE_ERROR("EGL_NOT_INITIALIZED");}\
    else if (err == EGL_BAD_ACCESS){ \
        GE_CORE_ERROR("EGL_BAD_ACCESS");}\
    else if (err == EGL_BAD_ALLOC){\
        GE_CORE_ERROR("EGL_BAD_ALLOC");}\
    else if (err == EGL_BAD_CONTEXT){ \
        GE_CORE_ERROR("EGL_BAD_CONTEXT");}\
    else if (err == EGL_BAD_CONFIG){ \
        GE_CORE_ERROR("EGL_BAD_CONFIG");}    \
    else if (err == EGL_BAD_CURRENT_SURFACE){ \
        GE_CORE_ERROR("EGL_BAD_CURRENT_SURFACE");}  \
    else if (err == EGL_BAD_DISPLAY){ \
        GE_CORE_ERROR("EGL_BAD_DISPLAY");}\
    else if (err == EGL_BAD_SURFACE){ \
        GE_CORE_ERROR("EGL_BAD_SURFACE");}\
    else if (err == EGL_BAD_MATCH){ \
        GE_CORE_ERROR("EGL_BAD_MATCH");}\
    else if (err == EGL_BAD_PARAMETER){ \
        GE_CORE_ERROR("EGL_BAD_PARAMETER");} \
    else if (err == EGL_BAD_PARAMETER){ \
        GE_CORE_ERROR("EGL_BAD_PARAMETER");} \
    else if (err == EGL_BAD_PARAMETER){ \
        GE_CORE_ERROR("EGL_BAD_PARAMETER");}\
    else if (err == EGL_BAD_PARAMETER){ \
        GE_CORE_ERROR("EGL_BAD_PARAMETER");}\
    else if (err == EGL_BAD_NATIVE_PIXMAP){ \
        GE_CORE_ERROR("EGL_BAD_NATIVE_PIXMAP");}\
    else if (err == EGL_CONTEXT_LOST){ \
        GE_CORE_ERROR("EGL_CONTEXT_LOST");}   \
    else if (err == EGL_BAD_NATIVE_WINDOW){ \
        GE_CORE_ERROR("EGL_BAD_NATIVE_WINDOW");} \
    else{\
        GE_CORE_ERROR("no such err type!"); }\
}

#endif
#define GE_INVALID_ENUM() GE_ASSERT(0,"Invalid enum value")
#define GE_INVALID_NOTSUPPORT() GE_ASSERT(0,"Not Supported")



#ifdef GE_PLATFORM_ANDROID

#define ENABLE_CONFIG_FILTER 1
#define ENABLE_EGL_DEBUG 0
#define ENABLE_VERIFY_EGL 0
#define ENABLE_VERIFY_EGL_TRACE 0

#if ENABLE_VERIFY_EGL

#define VERIFY_EGL(msg) { VerifyEGLResult(eglGetError(),TEXT(#msg),TEXT(""),TEXT(__FILE__),__LINE__); }

void VerifyEGLResult(EGLint ErrorCode, const TCHAR* Msg1, const TCHAR* Msg2, const TCHAR* Filename, uint32_t Line)
{
	if (ErrorCode != EGL_SUCCESS)
	{
		static const TCHAR* EGLErrorStrings[] =
		{
			TEXT("EGL_NOT_INITIALIZED"),
			TEXT("EGL_BAD_ACCESS"),
			TEXT("EGL_BAD_ALLOC"),
			TEXT("EGL_BAD_ATTRIBUTE"),
			TEXT("EGL_BAD_CONFIG"),
			TEXT("EGL_BAD_CONTEXT"),
			TEXT("EGL_BAD_CURRENT_SURFACE"),
			TEXT("EGL_BAD_DISPLAY"),
			TEXT("EGL_BAD_MATCH"),
			TEXT("EGL_BAD_NATIVE_PIXMAP"),
			TEXT("EGL_BAD_NATIVE_WINDOW"),
			TEXT("EGL_BAD_PARAMETER"),
			TEXT("EGL_BAD_SURFACE"),
			TEXT("EGL_CONTEXT_LOST"),
			TEXT("UNKNOWN EGL ERROR")
		};

		uint32_t ErrorIndex = FMath::Min<uint32_t>(ErrorCode - EGL_SUCCESS, UE_ARRAY_COUNT(EGLErrorStrings) - 1);
		UE_LOG(LogRHI, Warning, TEXT("%s(%u): %s%s failed with error %s (0x%x)"),
			Filename, Line, Msg1, Msg2, EGLErrorStrings[ErrorIndex], ErrorCode);
		check(0);
	}
}

class FEGLErrorScope
{
public:
	FEGLErrorScope(
		const TCHAR* InFunctionName,
		const TCHAR* InFilename,
		const uint32_t InLine)
		: FunctionName(InFunctionName)
		, Filename(InFilename)
		, Line(InLine)
	{
#if ENABLE_VERIFY_EGL_TRACE
		UE_LOG(LogRHI, Log, TEXT("EGL log before %s(%d): %s"), InFilename, InLine, InFunctionName);
#endif
		CheckForErrors(TEXT("Before "));
	}

	~FEGLErrorScope()
	{
#if ENABLE_VERIFY_EGL_TRACE
		UE_LOG(LogRHI, Log, TEXT("EGL log after  %s(%d): %s"), Filename, Line, FunctionName);
#endif
		CheckForErrors(TEXT("After "));
	}

private:
	const TCHAR* FunctionName;
	const TCHAR* Filename;
	const uint32_t Line;

	void CheckForErrors(const TCHAR* PrefixString)
	{
		VerifyEGLResult(eglGetError(), PrefixString, FunctionName, Filename, Line);
	}
};

#define MACRO_TOKENIZER(IdentifierName, Msg, FileName, LineNumber) FEGLErrorScope IdentifierName_ ## LineNumber (Msg, FileName, LineNumber)
#define MACRO_TOKENIZER2(IdentifierName, Msg, FileName, LineNumber) MACRO_TOKENIZER(IdentiferName, Msg, FileName, LineNumber)
#define VERIFY_EGL_SCOPE_WITH_MSG_STR(MsgStr) MACRO_TOKENIZER2(ErrorScope_, MsgStr, TEXT(__FILE__), __LINE__)
#define VERIFY_EGL_SCOPE() VERIFY_EGL_SCOPE_WITH_MSG_STR(ANSI_TO_TCHAR(__FUNCTION__))
#define VERIFY_EGL_FUNC(Func, ...) { VERIFY_EGL_SCOPE_WITH_MSG_STR(TEXT(#Func)); Func(__VA_ARGS__); }
#else
#define VERIFY_EGL(...)
#define VERIFY_EGL_SCOPE(...)
#endif
#endif

