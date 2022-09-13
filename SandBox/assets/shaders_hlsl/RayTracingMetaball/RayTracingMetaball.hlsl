//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#ifndef RAYTRACING_HLSL
#define RAYTRACING_HLSL

#define HLSL

#include "../../shaders_hlsl_h/RaytracingHlslCompat.h"
//#include "RaytracingHelper.hlsli"
#include "ProceduralPrimitivesLibrary.hlsli"
//***************************************************************************
//*****------ Shader resources bound via root signatures -------*************
//***************************************************************************
RaytracingAccelerationStructure g_scene : register(t0, space0);
//对应 globalRootParameters[GlobalRootSignature::OutputView]
RWTexture2D<float4> g_renderTarget : register(u0);
//对应 ConstantBuffer<SceneConstantBuffer> m_sceneCB; //GlobalRootSignature::SceneConstant;
ConstantBuffer<SceneConstantBuffer> g_sceneCB : register(b0);


// Triangle resources
//对应 GlobalRootSignature::VertexBuffers
ByteAddressBuffer g_indices : register(t1, space0);
// ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2 /*numDescriptors*/, 1 /*baseShaderRegister*/); 
StructuredBuffer<Vertex> g_vertices : register(t2, space0);

// Procedural geometry resources
//对应 m_aabbPrimitiveAttributeBuffer
StructuredBuffer<PrimitiveInstancePerFrameBuffer> g_AABBPrimitiveAttributes : register(t3, space0);
ConstantBuffer<PrimitiveConstantBuffer> l_materialCB : register(b1);
ConstantBuffer<PrimitiveInstanceConstantBuffer> l_aabbCB: register(b2);

//***************************************************************************
//********************------ Other Structure. -------************************
//***************************************************************************






//***************************************************************************
//****************------ Utility functions -------***************************
//***************************************************************************

// Diffuse lighting calculation.
float CalculateDiffuseCoefficient(in float3 hitPosition, in float3 incidentLightRay, in float3 normal)
{
    float fNDotL = saturate(dot(-incidentLightRay, normal));
    return fNDotL;
}

// Phong lighting specular component
float4 CalculateSpecularCoefficient(in float3 hitPosition, in float3 incidentLightRay, in float3 normal, in float specularPower)
{
    float3 reflectedLightRay = normalize(reflect(incidentLightRay, normal));
    return pow(saturate(dot(reflectedLightRay, normalize(-WorldRayDirection()))), specularPower);
}


// Phong lighting model = ambient + diffuse + specular components.
float4 CalculatePhongLighting(in float4 albedo, in float3 normal, in bool isInShadow, in float diffuseCoef = 1.0, in float specularCoef = 1.0, in float specularPower = 50)
{
    float3 hitPosition = HitWorldPosition();
    float3 lightPosition = g_sceneCB.lightPosition.xyz;
    float shadowFactor = isInShadow ? InShadowRadiance : 1.0;
    float3 incidentLightRay = normalize(hitPosition - lightPosition);

    // Diffuse component.
    float4 lightDiffuseColor = g_sceneCB.lightDiffuseColor;
    float Kd = CalculateDiffuseCoefficient(hitPosition, incidentLightRay, normal);
    float4 diffuseColor = shadowFactor * diffuseCoef * Kd * lightDiffuseColor * albedo;

    // Specular component.
    float4 specularColor = float4(0, 0, 0, 0);
    if (!isInShadow)
    {
        float4 lightSpecularColor = float4(1, 1, 1, 1);
        float4 Ks = CalculateSpecularCoefficient(hitPosition, incidentLightRay, normal, specularPower);
        specularColor = specularCoef * Ks * lightSpecularColor;
    }

    // Ambient component.
    // Fake AO: Darken faces with normal facing downwards/away from the sky a little bit.
    float4 ambientColor = g_sceneCB.lightAmbientColor;
    float4 ambientColorMin = g_sceneCB.lightAmbientColor - 0.1;
    float4 ambientColorMax = g_sceneCB.lightAmbientColor;
    float a = 1 - saturate(dot(normal, float3(0, -1, 0)));
    ambientColor = albedo * lerp(ambientColorMin, ambientColorMax, a);

    return ambientColor + diffuseColor + specularColor;
}


//***************************************************************************
//*****------ TraceRay wrappers for radiance and shadow rays. -------********
//***************************************************************************
float4 TraceRadianceRay(inout Ray ray, int currentRecursionDepth) {
	if (currentRecursionDepth >= MAX_RAYTRACING_DEPTH) {
		return float4(0, 0, 0, 0);
	}
    RayDesc rayDesc;
    rayDesc.Origin = ray.origin;
    rayDesc.Direction = ray.direction;
    rayDesc.TMin = 0.001;
    rayDesc.TMax = 10000.0;

    RayPayload rayPayload = { float4(0,0,0,0),currentRecursionDepth + 1 };
    TraceRay(g_scene,
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES,
        TraceRayParameters::InstanceMask,
        TraceRayParameters::HitGroup::Offset[RayType::Radiance],
        TraceRayParameters::HitGroup::GeometryStride,
        TraceRayParameters::MissShader::Offset[RayType::Radiance],
        rayDesc, rayPayload);

    return rayPayload.color;

}
// Trace a shadow ray and return true if it hits any geometry.
bool TraceShadowRayAndReportIfHit(in Ray ray, in UINT currentRayRecursionDepth)
{
    if (currentRayRecursionDepth >= MAX_RAY_RECURSION_DEPTH)
    {
        return false;
    }

    // Set the ray's extents.
    RayDesc rayDesc;
    rayDesc.Origin = ray.origin;
    rayDesc.Direction = ray.direction;
    // Set TMin to a zero value to avoid aliasing artifcats along contact areas.
    // Note: make sure to enable back-face culling so as to avoid surface face fighting.
    rayDesc.TMin = 0;
    rayDesc.TMax = 10000;

    // Initialize shadow ray payload.
    // Set the initial value to true since closest and any hit shaders are skipped. 
    // Shadow miss shader, if called, will set it to false.
    ShadowRayPayload shadowPayload = { true };
    TraceRay(g_scene,
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES
        | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH
        | RAY_FLAG_FORCE_OPAQUE             // ~skip any hit shaders
        | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, // ~skip closest hit shaders,
        TraceRayParameters::InstanceMask,
        TraceRayParameters::HitGroup::Offset[RayType::Shadow],
        TraceRayParameters::HitGroup::GeometryStride,
        TraceRayParameters::MissShader::Offset[RayType::Shadow],
        rayDesc, shadowPayload);

    return shadowPayload.hit;
}
//***************************************************************************
//********************------ Ray gen shader.. -------************************
//***************************************************************************
[shader("raygeneration")]
void MyRaygenShader()
{
    Ray ray = GenerateCameraRay(DispatchRaysIndex().xy, g_sceneCB.cameraPosition.xyz, g_sceneCB.projectionToWorld);

    UINT currentRecursionDepth = 0;
    float4 color = TraceRadianceRay(ray, currentRecursionDepth);

    g_renderTarget[DispatchRaysIndex().xy] = color;

}

//***************************************************************************
//**********************------ Miss shaders -------**************************
//***************************************************************************
[shader("miss")]
void MyMissShader(inout RayPayload rayPayload)
{
    float4 backgroundColor = float4(BackgroundColor);
    rayPayload.color = backgroundColor;
}

[shader("miss")]
void MyMissShader_ShadowRay(inout ShadowRayPayload rayPayload)
{
    rayPayload.hit = false;
}
//***************************************************************************
//******************------ Closest hit shaders -------***********************
//***************************************************************************

[shader("closesthit")]
void MyClosestHitShader_Triangle(inout RayPayload rayPayload, in BuiltInTriangleIntersectionAttributes attr)
{
    // Get the base index of the triangle's first 16 bit index.
    uint indexSizeInBytes = 2;
    uint indicesPerTriangle = 3;
    uint triangleIndexStride = indicesPerTriangle * indexSizeInBytes;
    uint baseIndex = PrimitiveIndex() * triangleIndexStride;

    // Load up three 16 bit indices for the triangle.
    const uint3 indices = Load3x16BitIndices(baseIndex, g_indices);

    // Retrieve corresponding vertex normals for the triangle vertices.
    float3 triangleNormal = g_vertices[indices[0]].normal;

    // PERFORMANCE TIP: it is recommended to avoid values carry over across TraceRay() calls. 
    // Therefore, in cases like retrieving HitWorldPosition(), it is recomputed every time.

    // Shadow component.
    // Trace a shadow ray.
    float3 hitPosition = HitWorldPosition();
    Ray shadowRay = { hitPosition, normalize(g_sceneCB.lightPosition.xyz - hitPosition) };
    bool shadowRayHit = TraceShadowRayAndReportIfHit(shadowRay, rayPayload.recursionDepth);

    float checkers = AnalyticalCheckersTexture(HitWorldPosition(), triangleNormal, g_sceneCB.cameraPosition.xyz, g_sceneCB.projectionToWorld);

    // Reflected component.
    float4 reflectedColor = float4(0, 0, 0, 0);
    if (l_materialCB.reflectanceCoef > 0.001)
    {
        // Trace a reflection ray.
        Ray reflectionRay = { HitWorldPosition(), reflect(WorldRayDirection(), triangleNormal) };
        float4 reflectionColor = TraceRadianceRay(reflectionRay, rayPayload.recursionDepth);

        float3 fresnelR = FresnelReflectanceSchlick(WorldRayDirection(), triangleNormal, l_materialCB.albedo.xyz);
        reflectedColor = l_materialCB.reflectanceCoef * float4(fresnelR, 1) * reflectionColor;
    }

    // Calculate final color.
    float4 phongColor = CalculatePhongLighting(l_materialCB.albedo, triangleNormal, shadowRayHit, l_materialCB.diffuseCoef, l_materialCB.specularCoef, l_materialCB.specularPower);
    float4 color = checkers * (phongColor + reflectedColor);

    // Apply visibility falloff.
    float t = RayTCurrent();
    color = lerp(color, BackgroundColor, 1.0 - exp(-0.000002 * t * t * t));

    rayPayload.color = color;
}

[shader("closesthit")]
void MyClosestHitShader_AABB(inout RayPayload rayPayload, in ProceduralPrimitiveAttributes attr)
{
    // PERFORMANCE TIP: it is recommended to minimize values carry over across TraceRay() calls. 
    // Therefore, in cases like retrieving HitWorldPosition(), it is recomputed every time.

    // Shadow component.
    // Trace a shadow ray.
    float3 hitPosition = HitWorldPosition();
    Ray shadowRay = { hitPosition, normalize(g_sceneCB.lightPosition.xyz - hitPosition) };
    bool shadowRayHit = TraceShadowRayAndReportIfHit(shadowRay, rayPayload.recursionDepth);

    // Reflected component.
    float4 reflectedColor = float4(0, 0, 0, 0);
    if (l_materialCB.reflectanceCoef > 0.001)
    {
        // Trace a reflection ray.
        Ray reflectionRay = { HitWorldPosition(), reflect(WorldRayDirection(), attr.normal) };
        float4 reflectionColor = TraceRadianceRay(reflectionRay, rayPayload.recursionDepth);

        float3 fresnelR = FresnelReflectanceSchlick(WorldRayDirection(), attr.normal, l_materialCB.albedo.xyz);
        reflectedColor = l_materialCB.reflectanceCoef * float4(fresnelR, 1) * reflectionColor;
    }

    // Calculate final color.
    float4 phongColor = CalculatePhongLighting(l_materialCB.albedo, attr.normal, shadowRayHit, l_materialCB.diffuseCoef, l_materialCB.specularCoef, l_materialCB.specularPower);
    float4 color = phongColor + reflectedColor;

    // Apply visibility falloff.
    float t = RayTCurrent();
    color = lerp(color, BackgroundColor, 1.0 - exp(-0.000002 * t * t * t));

    rayPayload.color = color;
}






//***************************************************************************
//*****************------ Intersection shaders-------************************
//***************************************************************************
// Get ray in AABB's local space.
//把aabb空间转换到object空间
Ray GetRayInAABBPrimitiveLocalSpace()
{
    PrimitiveInstancePerFrameBuffer attr = g_AABBPrimitiveAttributes[l_aabbCB.instanceIndex];

    // Retrieve a ray origin position and direction in bottom level AS space 
    // and transform them into the AABB primitive's local space.
    Ray ray;
    ray.origin = mul(float4(ObjectRayOrigin(), 1), attr.bottomLevelASToLocalSpace).xyz;
    ray.direction = mul(ObjectRayDirection(), (float3x3) attr.bottomLevelASToLocalSpace);
    return ray;
}

[shader("intersection")]
void MyIntersectionShader_AnalyticPrimitive()
{
    //先从bottomLevelAABB空间转换到object空间，产生object空间的localRay
    Ray localRay = GetRayInAABBPrimitiveLocalSpace();
    AnalyticPrimitive::Enum primitiveType = (AnalyticPrimitive::Enum)l_aabbCB.primitiveType;

    float thit;
    ProceduralPrimitiveAttributes attr = (ProceduralPrimitiveAttributes)0;
    //在object空间判断是否有交集后，在转换回bottomLevelAABB空间，然后GPU执行ReportHit
    if (RayAnalyticGeometryIntersectionTest(localRay, primitiveType, thit, attr))
    {
        PrimitiveInstancePerFrameBuffer aabbAttribute = g_AABBPrimitiveAttributes[l_aabbCB.instanceIndex];
        attr.normal = mul(attr.normal, (float3x3) aabbAttribute.localSpaceToBottomLevelAS);
        attr.normal = normalize(mul((float3x3) ObjectToWorld3x4(), attr.normal));

        ReportHit(thit, /*hitKind*/ 0, attr);
    }
}

[shader("intersection")]
void MyIntersectionShader_VolumetricPrimitive()
{
    Ray localRay = GetRayInAABBPrimitiveLocalSpace();
    VolumetricPrimitive::Enum primitiveType = (VolumetricPrimitive::Enum)l_aabbCB.primitiveType;

    float thit;
    ProceduralPrimitiveAttributes attr = (ProceduralPrimitiveAttributes)0;
    if (RayVolumetricGeometryIntersectionTest(localRay, primitiveType, thit, attr, g_sceneCB.elapsedTime))
    {
        PrimitiveInstancePerFrameBuffer aabbAttribute = g_AABBPrimitiveAttributes[l_aabbCB.instanceIndex];
        attr.normal = mul(attr.normal, (float3x3) aabbAttribute.localSpaceToBottomLevelAS);
        attr.normal = normalize(mul((float3x3) ObjectToWorld3x4(), attr.normal));

        ReportHit(thit, /*hitKind*/ 0, attr);
    }
}

[shader("intersection")]
void MyIntersectionShader_SignedDistancePrimitive()
{
    Ray localRay = GetRayInAABBPrimitiveLocalSpace();
    SignedDistancePrimitive::Enum primitiveType = (SignedDistancePrimitive::Enum)l_aabbCB.primitiveType;

    float thit;
    ProceduralPrimitiveAttributes attr = (ProceduralPrimitiveAttributes)0;
    if (RaySignedDistancePrimitiveTest(localRay, primitiveType, thit, attr, l_materialCB.stepScale))
    {
        PrimitiveInstancePerFrameBuffer aabbAttribute = g_AABBPrimitiveAttributes[l_aabbCB.instanceIndex];
        attr.normal = mul(attr.normal, (float3x3) aabbAttribute.localSpaceToBottomLevelAS);
        attr.normal = normalize(mul((float3x3) ObjectToWorld3x4(), attr.normal));

        ReportHit(thit, /*hitKind*/ 0, attr);
    }
}
#endif