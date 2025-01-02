#include "pch.h"
#include "ShadowMap2D.h"

namespace BlackPearl {
	ShadowMap2D::ShadowMap2D(IDevice* device, int resolution, Format format)
	{
	}

	ShadowMap2D::ShadowMap2D(IDevice* device, ITexture* texture, uint32_t arraySlice, const RHIViewport& viewport)
	{
	}

	bool ShadowMap2D::SetupWholeSceneDirectionalLightView(const ParallelLight& light, box3_arg sceneBounds, float fadeRangeWorld)
	{
		return false;
	}

	bool ShadowMap2D::SetupDynamicDirectionalLightView(const ParallelLight& light, float3 anchor, float3 halfShadowBoxSize, float3 preViewTranslation, float fadeRangeWorld)
	{
		return false;
	}

	void ShadowMap2D::SetupProxyView()
	{
	}

	void ShadowMap2D::Clear(ICommandList* commandList)
	{
	}

	void ShadowMap2D::SetLitOutOfBounds(bool litOutOfBounds)
	{
	}

	void ShadowMap2D::SetFalloffDistance(float distance)
	{
	}

	std::shared_ptr<SceneData> ShadowMap2D::GetPlanarView()
	{
		return std::shared_ptr<SceneData>();
	}

	float4x4 ShadowMap2D::GetWorldToUvzwMatrix() const
	{
		return float4x4();
	}

	const std::vector<SceneData*>& ShadowMap2D::GetView() const
	{
		// TODO: 在此处插入 return 语句
		/*std::vector<std::shared_ptr<SceneData>> views;
		views.push_back(m_View);*/
		std::vector<SceneData*> views;
		views.push_back(m_View);
		return views;
	}

	ITexture* ShadowMap2D::GetTexture() const
	{
		return nullptr;
	}

	uint32_t ShadowMap2D::GetNumberOfCascades() const
	{
		return 0;
	}

	const IShadowMap* ShadowMap2D::GetCascade(uint32_t index) const
	{
		return nullptr;
	}

	uint32_t ShadowMap2D::GetNumberOfPerObjectShadows() const
	{
		return 0;
	}

	const IShadowMap* ShadowMap2D::GetPerObjectShadow(uint32_t index) const
	{
		return nullptr;
	}

	int2 ShadowMap2D::GetTextureSize() const
	{
		return int2();
	}

	box2 ShadowMap2D::GetUVRange() const
	{
		return box2();
	}

	float2 ShadowMap2D::GetFadeRangeInTexels() const
	{
		return float2();
	}

	bool ShadowMap2D::IsLitOutOfBounds() const
	{
		return false;
	}

	void ShadowMap2D::FillShadowConstants(ShadowConstants& constants) const
	{
	}

}
