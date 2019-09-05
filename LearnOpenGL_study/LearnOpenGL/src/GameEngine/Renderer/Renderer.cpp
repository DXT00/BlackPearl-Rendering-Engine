#include "pch.h"
#include "Renderer.h"
#include<glad/glad.h>
Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::Init()
{
	glEnable(GL_DEPTH_TEST);
}
