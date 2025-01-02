#include "pch.h"
#include "stb_util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float* stbi_loadf_util(char const* filename, int* x, int* y, int* comp, int req_comp)
{
    return stbi_loadf( filename,  x,  y,  comp, req_comp);
}
 unsigned char* stbi_load_util(char const* filename, int* x, int* y, int* comp, int req_comp)
{
	
	return stbi_load(filename, x, y, comp, req_comp);
}

void stbi_image_free_util(void* retval_from_stbi_load)
{
	stbi_image_free(retval_from_stbi_load);
}

 void stbi_set_flip_vertically_on_load_util(int flag_true_if_should_flip)
{
	 stbi_set_flip_vertically_on_load(flag_true_if_should_flip);
}
