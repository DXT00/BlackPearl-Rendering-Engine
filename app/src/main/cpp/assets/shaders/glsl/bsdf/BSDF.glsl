

#include <assets/shaders/glsl/common/CommonTextureSample.glsl>

#if COOK
	#include <assets/shaders/glsl/bsdf/CookBSDF.glsl>
#elif (Disney)
	#include <assets/shaders/glsl/bsdf/DisneyBSDF.glsl>
#endif