
#include <assets/shaders/glsl/common/CommonOctahedral.glsl>

#include <assets/shaders/glsl/common/CommonDepth.glsl>

#include <assets/shaders/glsl/bsdf/BSDF.glsl>

#include <material_cb.h>

// all values that are output by the forward rendering pass
struct GBufferData
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
	int ShadingModelID;
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
	return log2( L + LogBlackPoint ) / 16.0 + 0.5;
}

float DecodeIndirectIrradiance(float IndirectIrradiance)
{
	// LogL -> L
	float LogL = IndirectIrradiance;
	const float LogBlackPoint = 0.00390625;	// exp2(-8);
	return (1.0/g_View.preExposure) * (exp2( LogL * 16.0 - 8.0 ) - LogBlackPoint);	// 1 exp2, 1 smad, 1 ssub
}


void MobileFetchGBuffer(in float2 UV, inout half4 GBufferA, inout half4 GBufferB, inout half4 GBufferC, inout float SceneDepth)
{
#if DEFERRED_SHADING_PASS
#if VULKAN_PROFILE
	GBufferA = VulkanSubpassFetch1();
	GBufferB = VulkanSubpassFetch2();
	GBufferC = VulkanSubpassFetch3();

	SceneDepth = ConvertFromDeviceZ(VulkanSubpassDepthFetch(), g_View.zNear, g_View.zFar);
#elif METAL_PROFILE
	GBufferA = SubpassFetchRGBA_1();
	GBufferB = SubpassFetchRGBA_2();
	GBufferC = SubpassFetchRGBA_3();

	SceneDepth = ConvertFromDeviceZ(SubpassFetchR_4(), g_View.zNear, g_View.zFar);
#elif USE_GLES_FBF_DEFERRED
	GBufferA = GLSubpassFetch1();
	GBufferB = GLSubpassFetch2();
	GBufferC = GLSubpassFetch3();  // PLS is limited to 128bits
	SceneDepth = ConvertFromDeviceZ(DepthbufferFetchES2(), g_View.zNear, g_View.zFar);
#elif USE_GLES_PLS
    	GBufferA = pls.t_gGbufferA;
    	GBufferB = pls.t_gGbufferB;
    	GBufferC = pls.t_gGbufferC;  // PLS is limited to 128bits
    	//SceneDepth = ConvertFromDeviceZ(DepthbufferFetchES2(), g_View.zNear, g_View.zFar);

    	SceneDepth = gl_LastFragDepthARM;//ConvertFromDeviceZ(textureLod(t_gSceneDepth, UV, 0.0).r, g_View.zNear, g_View.zFar);
#else
//	GBufferA = textureLod(t_gGbufferA, UV, 0);
//	GBufferB = textureLod(t_gGbufferB, UV, 0);
//	GBufferC = textureLod(t_gGbufferC, UV, 0);
    GBufferA = texture(t_gGbufferA, UV);
	GBufferB = texture(t_gGbufferB, UV);
	GBufferC = texture(t_gGbufferC, UV);
	SceneDepth = texture(t_gSceneDepth, UV).r;//ConvertFromDeviceZ(texture(t_gSceneDepth, UV).r, g_View.zNear, g_View.zFar);

//	SceneDepth = ConvertFromDeviceZ(textureLod(t_gSceneDepth, UV, 0).r, g_View.zNear, g_View.zFar);
#endif

#endif //DEFERRED_SHADING_PASS
}



GBufferData MobileDecodeGBuffer(in half4 InGBufferA, in half4 InGBufferB, in half4 InGBufferC)
{
	GBufferData GBuffer;
	GBuffer.WorldNormal = OctahedronToUnitVector(InGBufferA.xy * 2.0f - 1.0f);
#if ALLOW_STATIC_LIGHTING
	GBuffer.IndirectIrradiance = DecodeIndirectIrradiance(InGBufferA.z);
#else
	GBuffer.IndirectIrradiance = 1.0;
#endif
	GBuffer.PerObjectGBufferData = InGBufferA.a;

	GBuffer.Metallic	= InGBufferB.r;
	GBuffer.Specular	= InGBufferB.g;
	GBuffer.Roughness	= InGBufferB.b;
	// Note: must match GetShadingModelId standalone function logic
	// Also Note: SimpleElementPixelShader directly sets SV_Target2 ( GBufferB ) to indicate unlit.
	// An update there will be required if this layout changes.
#if MOBILE_SHADINGMODEL_SUPPORT
    	GBuffer.ShadingModelID =  (round(InGBufferB.a * 255.0f));
#else
    	GBuffer.ShadingModelID =  (ShadingModel_DefaultLit);
    #endif
	//GBuffer.SelectiveOutputMask = 0;
	GBuffer.BaseColor = InGBufferC.rgb;//DecodeBaseColor(InGBufferC.rgb);
#if ALLOW_STATIC_LIGHTING
	GBuffer.AO = 1.0;
	//GBuffer.PrecomputedShadowFactors = half4(InGBufferC.a, 1, 1, 1);
#else
	GBuffer.AO = InGBufferC.a;
	//GBuffer.PrecomputedShadowFactors = 1.0;
#endif
    GBuffer.Anisotropy = 0.0;
	// derived from BaseColor, Metalness, Specular
	{
		GBuffer.SpecularColor = ComputeF0(GBuffer.Specular, GBuffer.BaseColor, GBuffer.Metallic);
		GBuffer.DiffuseColor = GBuffer.BaseColor - GBuffer.BaseColor * GBuffer.Metallic;
	}
	return GBuffer;
}



GBufferData MobileFetchAndDecodeGBuffer(in float2 UV)
{
	GBufferData GBuffer;
	float SceneDepth = 0.0; 
	half4 GBufferA = half4(0.0);
	half4 GBufferB = half4(0.0);
	half4 GBufferC = half4(0.0);
	MobileFetchGBuffer(UV, GBufferA, GBufferB, GBufferC, SceneDepth);
	GBuffer = MobileDecodeGBuffer(GBufferA, GBufferB, GBufferC);
	GBuffer.Depth = SceneDepth;
    GBuffer.WorldTangent = half3(0); //TODO:: get Aniso flag
   
	return GBuffer;
}


GBufferData DecodeGBuffer(in float2 texcoord)
{
    return MobileFetchAndDecodeGBuffer(texcoord);
}


/** Mobile specific encoding of GBuffer data */
void MobileEncodeGBuffer(
	in GBufferData GBuffer,
	inout half4 OutGBufferA,
	inout half4 OutGBufferB,
	inout half4 OutGBufferC
)
{
	if (GBuffer.ShadingModelID == ShadingModel_Unlit)
	{
		OutGBufferA = half4(0.0);
		OutGBufferB = half4(0.0);
		OutGBufferC = half4(0.0);
	}
	else
	{
		OutGBufferA.rg = UnitVectorToOctahedron(normalize(GBuffer.WorldNormal)) * 0.5f + 0.5f;
#if ALLOW_STATIC_LIGHTING
		OutGBufferA.b = EncodeIndirectIrradiance(GBuffer.IndirectIrradiance * GBuffer.AO);
#else
		OutGBufferA.b = 1.0;
#endif
		OutGBufferA.a = GBuffer.PerObjectGBufferData;		

		OutGBufferB.r = GBuffer.Metallic;
		OutGBufferB.g = GBuffer.Specular;
		OutGBufferB.b = GBuffer.Roughness;
		OutGBufferB.a = float(GBuffer.ShadingModelID) / 255.0f;

		OutGBufferC.rgb = EncodeBaseColor( GBuffer.BaseColor );
#if ALLOW_STATIC_LIGHTING
		OutGBufferC.a = GBuffer.AO;//todo::GBuffer.PrecomputedShadowFactors.x;
#else
		OutGBufferC.a = GBuffer.AO;
#endif
}
}


#if COOK
MaterialSample GetMaterialFromGBuffer(GBufferData GBuffer){
	MaterialSample mat;
	mat.shadingNormal = GBuffer.WorldNormal;
    mat.flags = 0; //not use
    mat.materialID = // not use
    mat.shadingModelID = GBuffer.ShadingModelID;
    mat.domain = MaterialDomain_Opaque; // Cook-torrance default to Opaque material
    mat.opacity = 1.0; // Cook-torrance default to Opaque material
    mat.alphaThreshold = 0.0; // not use, Gbuffer default to opacity /mask object
    mat.roughness = GBuffer.Roughness;
    mat.metallic = GBuffer.Metallic;
    mat.specular = GBuffer.Specular;
    mat.ao = GBuffer.AO;
    mat.albedo = GBuffer.BaseColor;
    mat.emissive =  half3(0.0);// 不在Gbuffer 处理, 在 SceneColor
    mat.transmission = half3(0.0);// not use,
    mat.ior = 0.0;//  not use,
    return mat;
}

GBufferData GetGBufferFormMatetial(SurfaceGeometry geom, MaterialSample mat){

	GBufferData GBuffer;

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
	GBuffer.Anisotropy = 0.0;
    GBuffer.PerObjectGBufferData = 1.0; //reserve channel
    GBuffer.Depth = 1.0; //not get from mat, use default temporary

	return GBuffer;
}


#elif (Disney)

DisneyMaterialSample GetMaterialFromGBuffer(GBufferData gbufferData){

    DisneyMaterialSample ret;
    //TODO::
    return ret;
}

GBufferData GetGBufferFormMatetial(SurfaceGeometry geom, DisneyMaterialSample mat){


}
#endif






