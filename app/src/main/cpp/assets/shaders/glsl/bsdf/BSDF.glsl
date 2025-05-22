

#include <assets/shaders/glsl/common/TextureSample.glsl>

#if COOK
	#include <assets/shaders/glsl/bsdf/CookBSDF.glsl>
#elif (Disney)
	#include <assets/shaders/glsl/bsdf/DisneyBSDF.glsl>
#endif