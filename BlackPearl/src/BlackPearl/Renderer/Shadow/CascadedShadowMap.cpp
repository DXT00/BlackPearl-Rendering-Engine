#include "pch.h"
#include "CascadedShadowMap.h"
#include "ShadowMap2D.h"
namespace BlackPearl{
	CascadedShadowMap::CascadedShadowMap(IDevice* device, int resolution, int numCascades, int numPerObjectShadows, Format format, bool isUAV)
	{
		assert(numCascades > 0);
		assert(numCascades <= 4);

		TextureDesc desc;
		desc.width = resolution;
		desc.height = resolution;
		desc.sampleCount = 1;
		desc.isRenderTarget = true;
		desc.isTypeless = true;
		desc.format = format;
		desc.debugName = "ShadowMap";
		desc.useClearValue = true;
		desc.clearValue = Color(1.f);
		desc.initialState = ResourceStates::ShaderResource;
		desc.keepInitialState = true;
		desc.dimension = TextureDimension::Texture2DArray;
		desc.arraySize = numCascades + numPerObjectShadows;
		desc.isUAV = isUAV;
		m_ShadowMapTexture = device->createTexture(desc);

		RHIViewport cascadeViewport = RHIViewport(float(resolution), float(resolution));

		for (int cascade = 0; cascade < numCascades; cascade++)
		{
			std::shared_ptr<ShadowMap2D> planarShadowMap = std::make_shared<ShadowMap2D>(device, m_ShadowMapTexture, cascade, cascadeViewport);
			m_Cascades.push_back(planarShadowMap);

			m_CompositeView.push_back(planarShadowMap->GetPlanarView().get());
		}

		m_NumberOfCascades = 0;

		for (int object = 0; object < numPerObjectShadows; object++)
		{
			std::shared_ptr<ShadowMap2D> planarShadowMap = std::make_shared<ShadowMap2D>(device, m_ShadowMapTexture, numCascades + object, cascadeViewport);
			planarShadowMap->SetFalloffDistance(0.f); // disable falloff on per-object shadows: their bboxes are short by design

			m_PerObjectShadows.push_back(planarShadowMap);
		}
	}
	bool CascadedShadowMap::SetupForPlanarView(const ParallelLight& light, frustum viewFrustum, float maxShadowDistance, float lightSpaceZUp, float lightSpaceZDown, float exponent, float3 preViewTranslation, int numberOfCascades)
	{
		return false;
	}
	bool CascadedShadowMap::SetupForPlanarViewStable(const ParallelLight& light, frustum projectionFrustum, affine3 inverseViewMatrix, float maxShadowDistance, float lightSpaceZUp, float lightSpaceZDown, float exponent, float3 preViewTranslation, int numberOfCascades)
	{
		return false;
	}
	bool CascadedShadowMap::SetupForCubemapView(const ParallelLight& light, float3 center, float maxShadowDistance, float lightSpaceZUp, float lightSpaceZDown, float exponent, int numberOfCascades)
	{
		return false;
	}
	bool CascadedShadowMap::SetupPerObjectShadow(const ParallelLight& light, uint32_t object, const box3& objectBounds)
	{
		return false;
	}
	void CascadedShadowMap::SetupProxyViews()
	{
	}
	void CascadedShadowMap::Clear(ICommandList* commandList)
	{
	}
	void CascadedShadowMap::SetLitOutOfBounds(bool litOutOfBounds)
	{
	}
	void CascadedShadowMap::SetFalloffDistance(float distance)
	{
	}
	void CascadedShadowMap::SetNumberOfCascadesUnsafe(int cascades)
	{
	}
	std::shared_ptr<IView> CascadedShadowMap::GetCascadeView(uint32_t cascade)
	{
		return std::shared_ptr<IView>();
	}
	std::shared_ptr<IView> CascadedShadowMap::GetPerObjectView(uint32_t object)
	{
		return std::shared_ptr<IView>();
	}
	float4x4 CascadedShadowMap::GetWorldToUvzwMatrix() const
	{
		return float4x4();
	}
	const std::vector<SceneData*>& CascadedShadowMap::GetView() const
	{
		// TODO: 在此处插入 return 语句
		return m_CompositeView;
	}
	ITexture* CascadedShadowMap::GetTexture() const
	{
		return nullptr;
	}
	uint32_t CascadedShadowMap::GetNumberOfCascades() const
	{
		return 0;
	}
	const IShadowMap* CascadedShadowMap::GetCascade(uint32_t index) const
	{
		return nullptr;
	}
	uint32_t CascadedShadowMap::GetNumberOfPerObjectShadows() const
	{
		return 0;
	}
	const IShadowMap* CascadedShadowMap::GetPerObjectShadow(uint32_t index) const
	{
		return nullptr;
	}
	int2 CascadedShadowMap::GetTextureSize() const
	{
		return int2();
	}
	box2 CascadedShadowMap::GetUVRange() const
	{
		return box2();
	}
	float2 CascadedShadowMap::GetFadeRangeInTexels() const
	{
		return float2();
	}
	bool CascadedShadowMap::IsLitOutOfBounds() const
	{
		return false;
	}
	void CascadedShadowMap::FillShadowConstants(ShadowConstants& constants) const
	{
	}
}
