#ifdef GE_PLATFORM_WINDOWS
#if _DEBUG 
#define FORCEINLINE	inline									/* Easier to debug */
#else
#define FORCEINLINE	 									    /* Easier to debug */
#endif

#elif defined(GE_PLATFORM_ANDRIOD)
#if _DEBUG 
#define FORCEINLINE	inline									/* Easier to debug */
#else
#define FORCEINLINE inline __attribute__ ((always_inline))	/* Force code to be inline */
#endif
#endif

