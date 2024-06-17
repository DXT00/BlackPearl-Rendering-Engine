struct ComputeShaderInput
{
	uint3 groupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
	uint3 groupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
	uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

float zNearVS;
RWStructuredBuffer<AABB> RWClusterAABBs;

[numthreads(1024, 1, 1)]
void ClusterAABBCore(ComputeShaderInput cs_IDs)
{
	// TODO: insert actual code here!
	uint clusterIndex1D = cs_IDs.dispatchThreadID.x;

	// Convert the 1D cluster index into a 3D index in the cluster grid.
	uint3 clusterIndex3D = ComputeClusterIndex3D(clusterIndex1D);

	Plane nearPlane;
	Plane farPlane;
	if (clusterIndex3D.z == 0)
	{
		nearPlane.N = float3(0.0f, 0.0f, -1.0f);  
		nearPlane.d = zNearVS;
		farPlane.N = float3(0.0f, 0.0f, -1.0f); 
		farPlane.d = ClusterCB_ViewNear;
	}
	else
	{
		nearPlane.N = float3(0.0f, 0.0f, -1.0f); 
		nearPlane.d = ClusterCB_ViewNear * pow(abs(ClusterCB_NearK), clusterIndex3D.z - 1);
		farPlane.N = float3(0.0f, 0.0f, -1.0f); 
		farPlane.d = ClusterCB_ViewNear * pow(abs(ClusterCB_NearK), clusterIndex3D.z);
	}
	float4 pMin = float4(clusterIndex3D.xy * ClusterCB_Size.xy, 0.0f, 1.0f);
	// The bottom-right point of cluster K in screen space.
	float4 pMax = float4((clusterIndex3D.xy + 1) * ClusterCB_Size.xy, 0.0f, 1.0f);

	pMin = ScreenToView(pMin);
	pMax = ScreenToView(pMax);

	//pMin.z *= -1;
	//pMax.z *= -1;

	float3 nearMin, nearMax, farMin, farMax;
	// Origin (camera eye position)
	float3 eye = float3(0, 0, 0);
	IntersectLinePlane(eye, (float3)pMin, nearPlane, nearMin);
	IntersectLinePlane(eye, (float3)pMax, nearPlane, nearMax);
	IntersectLinePlane(eye, (float3)pMin, farPlane, farMin);
	IntersectLinePlane(eye, (float3)pMax, farPlane, farMax);

	float3 aabbMin = min(nearMin, min(nearMax, min(farMin, farMax)));
	float3 aabbMax = max(nearMin, max(nearMax, max(farMin, farMax)));

	AABB aabb = { float4(aabbMin, 1.0f), float4(aabbMax, 1.0f) };

	RWClusterAABBs[clusterIndex1D] = aabb;

}

struct Plane
{
	float3 N;   // Plane normal.
	float  d;   // Distance to origin.
};

struct Sphere
{
	float3 c;   // Center point.
	float  r;   // Radius.
};

struct AABB
{
	float4 min;
	float4 max;
};

float4 ClipToView(float4 clip)
{
	// View space position.
	float4 view = mul(inverseProjection, clip);
	// Perspective projection.
	view = view / view.w;

	return view;
}

float3 WorldToView(float3 world)
{
	float3 view = mul(worldtoview, float4(world, 1.0f)).xyz;
	return view;
}

// Convert screen space coordinates to view space.
float4 ScreenToView(float4 screen)
{
	// Convert to normalized texture coordinates
	float2 texCoord = screen.xy * ClusterCB_ScreenDimensions.zw;

	// Convert to clip space
	float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

	return ClipToView(clip);
}


uint3 ComputeClusterIndex3D(uint clusterIndex1D)
{
	uint i = clusterIndex1D % ClusterCB_GridDim.x;
	uint j = clusterIndex1D % (ClusterCB_GridDim.x * ClusterCB_GridDim.y) / ClusterCB_GridDim.x;
	uint k = clusterIndex1D / (ClusterCB_GridDim.x * ClusterCB_GridDim.y);

	return uint3(i, j, k);
}

/**
 * Convert the 3D cluster index into a 1D cluster index.
 */
uint ComputeClusterIndex1D(uint3 clusterIndex3D)
{
	return clusterIndex3D.x + (ClusterCB_GridDim.x * (clusterIndex3D.y + ClusterCB_GridDim.y * clusterIndex3D.z));
}

bool IntersectLinePlane(float3 a, float3 b, Plane p, out float3 q)
{
	float3 ab = b - a;

	float t = (p.d - dot(p.N, a)) / dot(p.N, ab);

	bool intersect = (t >= 0.0f && t <= 1.0f);

	q = float3(0, 0, 0);
	if (intersect)
	{
		q = a + t * ab;
	}

	return intersect;
}