#pragma once
#pragma once
#if COMPILE_SHADER
#define ALIGN(X) 
#else
#define ALIGN(X) alignas(X)
#endif
