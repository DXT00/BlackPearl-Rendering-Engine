#include "pch.h"

#include "LightProbes/LightProbeManager.h"

#include "Map/Area.h"
#include "Math/Math.h"
#include "Map/MapManager.h"
#include "Renderer/MasterRenderer/GI/IBLProbeRenderer.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
#include "Renderer/MasterRenderer/GI/SSGIRenderer.h"

namespace BlackPearl {


	LightProbeManager::LightProbeManager() {

	}

	LightProbeManager* LightProbeManager::GetInstance()
	{
		static LightProbeManager s_Instance;
		return &s_Instance;
	}

	std::vector<Object*> LightProbeManager::FindKnearProbes(math::float3 objPos, std::vector<Object*> probes, unsigned int k)
	{
		math::float3 pos = objPos;
		std::vector<Object*> kProbes;
		std::sort(probes.begin(), probes.end(), [=](Object* pa, Object* pb)
			{return glm::length(pa->GetComponent<Transform>()->GetPosition() - Math::ToVec3(pos)) < glm::length(pb->GetComponent<Transform>()->GetPosition() - Math::ToVec3(pos)); });

		GE_ASSERT(k <= (unsigned int)probes.size(), "m_K larger than probes' number!");

		for (int i = 0; i < k; i++)
		{
			kProbes.push_back(probes[i]);
		}


		return kProbes;
	}
	//only for diffuse probe
	std::vector<unsigned int> LightProbeManager::FindKnearAreaProbes(math::float3 objPos, std::vector<Object*> probes, unsigned int k, MapManager* mapManager)
	{
		//math::float3 pos = objPos;
		//Area currentArea = mapManager->GetArea( mapManager->CalculateAreaId(pos));
		//std::set<unsigned int> nearByArea = mapManager->FindNearByArea(pos);

		//std::vector<LightProbe*> nearByProbes;
		//std::set<unsigned int>::iterator it;
		//for (it = nearByArea.begin(); it != nearByArea.end(); it++) {
		//	Area area = mapManager->GetArea(*it);
		//	for (auto probeIdx:area.GetProbesId())
		//	{
		//		nearByProbes.push_back(probes[probeIdx]);
		//	}
		//}
		//for (auto probeIdx : currentArea.GetProbesId())
		//{
		//	nearByProbes.push_back(probes[probeIdx]);
		//}
		//std::vector<LightProbe*> kProbes;
		//std::sort(nearByProbes.begin(), nearByProbes.end(), [=](LightProbe* pa, LightProbe* pb)
		//{return glm::length(pa->GetPosition() - pos) < glm::length(pb->GetPosition() - pos); });

		////if(k > (unsigned int)nearByProbes.size())
		////	GE_CORE_WARN( "m_K {0} larger than nearby probes' number:{1}!", k, (unsigned int)nearByProbes.size());

		//int kMin = glm::min(k, nearByProbes.size());
		////if (k <=0)
		////	GE_CORE_WARN("no probe found near this object!");

		//for (int i = 0; i < kMin; i++)
		//{
		//	kProbes.push_back(nearByProbes[i]);
		//}


		//return kProbes;
		math::float3 pos = objPos;
		Area* currentArea = mapManager->GetArea(mapManager->CalculateAreaId(Math::ToVec3(pos)));
		std::set<unsigned int> nearByArea = mapManager->FindNearByArea(Math::ToVec3(pos));

		std::vector<unsigned int> nearByProbes;
		std::set<unsigned int>::iterator it;
		for (it = nearByArea.begin(); it != nearByArea.end(); it++) {
			Area* area = mapManager->GetArea(*it);
			for (auto probeIdx : area->GetProbesId())
			{
				nearByProbes.push_back(probeIdx);
			}
		}
		for (auto probeIdx : currentArea->GetProbesId())
		{
			nearByProbes.push_back(probeIdx);
		}
		std::vector<unsigned int> kProbes;
		std::sort(nearByProbes.begin(), nearByProbes.end(), [=](unsigned int a, unsigned int b)
			{return glm::length(probes[a]->GetComponent<Transform>()->GetPosition() - Math::ToVec3(pos)) < glm::length(probes[b]->GetComponent<Transform>()->GetPosition() - Math::ToVec3(pos)); });

		//if(k > (unsigned int)nearByProbes.size())
		//	GE_CORE_WARN( "m_K {0} larger than nearby probes' number:{1}!", k, (unsigned int)nearByProbes.size());

		int kMin = std::min(k, (unsigned int)nearByProbes.size());
		//if (k <=0)
		//	GE_CORE_WARN("no probe found near this object!");

		for (int i = 0; i < kMin; i++)
		{
			kProbes.push_back(nearByProbes[i]);
		}


		return kProbes;
	}

    ProbeRenderer* LightProbeManager::CreateProbeRenderer(IDevice* device, ProbeGenerateMethod method)
    {
        switch (method)
        {
        case BlackPearl::DDGI:
            return  DBG_NEW DDGIRenderer(device);
        case BlackPearl::RTXDI:
            //todo::
            return nullptr;
        case BlackPearl::IBL:
            return  DBG_NEW IBLProbeRenderer(device);
        case BlackPearl::SSGI:
            return  DBG_NEW SSGIRenderer(device);
        default:
            break;
        }
        return nullptr;
    }

}