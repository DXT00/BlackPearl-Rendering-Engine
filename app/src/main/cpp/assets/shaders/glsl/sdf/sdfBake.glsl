#type compute
#version 450 core

layout (local_size_x = 8,local_size_y = 8,local_size_z = 8) in;

#include <sdf_cb.h>
layout(binding = 0, r16f) uniform writeonly image3D t_DistanceFieldTexture;

layout(std140, binding = 8) uniform GDFUbo {
    GlobalDistanceFieldConstants g_Gdf;
};


layout(std430, binding = 9) buffer ObjsUbo {
	DFObjectConstants g_objsInfo[];
};

//
//struct ObjInfo {
//	vec2 mesh; //meshId = mesh.x; meshCnt = mesh.y
//	vec3 boundingBox; // bbox.xyz = bbox.extend, 
//	vec3 pos;
//};
//
//

// 计算点到场景中最近物体的距离（简化版：仅处理球体）
float SceneSDF(vec3 p) {
    // 示例：两个球体的联合 SDF
    float sphere1 = length(p - vec3(0, 0, 0)) - 5.0;
    float sphere2 = length(p - vec3(8, 0, 0)) - 3.0;
    return min(sphere1, sphere2);
}

// 使用 object 的 bounding box
float boxSDF(vec3 p, vec3 center, vec3 halfExtent) {
    vec3 d = abs(p - center) - halfExtent;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

void main(){
	ivec3 voxelCoord = ivec3(gl_GlobalInvocationID.xyz);
    vec3 worldPos = g_Gdf.clipmapCenter + (vec3(voxelCoord) - g_Gdf.clipmapDimension *0.5) * g_Gdf.voxelSize; // 64x64x64 纹理中心对齐
    
//    ObjInfo obj;
//	obj.mesh = glm::vec2( objsInfo[objId*8], objsInfo[objId*8+1]);
//	obj.boundingBox = vec3(objsInfo[objId*8+2],objsInfo[objId*8+3],objsInfo[objId*8+4]);
//	obj.pos = vec3(objsInfo[objId*8+5],objsInfo[objId*8+6],objsInfo[objId*8+7]);

    float d = 1e9;
    for(int i = 0; i< g_Gdf.objsCnt; i++)
    {
      DFObjectConstants obj = g_objsInfo[i];
      float distance = boxSDF(worldPos, obj.center, obj.extend);
      distance = min(d, distance);
      imageStore(t_DistanceFieldTexture, voxelCoord, vec4(distance, 0.0, 0.0, 0.0));

    }
  


}


//
//[numthreads(CULLOBJECTS_THREADGROUP_SIZE, 1, 1)]
//void CullObjectsToClipmapCS(
//	uint GroupIndex : SV_GroupIndex,
//	uint3 GroupId : SV_GroupID)
//{
//	const FLWCVector3 PreViewTranslation = MakeLWCVector3(-ViewTilePosition, RelativePreViewTranslation);
//
//	const uint ObjectIndex = GetUnWrappedDispatchThreadId(GroupId, GroupIndex, CULLOBJECTS_THREADGROUP_SIZE);
//	if (ObjectIndex < NumSceneObjects)
//	{
//		const FDFObjectBounds DFObjectBounds = LoadDFObjectBounds(ObjectIndex);
//		const float3 TranslatedBoundsCenter = LWCToFloat(LWCAdd(DFObjectBounds.Center, PreViewTranslation));
//
//		BRANCH
//		if ((DFObjectBounds.bVisible || DFObjectBounds.bAffectIndirectLightingWhileHidden)
//			&& (DFObjectBounds.SphereRadius > MeshSDFRadiusThreshold || DFObjectBounds.bEmissiveLightSource)
//			&& (AcceptOftenMovingObjectsOnly == 2 || DFObjectBounds.OftenMoving == AcceptOftenMovingObjectsOnly))
//		{
//			float DistanceSq = ComputeSquaredDistanceBetweenAABBs(ClipmapTranslatedWorldCenter, ClipmapWorldExtent, TranslatedBoundsCenter, DFObjectBounds.BoxExtent);
//			if (DistanceSq < InfluenceRadiusSq)
//			{
//				uint DestIndex;
//				InterlockedAdd(RWObjectIndexNumBuffer[0], 1U, DestIndex);
//				RWObjectIndexBuffer[DestIndex] = ObjectIndex;
//
//#if READBACK_HAS_PENDING_STREAMING
//				FDFObjectData DFObjectData = LoadDFObjectData(ObjectIndex);
//				uint NumMips = LoadDFAssetData(DFObjectData.AssetIndex, 0).NumMips;
//
//				if (NumMips == 1)
//				{
//					RWHasPendingStreaming[0] = 1;
//				}
//#endif
//			}
//		}
//	}
//}