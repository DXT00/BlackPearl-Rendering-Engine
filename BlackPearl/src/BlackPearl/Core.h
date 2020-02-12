#pragma once
#include"Log.h"

#ifdef GE_DEBUG
#define GE_ENABLE_ASSERTS
#endif

#ifdef GE_ENABLE_ASSERTS
#define GE_ASSERT(x,...) {if(!(x)){GE_CORE_ERROR("Assertion Failed:{0}",__VA_ARGS__);__debugbreak();}}
#else
#define GE_ASSERT(x,...) 
#endif

#define BIT(x) 1<<x



#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define _CRTDBG_MAP_ALLOC

//Ê¹ÓÃÄ£°å»º³å
#ifdef GE_STENCIL_TEST
#define GE_ENABLE_STENCIL_TEST
#endif // GE_STENCIL_TEST


/*LightProbes*/
/*use spherical harmonics to calculate diffuse IrradianceMap*/
#define _SH