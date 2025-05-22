
#include <assets/glsl/common/CommonOctahedral.glsl>



// all values that are output by the forward rendering pass
struct FGBufferData
{
	// normalized
	half3 WorldNormal;
	// normalized, only valid if HAS_ANISOTROPY_MASK in SelectiveOutputMask
	half3 WorldTangent;
	// 0..1 (derived from BaseColor, Metalness, Specular)
	half3 DiffuseColor;
	// 0..1 (derived from BaseColor, Metalness, Specular)
	half3 SpecularColor;
	// 0..1, white for SHADINGMODELID_SUBSURFACE_PROFILE and SHADINGMODELID_EYE (apply BaseColor after scattering is more correct and less blurry)
	half3 BaseColor;
	// 0..1
	half Metallic;
	// 0..1
	half Specular;
	// AO utility value
	//half GenericAO;
	// Indirect irradiance luma, (light probe lumance)
	half IndirectIrradiance;
	// Static shadow factors for channels assigned by Lightmass
	// Lights using static shadowing will pick up the appropriate channel in their deferred pass
	//half4 PrecomputedShadowFactors;
	// 0..1
	half Roughness;
	// -1..1, only valid if only valid if HAS_ANISOTROPY_MASK in SelectiveOutputMask
	half Anisotropy;
	// 0..1 ambient occlusion  e.g.SSAO, wet surface mask, skylight mask, ...
	half AO;
	// Bit mask for occlusion of the diffuse indirect samples
	//uint DiffuseIndirectSampleOcclusion;
	// 0..255 
	uint ShadingModelID;
    // in world space (linear value), can be used to reconstruct world position,
	// only valid when decoding the GBuffer as the value gets reconstructed from the Z buffer
	half Depth;
	// use for objectID or other attributes in Object
	half PerObjectGBufferData;
	
};

float3 EncodeBaseColor(float3 BaseColor)
{
	// we use sRGB on the render target to give more precision to the darks
	return BaseColor;
}

float3 DecodeBaseColor(float3 BaseColor)
{
	// we use sRGB on the render target to give more precision to the darks
	return BaseColor;
}

float EncodeIndirectIrradiance(float IndirectIrradiance)
{
	float L = IndirectIrradiance;
	L *= g_View.preExposure; // Apply pre-exposure as a mean to prevent compression overflow.
	const float LogBlackPoint = 0.00390625;	// exp2(-8);
	return log2( L + LogBlackPoint ) / 16 + 0.5;
}

float DecodeIndirectIrradiance(float IndirectIrradiance)
{
	// LogL -> L
	float LogL = IndirectIrradiance;
	const float LogBlackPoint = 0.00390625;	// exp2(-8);
	return (1.0/g_View.preExposure) * (exp2( LogL * 16 - 8 ) - LogBlackPoint);	// 1 exp2, 1 smad, 1 ssub
}

void MobileFetchGBuffer(in float2 UV, out half4 GBufferA, out half4 GBufferB, out half4 GBufferC, out float SceneDepth)
{

#if VULKAN_PROFILE
	GBufferA = VulkanSubpassFetch1(); 
	GBufferB = VulkanSubpassFetch2(); 
	GBufferC = VulkanSubpassFetch3();

	SceneDepth = ConvertFromDeviceZ(VulkanSubpassDepthFetch());
#elif METAL_PROFILE
	GBufferA = SubpassFetchRGBA_1(); 
	GBufferB = SubpassFetchRGBA_2(); 
	GBufferC = SubpassFetchRGBA_3(); 

	SceneDepth = ConvertFromDeviceZ(SubpassFetchR_4());
#elif USE_GLES_FBF_DEFERRED
	GBufferA = GLSubpassFetch1(); 
	GBufferB = GLSubpassFetch2(); 
	GBufferC = GLSubpassFetch3();  // PLS is limited to 128bits
	SceneDepth = ConvertFromDeviceZ(DepthbufferFetchES2());
#else
	GBufferA = textureLod(t_gGbufferA, UV, 0); 
	GBufferB = textureLod(t_gGbufferB, UV, 0);
	GBufferC = textureLod(t_gGbufferC, UV, 0);

	SceneDepth = ConvertFromDeviceZ(textureLod(t_gSceneDepth, UV, 0).r);
#endif


}
#endif //MOBILE_DEFERRED_SHADING

FGBufferData MobileFetchAndDecodeGBuffer(in float2 UV)
{
	FGBufferData GBuffer = (FGBufferData)0;
	float SceneDepth = 0; 
	half4 GBufferA = 0;
	half4 GBufferB = 0;
	half4 GBufferC = 0;
	MobileFetchGBuffer(UV, GBufferA, GBufferB, GBufferC, SceneDepth);
	GBuffer = MobileDecodeGBuffer(GBufferA, GBufferB, GBufferC, GBufferD);
	GBuffer.Depth = SceneDepth;
    GBuffer.WorldTangent = half3(0); //TODO:: get Aniso flag
   
	return GBuffer;
}
#endif //SHADING_PATH_MOBILE


GbufferData MobileDecodeGBuffer(half4 InGBufferA, half4 InGBufferB, half4 InGBufferC)
{
	GbufferData GBuffer = (GbufferData)0;
	GBuffer.WorldNormal = OctahedronToUnitVector(InGBufferA.xy * 2.0f - 1.0f);
#if ALLOW_STATIC_LIGHTING
	GBuffer.IndirectIrradiance = DecodeIndirectIrradiance(InGBufferA.z);
#else
	GBuffer.IndirectIrradiance = 1;
#endif
	GBuffer.PerObjectGBufferData = InGBufferA.a;

	GBuffer.Metallic	= InGBufferB.r;
	GBuffer.Specular	= InGBufferB.g;
	GBuffer.Roughness	= InGBufferB.b;
	// Note: must match GetShadingModelId standalone function logic
	// Also Note: SimpleElementPixelShader directly sets SV_Target2 ( GBufferB ) to indicate unlit.
	// An update there will be required if this layout changes.
	GBuffer.ShadingModelID = MOBILE_SHADINGMODEL_SUPPORT ? (uint)round(InGBufferB.a * 255.0f) : SHADINGMODELID_DEFAULT_LIT;
	//GBuffer.SelectiveOutputMask = 0;
	GBuffer.BaseColor = DecodeBaseColor(InGBufferC.rgb);
#if ALLOW_STATIC_LIGHTING
	GBuffer.GBufferAO = 1;
	//GBuffer.PrecomputedShadowFactors = half4(InGBufferC.a, 1, 1, 1);
#else
	GBuffer.GBufferAO = InGBufferC.a;
	//GBuffer.PrecomputedShadowFactors = 1.0;
#endif

	// derived from BaseColor, Metalness, Specular
	{
		GBuffer.SpecularColor = ComputeF0(GBuffer.Specular, GBuffer.BaseColor, GBuffer.Metallic);
		GBuffer.DiffuseColor = GBuffer.BaseColor - GBuffer.BaseColor * GBuffer.Metallic;
	}
	return GBuffer;
}


GbufferData DecodeGBuffer(vec2 texcoord)
{
    return MobileFetchAndDecodeGBuffer(texcoord);
}


/** Mobile specific encoding of GBuffer data */
void MobileEncodeGBuffer(
	GbufferData GBuffer,
	out half4 OutGBufferA,
	out half4 OutGBufferB,
	out half4 OutGBufferC
)
{
	if (GBuffer.ShadingModelID == ShadingModel_Unlit)
	{
		OutGBufferA = 0;
		OutGBufferB = 0;
		OutGBufferC = 0;
	}
	else
	{
		OutGBufferA.rg = UnitVectorToOctahedron(normalize(GBuffer.WorldNormal)) * 0.5f + 0.5f;
#if ALLOW_STATIC_LIGHTING
		OutGBufferA.b = EncodeIndirectIrradiance(GBuffer.IndirectIrradiance * GBuffer.GBufferAO);
#else
		OutGBufferA.b = 1;
#endif
		OutGBufferA.a = GBuffer.PerObjectGBufferData;		

		OutGBufferB.r = GBuffer.Metallic;
		OutGBufferB.g = GBuffer.Specular;
		OutGBufferB.b = GBuffer.Roughness;
		OutGBufferB.a = GBuffer.ShadingModelID / 255.0;

		OutGBufferC.rgb = EncodeBaseColor( GBuffer.BaseColor );
#if ALLOW_STATIC_LIGHTING
		OutGBufferC.a = GBuffer.PrecomputedShadowFactors.x;
#else
		OutGBufferC.a = GBuffer.GBufferAO;
#endif
}
}


#ifdef COOK
MaterialSample GetMaterialFromGBuffer(GbufferData GBuffer){
	MaterialSample mat;
	mat.shadingNormal = GBuffer.WorldNormal;
    mat.flags = 0; //not use
    mat.materialID = // not use
    mat.shadingModelID = GBuffer.ShadingModelID;
    mat.domain = MaterialDomain_Opaque; // Cook-torrance default to Opaque material
    mat.opacity = 1; // Cook-torrance default to Opaque material
    mat.alphaThreshold = 0; // not use, Gbuffer default to opacity /mask object
    mat.roughness = GBuffer.Roughness;
    mat.metalness = GBuffer.Metallic;
    mat.specular = GBuffer.Specular
    mat.ao = GBuffer.AO;
    mat.albedo = GBuffer.BaseColor;
    mat.emissive = 0;// 不在Gbuffer 处理, 在 SceneColor
    mat.transmission = 0;// not use,
    mat.ior = 0;//  not use,

}

GbufferData GetGBufferFormMatetial(SurfaceGeometry geom, MaterialSample mat){

	GbufferData GBuffer;

    GBuffer.WorldNormal = mat.shadingNormal;
	GBuffer.WorldTangent = geom.tangent;
	GBuffer.BaseColor  = mat.albedo;
	GBuffer.SpecularColor  = mix(vec3(mat.specular), mat.albedo, mat.metallic);
	GBuffer.Metallic  = mat.metallic;
	GBuffer.Specular  = mat.specular;

#if NONMETAL
	GBuffer.DiffuseColor = GBuffer.BaseColor;
	GBuffer.SpecularColor = vec3(0.04);
#else
	GBuffer.SpecularColor = ComputeF0(GBuffer.Specular, GBuffer.BaseColor, GBuffer.Metallic);
	GBuffer.DiffuseColor = GBuffer.BaseColor - GBuffer.BaseColor * GBuffer.Metallic;
#endif
	GBuffer.Roughness = mat.roughness;
	GBuffer.ShadingModelID =  mat.shadingModelID;
	GBuffer.AO = mat.ao;
	GBuffer.Anisotropy = 0;
	return GBuffer;
}


#elif defined(Disney)

DisneyMaterialSample GetMaterialFromGBuffer(GbufferData gbufferData){


}

GbufferData GetGBufferFormMatetial(SurfaceGeometry geom, DisneyMaterialSample mat){


}
#endif

#endif

SurfaceGeometry GetGeometryFromGBuffer(GbufferData gbufferData){


}




