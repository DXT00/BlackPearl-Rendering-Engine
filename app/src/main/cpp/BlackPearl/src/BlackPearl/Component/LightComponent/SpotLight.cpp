#include "pch.h"
#include "Component/LightComponent/SpotLight.h"


namespace BlackPearl {


	void SpotLight::Init()
	{
		
	}

    void SpotLight::FillLightConstants(LightConstants& lightConstants) {


        lightConstants = DefaultLightConstants();
        lightConstants.lightType = LightType_Spot;
        lightConstants.color = m_LightProp.diffuse;
        lightConstants.innerAngle = m_CutOffAngle;
        lightConstants.innerAngle = m_OuterCutOffAngle;
    }
}