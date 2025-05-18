#include "pch.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplatePBR.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplateCustom.h"
namespace BlackPearl {




	void MaterialTemplateCustom::FillMaterialConstants(ICommandList* cmdLIst, Material* material)
	{
	}

	size_t MaterialTemplateCustom::GetMaterialConstantSize()
	{
		return s0;
	}

}
