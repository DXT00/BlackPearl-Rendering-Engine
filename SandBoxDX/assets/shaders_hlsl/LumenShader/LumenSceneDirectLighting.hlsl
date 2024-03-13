#pragma once
void LumenCardDirectLightingPS(
	FCardVSToPS CardInterpolants,
	out float4 OutColor : SV_Target0,
	float4 SvPosition : SV_Position)
{
	FShadowMaskRay ShadowMaskRay;
	ShadowMaskRay.bShadowFactorComplete = true;
	ShadowMaskRay.ShadowFactor = 1.0f;

	bool bValidTexel = true;
#if SHADOW_MASK
	{
		uint2 CoordInCardTile = SvPosition.xy % CARD_TILE_SIZE;
		ReadShadowMaskRay(CardInterpolants.CardTileIndex, CoordInCardTile, ShadowMaskRay);
	}
#else
	{
		float Opacity = Texture2DSampleLevel(LumenCardScene.OpacityAtlas, GlobalBilinearClampedSampler, CardInterpolants.AtlasUV, 0).x;
		bValidTexel = Opacity > 0.0f;
	}
#endif

	float3 Irradiance = 0.0f;

	if (bValidTexel && ShadowMaskRay.ShadowFactor > 0.0f)
	{
		FDeferredLightData LightData = LoadLightData(LIGHT_TYPE);

		float Depth = 1.0f - Texture2DSampleLevel(LumenCardScene.DepthAtlas, GlobalBilinearClampedSampler, CardInterpolants.AtlasUV, 0).x;

		FLumenCardPageData CardPage = GetLumenCardPageData(CardInterpolants.CardPageIndex);
		FLumenCardData Card = GetLumenCardData(CardPage.CardIndex);
		float3 WorldPosition = GetCardWorldPosition(Card, CardInterpolants.CardUV, Depth);
		float3 TranslatedWorldPosition = WorldPosition + LWCHackToFloat(PrimaryView.PreViewTranslation);

		float3 LightColor = DeferredLightUniforms.Color;
		float3 L = LightData.Direction;
		float3 ToLight = L;

#if LIGHT_TYPE == LIGHT_TYPE_DIRECTIONAL
		float CombinedAttenuation = 1;
#else
		float LightMask = 1;
		if (LightData.bRadialLight)
		{
			LightMask = GetLocalLightAttenuation(TranslatedWorldPosition, LightData, ToLight, L);
		}

		float Attenuation;

		if (LightData.bRectLight)
		{
			FRect Rect = GetRect(ToLight, LightData);
			FRectTexture RectTexture = InitRectTexture(DeferredLightUniforms.SourceTexture);
			Attenuation = IntegrateLight(Rect, RectTexture);
		}
		else
		{
			FCapsuleLight Capsule = GetCapsule(ToLight, LightData);
			Capsule.DistBiasSqr = 0;
			Attenuation = IntegrateLight(Capsule, LightData.bInverseSquared);
		}

		float CombinedAttenuation = Attenuation * LightMask;
#endif

		if (CombinedAttenuation > 0)
		{
			float3 WorldNormal = DecodeSurfaceCacheNormal(Card, Texture2DSampleLevel(LumenCardScene.NormalAtlas, GlobalBilinearClampedSampler, CardInterpolants.AtlasUV, 0).xy);

			if (dot(WorldNormal, L) > 0)
			{
#if LIGHT_FUNCTION
				ShadowMaskRay.ShadowFactor *= GetLightFunction(TranslatedWorldPosition);
#endif

#if USE_CLOUD_TRANSMITTANCE
				{
					float OutOpticalDepth = 0.0f;
					ShadowMaskRay.ShadowFactor *= lerp(1.0f, GetCloudVolumetricShadow(TranslatedWorldPosition, CloudShadowmapTranslatedWorldToLightClipMatrix, CloudShadowmapFarDepthKm, CloudShadowmapTexture, CloudShadowmapSampler, OutOpticalDepth), CloudShadowmapStrength);
				}
#endif

				if (UseIESProfile > 0)
				{
					ShadowMaskRay.ShadowFactor *= ComputeLightProfileMultiplier(TranslatedWorldPosition, DeferredLightUniforms.TranslatedWorldPosition, -DeferredLightUniforms.Direction, DeferredLightUniforms.Tangent);
				}

				float NoL = saturate(dot(WorldNormal, L));
				Irradiance = LightColor * (CombinedAttenuation * NoL * ShadowMaskRay.ShadowFactor);
				//Irradiance = bShadowFactorValid ? float3(0, 1, 0) : float3(0.2f, 0.0f, 0.0f);
			}
		}
	}

	OutColor = float4(Irradiance, 0);
}
