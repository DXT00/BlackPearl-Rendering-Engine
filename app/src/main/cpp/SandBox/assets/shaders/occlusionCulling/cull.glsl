#type compute
#version 430 core

layout(local_size_x = 1) in;

struct ObjInfo {
	vec2 mesh; //meshId = mesh.x; meshCnt = mesh.y
	vec3 boundingBox; // bbox.xyz = bbox.extend, 
	vec3 pos;
	
};


layout(std430, binding = 0) buffer OutCntBuffer {
	uint countBuffer[];
};

layout(std430, binding = 1) buffer OutDispatch {
	uvec3 dispatchObjsCnt;
};

layout(std430, binding = 2) buffer InObjsInfo {
	float objsInfo[];
};

uniform mat4 u_ProjectionView;
uniform sampler2D u_DeapthMap;
uniform float u_Far;
uniform float u_Near;
uniform float u_MaxLevel;
uniform vec2 u_ViewPortSize;


struct AABB{
	vec4 v[8];

};


struct MeshIdxInfo
{
	vec2 pos; //pos.x = mesh.startidx, pos.y = mesh.cnt (num of mesh in a obj)
	vec4 matId; //最大存储4种material， 每个mesh 一种的话，每个obj最多4个mesh
};


//layout(std430, binding = 3) buffer OutCntBuffer {
//	uvec4 countBuffer[];
//};
//
//layout(std430, binding = 3) buffer OutCntBuffer {
//	uvec4 countBuffer[];
//};

float Linearize(float near, float far, float depth) {
	float z = depth * 2.0 - 1.0;
	float res = (2.0 * near * far) / (far + near - z * (far - near));
	return (res - near) / (far - near);
}


AABB GetBound(ObjInfo ins, vec3 translate) {
	vec3 bound = ins.boundingBox;
	vec3 size = bound.xyz;
	vec3 pose = vec3(translate.x, translate.y, translate.z);

	AABB res;
	res.v[0] = vec4(pose + size * vec3( 0.5,  0.5,  0.5), 1.0);
	res.v[1] = vec4(pose + size * vec3( 0.5,  0.5, -0.5), 1.0);
	res.v[2] = vec4(pose + size * vec3(-0.5,  0.5, -0.5), 1.0);
	res.v[3] = vec4(pose + size * vec3(-0.5,  0.5,  0.5), 1.0);
	res.v[4] = vec4(pose + size * vec3( 0.5, -0.5,  0.5), 1.0);
	res.v[5] = vec4(pose + size * vec3( 0.5, -0.5, -0.5), 1.0);
	res.v[6] = vec4(pose + size * vec3(-0.5, -0.5, -0.5), 1.0);
	res.v[7] = vec4(pose + size * vec3(-0.5, -0.5,  0.5), 1.0);
	return res;
}
bool CheckCull(vec4 cp0, vec4 cp1, vec4 cp2, vec4 cp3, 
			vec4 cp4, vec4 cp5, vec4 cp6, vec4 cp7) {
	if(cp0.w < 0.001 || cp1.w < 0.001 || cp2.w < 0.001 || cp3.w < 0.001 || 
		cp4.w < 0.001 || cp5.w < 0.001 || cp6.w < 0.001 || cp7.w < 0.001) return false;

	vec3 p0 = cp0.xyz / cp0.w, p1 = cp1.xyz / cp1.w, p2 = cp2.xyz / cp2.w, p3 = cp3.xyz / cp3.w;
	vec3 p4 = cp4.xyz / cp4.w, p5 = cp5.xyz / cp5.w, p6 = cp6.xyz / cp6.w, p7 = cp7.xyz / cp7.w;

	float thr = 1.0, thz = 1.0;
	vec3 maxP = max(p7, max(p6, max(p5, max(p4, max(p3, max(p2, max(p0, p1)))))));		
	vec3 minP = min(p7, min(p6, min(p5, min(p4, min(p3, min(p2, min(p0, p1)))))));	
	return any(greaterThan(minP, vec3(thr, thr, thz))) || any(lessThan(maxP, vec3(-thr, -thr, -thz)));
}

bool FrustumCull(AABB ins) {
	vec4 p0 = u_ProjectionView * ins.v[0]; 
	vec4 p1 = u_ProjectionView * ins.v[1];
	vec4 p2 = u_ProjectionView * ins.v[2]; 
	vec4 p3 = u_ProjectionView * ins.v[3]; 
	vec4 p4 = u_ProjectionView * ins.v[4]; 
	vec4 p5 = u_ProjectionView * ins.v[5]; 
	vec4 p6 = u_ProjectionView * ins.v[6]; 
	vec4 p7 = u_ProjectionView * ins.v[7];

	return CheckCull(p0, p1, p2, p3, p4, p5, p6, p7);
}



bool HizQuery(mat4 viewProjectMat, sampler2D depthTex, float uFar, float uNear, float uMipLevel, vec2 viewportSize,
	vec4 bv0, vec4 bv1, vec4 bv2, vec4 bv3, vec4 bv4, vec4 bv5, vec4 bv6, vec4 bv7, float zOffset) {
		vec4 b0 = viewProjectMat * bv0;
		vec4 b1 = viewProjectMat * bv1;
		vec4 b2 = viewProjectMat * bv2;
		vec4 b3 = viewProjectMat * bv3;
		vec4 b4 = viewProjectMat * bv4;
		vec4 b5 = viewProjectMat * bv5;
		vec4 b6 = viewProjectMat * bv6;
		vec4 b7 = viewProjectMat * bv7;
		vec4 inv1 = 1.0 / vec4(b0.w, b1.w, b2.w, b3.w);
		vec4 inv2 = 1.0 / vec4(b4.w, b5.w, b6.w, b7.w);
		
		vec3 ndc0 = b0.xyz * inv1.x;
		vec3 ndc1 = b1.xyz * inv1.y;
		vec3 ndc2 = b2.xyz * inv1.z;
		vec3 ndc3 = b3.xyz * inv1.w;
		vec3 ndc4 = b4.xyz * inv2.x;
		vec3 ndc5 = b5.xyz * inv2.y;
		vec3 ndc6 = b6.xyz * inv2.z;
		vec3 ndc7 = b7.xyz * inv2.w;

		vec3 maxNDC = max(ndc7, max(ndc6, max(ndc5, max(ndc4, max(ndc3, max(ndc2, max(ndc0, ndc1)))))));
		vec3 minNDC = min(ndc7, min(ndc6, min(ndc5, min(ndc4, min(ndc3, min(ndc2, min(ndc0, ndc1)))))));
		//转换到 [0，1]
		vec3 maxClip = clamp(maxNDC * 0.5 + 0.5, vec3(0.0), vec3(1.0));
		vec3 minClip = clamp(minNDC * 0.5 + 0.5, vec3(0.0), vec3(1.0));

		vec3 bound = maxClip - minClip;
		float edge = max(1.0, max(bound.x, bound.y) * max(viewportSize.x, viewportSize.y));
		float mip = min(ceil(log2(edge)), uMipLevel);
//		
		vec4 occ = vec4(textureLod(depthTex, maxClip.xy, mip).x, 
						textureLod(depthTex, minClip.xy, mip).x, 
						textureLod(depthTex, vec2(maxClip.x, minClip.y), mip).x, 
						textureLod(depthTex, vec2(minClip.x, maxClip.y), mip).x);
//		vec4 occ = vec4(texture(depthTex, maxClip.xy).x, 
//						texture(depthTex, minClip.xy).x, 
//						texture(depthTex, vec2(maxClip.x, minClip.y)).x, 
//						texture(depthTex, vec2(minClip.x, maxClip.y)).x);
//		float occ0 = texture(depthTex, maxClip.xyz).x;
//		float occ1 = texture(depthTex, minClip.xyz).x;
//		float occ2 = texture(depthTex, vec3(maxClip.x, minClip.y, minClip.z)).x; 
//		float occ3 = texture(depthTex, vec3(minClip.x, minClip.y, maxClip.z)).x;
//		float occ4 = texture(depthTex, vec3(maxClip.x, minClip.y, maxClip.z)).x;
//		float occ5 = texture(depthTex, vec3(minClip.x, maxClip.y, maxClip.z)).x;
//		float occ6 = texture(depthTex, vec3(minClip.x, maxClip.y, minClip.z)).x;
//		float occ7 = texture(depthTex, vec3(maxClip.x, maxClip.y, maxClip.z)).x;

		//float occDepth = max(occ7, max(occ6, max(occ5, max(occ4, max(occ3, max(occ2, max(occ0, occ1)))))));

		float occDepth = max(occ.w, max(occ.z, max(occ.x, occ.y)));

		occDepth = Linearize(uNear, uFar, occDepth);	
		minClip.z = Linearize(uNear, uFar, minClip.z);

		return minClip.z > occDepth + zOffset;
}
bool HizIsOccluded(AABB ins) {
//	if (uShadowPass > 0) return false;
//	else {
		return HizQuery(u_ProjectionView, u_DeapthMap, u_Far, u_Near, u_MaxLevel, u_ViewPortSize,
			ins.v[0], ins.v[1], ins.v[2], ins.v[3], ins.v[4], ins.v[5], ins.v[6], ins.v[7], 0.0001);
	//}
}


void main() {
	uint objId = gl_GlobalInvocationID.x;
	ObjInfo obj;
	obj.mesh = glm::vec2( objsInfo[objId*8], objsInfo[objId*8+1]);
	obj.boundingBox = vec3(objsInfo[objId*8+2],objsInfo[objId*8+3],objsInfo[objId*8+4]);
	obj.pos = vec3(objsInfo[objId*8+5],objsInfo[objId*8+6],objsInfo[objId*8+7]);


	uint startMeshId = uint(objsInfo[objId*8]);// uint(obj.mesh.x);
	uint meshCnt =  uint(objsInfo[objId*8+1]);//uint(obj.mesh.y);

	AABB bound = GetBound(obj, obj.pos);
	//atomicAdd(countBuffer[startMeshId], 1);
	if(!FrustumCull(bound) && !HizIsOccluded(bound))
	{
		for(uint i = startMeshId; i < startMeshId + meshCnt; i++){
			atomicAdd(countBuffer[i], 1);
		}
		atomicAdd(dispatchObjsCnt.x,1);
	}


}