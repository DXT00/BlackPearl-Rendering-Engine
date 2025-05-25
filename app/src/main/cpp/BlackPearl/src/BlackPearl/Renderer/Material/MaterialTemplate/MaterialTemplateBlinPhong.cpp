#include "pch.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplateBlinPhong.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
namespace BlackPearl {

   


	void MaterialTemplateBlinPhong::FillMaterialConstants(ICommandList* cmdLIst, Material* material)
	{
	}

	size_t MaterialTemplateBlinPhong::GetMaterialConstantSize()
	{
		return 0;
	}

}
