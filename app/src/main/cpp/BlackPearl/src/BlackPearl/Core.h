#pragma once
#include"Log.h"

#ifdef GE_PLATFORM_WINDOWS
#define PLATFORM_HEADER_NAME Windows
#elif defined(GE_PLATFORM_ANDRIOD)
#define PLATFORM_HEADER_NAME Andriod
#endif

//#define PREPROCESSOR_TO_STRING(Token) #Token
#define GE_STRINGIZE(Token) GE_PRIVATE_STRINGIZE(Token)
#define GE_PRIVATE_STRINGIZE(Token) #Token
#define PREPROCESSOR_TO_STRING(Token)   GE_STRINGIZE(Token)

#define GE_JOIN(TokenA, TokenB) TokenA##TokenB

#define GE_PRIVATE_JOIN(TokenA, TokenB) 
#define PREPROCESSOR_JOIN(TokenA, TokenB)             GE_JOIN(TokenA, TokenB)

#define COMPILED_PLATFORM_HEADER(Suffix) PREPROCESSOR_TO_STRING(PREPROCESSOR_JOIN(PLATFORM_HEADER_NAME/PLATFORM_HEADER_NAME, Suffix))


#ifdef GE_DEBUG
#define GE_ENABLE_ASSERTS
#endif



#ifdef GE_ENABLE_ASSERTS
#define GE_ASSERT(x,...) {if(!(x)){GE_CORE_ERROR("Assertion Failed:{0}",__VA_ARGS__);DEBUG_BREAK();}}
#else
#define GE_ASSERT(x,...) 
#endif




#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define _CRTDBG_MAP_ALLOC

//ʹ��ģ�建��
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

#define GE_INVALID_ENUM() GE_ASSERT(0,"Invalid enum value")
#define GE_INVALID_NOTSUPPORT() GE_ASSERT(0,"Not Supported")



//#define TIME_DEBUG