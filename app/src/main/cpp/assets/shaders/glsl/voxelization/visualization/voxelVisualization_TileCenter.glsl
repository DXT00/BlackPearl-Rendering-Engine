#type vertex
#version 450 core

//uniform mat4 V;

layout(location = 0) in vec3 aPos;

void main(){
	gl_Position = vec4(aPos, 1);
}


#type fragment
#version 450 core


#define INV_STEP_LENGTH (1.0f/STEP_LENGTH)
#define STEP_LENGTH 0.005f

#include <forward_cb.h>
#include <voxel_cb.h>
#include <voxelization/voxelCommon.glsl>

layout(std140, binding = 0) uniform ForwardShadingViewConstantsUBO {
   ForwardShadingViewConstants g_View;
};

layout(std140, binding = 8) uniform VoxelUBO {
    VoxelVisualConstants g_VoxelVisual;
};

layout(binding = 0) uniform sampler3D texture3D;


out vec4 color;





vec3 GetVoxelLocalPos(vec3 origin){
     float voxelMinX = g_VoxelVisual.center.x - g_VoxelVisual.areaExtent.x * 0.5;
     float voxelMinY = g_VoxelVisual.center.y - g_VoxelVisual.areaExtent.y * 0.5;
     float voxelMinZ = g_VoxelVisual.center.z - g_VoxelVisual.areaExtent.z * 0.5;

     return vec3(origin.x - voxelMinX,
                 origin.y - voxelMinY,
                 origin.z - voxelMinZ);

}
vec3 GenRay(){
	vec2 texcoord = vec2(gl_FragCoord.xy)/vec2(g_View.viewportSize.x, g_View.viewportSize.y);
	texcoord = 2.0*texcoord - 1.0;
    texcoord.y = 1.0 -texcoord.y;
    vec3 end = (inverse(g_View.matProjectionView) * vec4(texcoord, -1.0, 1)).xyz;
	return normalize(end - g_View.cameraPos);
}

struct Ray {
    vec3 origin;
    vec3 direction;
};

Ray generateRay(vec2 fragCoord, vec2 viewportSize, vec3 cameraPos, vec3 cameraForward, vec3 cameraRight, vec3 cameraUp, float fov) {
    // 将屏幕坐标转换到 NDC [-1,1]
    vec2 uv = fragCoord / viewportSize; //[0,1]
    uv.y = 1.0 - uv.y;
    vec2 ndc = (2.0 * uv ) - 1.0;

    // 计算光线方向（透视投影）
    float tanFov = tan(fov / 2.0);
    vec3 rayDir = normalize(
        cameraForward + 
        ndc.x * cameraRight * tanFov * (viewportSize.x / viewportSize.y) + 
        ndc.y * cameraUp * tanFov
    );

    // 返回光线
    return Ray(cameraPos, rayDir);
}

void main() {
	const int mipmapLevel = g_VoxelVisual.mipLevel;

	// Initialize ray.
	 vec3 origin = g_View.cameraPos;
     vec3 originInVoxel = (origin - g_VoxelVisual.center);// / g_VoxelVisual.areaExtent; //GetVoxelLocalPos(origin);



     Ray ray = generateRay(gl_FragCoord.xy, g_View.viewportSize,g_View.cameraPos, g_View.cameraFront, g_View.cameraRight, g_View.cameraUp, g_View.fov);

//	 vec2 textureCoordinateFrag= vec2(gl_FragCoord.xy)/vec2(u_ScreenWidth, u_ScreenHeight);
//	 textureCoordinateFrag = 2.0*textureCoordinateFrag - 1.0;
//
//
//	vec3 stop = normalize(u_CameraFront)*u_CubeSize.x +
//				normalize(u_CameraRight)*textureCoordinateFrag.x*u_CubeSize.x +
//				normalize(u_CameraUp)*textureCoordinateFrag.y*u_CubeSize.x;
    
    float maxExtend =  max(g_VoxelVisual.areaExtent.x, max(g_VoxelVisual.areaExtent.y, g_VoxelVisual.areaExtent.z));
	float rayLength = maxExtend;// length(stop-origin);
	vec3 direction = ray.direction;//GenRay();

     
    float maxDim = max(g_VoxelVisual.dimension.x , max(g_VoxelVisual.dimension.y,g_VoxelVisual.dimension.z));
	uint numberOfSteps = uint(maxDim);//每次走一个voxel， 最多走dimesion 次uint(rayLength/voxelSize);

    float voxelSize = rayLength / numberOfSteps;

	// Trace.
	color = vec4(0.0f);
	for(uint step_ = 0; step_ < numberOfSteps && color.a < 0.99f; ++step_) {

		vec3 currentPoint =  (originInVoxel + voxelSize * float(step_) * direction) / g_VoxelVisual.areaExtent; //[-1,1]
		if(!isInsideCube(currentPoint,0))break;
		vec4 currentSample = textureLod(texture3D, scaleAndBias(currentPoint), 0);///u_CubeSize.x
		//if(currentSample.a!=-1)//if voxel is not empty
		color += (1.0f - color.a) * currentSample;
	} 
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}