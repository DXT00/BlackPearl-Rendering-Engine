#ifndef ALINGN_H
#define ALINGN_H
#if COMPILE_SHADER
#define ALIGN(X) 
#else
#define ALIGN(X) alignas(X)
#endif
#endif