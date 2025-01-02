#pragma once

float* stbi_loadf_util(char const* filename, int* x, int* y, int* comp, int req_comp);
void stbi_image_free_util(void* retval_from_stbi_load);
void stbi_set_flip_vertically_on_load_util(int flag_true_if_should_flip);

unsigned char* stbi_load_util(char const* filename, int* x, int* y, int* comp, int req_comp);