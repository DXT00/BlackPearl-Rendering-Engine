#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aNormal) in vec3 aNormal;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;


void main()
{
	v_TexCoord = aTexCoords;
	gl_Position = vec4(aPos,1.0);
}


#type fragment
#version 450 core


#ifdef GL_ES
precision highp float;  // 必须声明精度（ES 要求）
#endif


layout(binding = 0, r16f) uniform readonly image3D sdfClip0;  
layout(binding = 1, r16f) uniform readonly image3D sdfClip1;  
layout(binding = 2, r16f) uniform readonly image3D sdfClip2;  

//uniform vec3 uClipmapScale[3]; // 各层clipmap的尺度(世界空间到纹理空间的转换)
//uniform vec3 uClipmapOffset[3]; // 各层clipmap的偏移
//uniform float uClipmapCellSize[3]; // 各层clipmap的体素大小(世界单位)
//uniform vec2 iResolution;
//uniform float iTime;
//uniform int uVisualizationMode; // 0=表面, 1=clipmap层级, 2=距离场值
out vec4 FragColor;

#include <sdf_cb.h>
#include <common/CommonViewStruct.glsl>


layout(binding = 8, std140) uniform GDFUbo
{
    //GlobalSurfaceAtlasData data;
    GlobalSDFTraceConstants g_sdfData;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct ClipmapSample {
    float distance;
    int level;
};

// 从clipmap层级采样SDF
ClipmapSample sampleClipmap(vec3 worldPos) {
    // 确定使用哪个clipmap层级(这里简单根据y高度选择), todo:: 默认3层 clip, 不是3层要做筛选
    int level = 0;
    if (worldPos.y > g_View.cameraPos.y + g_sdfData.clipPosDistance[0].w || worldPos.y <  g_View.cameraPos.y -g_sdfData.clipPosDistance[0].w) level = 1;
    if (worldPos.y > g_View.cameraPos.y + g_sdfData.clipPosDistance[1].w || worldPos.y <  g_View.cameraPos.y -g_sdfData.clipPosDistance[1].w) level = 2;
    

    float3 center = g_sdfData.clipPosDistance[level].xyz;
    float extend = g_sdfData.clipPosDistance[level].w * 2.0;
    float voxelSize = g_sdfData.clipVoxelSize[level];
    // 计算纹理坐标
    vec3 voxelCoord = ((worldPos - center) / voxelSize) + g_sdfData.dimension * 0.5; //[0, dimesion]
    vec3 texCoord = voxelCoord/g_sdfData.dimension; //[0,1]
    // 采样纹理
    float distance = 0.0;
    if (level == 0) {
        distance = imageLoad(sdfClip0, ivec3(voxelCoord)).r;
    } else if (level == 1) {
        distance = imageLoad(sdfClip1, ivec3(voxelCoord)).r;
    } else {
        distance = imageLoad(sdfClip2, ivec3(voxelCoord)).r;
    }
    //distance 解码
    distance = distance* 2.0 -1.0; //[-1,1]
    distance = distance * g_sdfData.dimension * voxelSize;

    // 返回采样结果
    return ClipmapSample(distance, level);
}

// 场景SDF组合
float sceneSDF(vec3 p) {
    ClipmapSample sam = sampleClipmap(p);
    return sam.distance;
}

// 计算法线(使用中心差分)
vec3 calcNormal(vec3 p) {
    const float eps = 0.001;
    return normalize(vec3(
        sceneSDF(vec3(p.x + eps, p.y, p.z)) - sceneSDF(vec3(p.x - eps, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + eps, p.z)) - sceneSDF(vec3(p.x, p.y - eps, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z + eps)) - sceneSDF(vec3(p.x, p.y, p.z - eps))
    ));
}

// Ray marching
float rayMarch(Ray ray, float maxDist, int maxSteps) {
    float t = 0.0;
    for (int i = 0; i < maxSteps; i++) {
        vec3 p = ray.origin + ray.direction * t;
        float d = sceneSDF(p);
        if (abs(d) < 0.1 || t > maxDist) break;
        //if (abs(d) < 0.001 * t || t > maxDist) break;

        t += d;
    }
    return t;
}

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

    return Ray(cameraPos, rayDir);
}


vec3 shade(vec3 p, vec3 rd, ClipmapSample sam) {
//    if (uVisualizationMode == 0) {
//        // 表面渲染
//        vec3 normal = calcNormal(p);
//        vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0));
//        float diff = max(dot(normal, lightDir), 0.2);
//        return vec3(diff);
//    } 
//    else if (uVisualizationMode == 1) {
//        
//        if (sam.level == 0) return vec3(1.0, 0.0, 0.0); // 红: 最精细层
//        if (sam.level == 1) return vec3(0.0, 1.0, 0.0); // 绿: 中间层
//        return vec3(0.0, 0.0, 1.0); // 蓝: 最粗糙层
//    }
//    else {
        // 模式2: 显示距离场值

        

        float voxelSize = g_sdfData.clipVoxelSize[sam.level];

        float d = sam.distance / (g_sdfData.dimension * voxelSize);  //[-1,1]


        if(d<-1.0 || d>=1.0){
            return vec3(0.1,0.1,0.1);
        }
   
       // 表面点标记为白色
        if (abs(d) < 0.001) {
            return vec3(1.0,1.0,0.0); // 表面=白色
        }
        // 表面外部（蓝→白渐变）
        else if (d > 0.0) {
            return mix(vec3(0,0,1), vec3(1), d);
        }
        // 表面内部（红→白渐变）
        else {
            return mix(vec3(1,0,0), vec3(1), -1.0*d);
        }
   


}



void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    Ray ray = generateRay(gl_FragCoord.xy, g_View.viewportSize,g_View.cameraPos, g_View.cameraFront, g_View.cameraRight, g_View.cameraUp, g_View.fov);

    float MaxExtend = g_sdfData.clipPosDistance[2].w * 2.0;

    // Ray marching
    float t = rayMarch(ray, MaxExtend, 256);
    

    // 着色
    if (t < MaxExtend) {
        vec3 p = ray.origin + ray.direction * t;
        ClipmapSample sam = sampleClipmap(p);
        fragColor = vec4(shade(p, ray.direction, sam), 1.0);
    } else {
        // 背景
        fragColor = vec4(vec3(0.1), 1.0);
    }
}

void main() {
    mainImage(FragColor, gl_FragCoord.xy);
}



//#type compute
//
//#version 450 core
//
//layout(local_size_y = 8,local_size_y = 8, local_size_z = 8) in;
//
//
//#ifdef GL_ES
//precision mediump float;  // 必须声明精度（ES 要求）
//#endif
//
//
//#include <sdf_cb.h>
////uniform sampler3D uClipSDF1;
////uniform sampler3D uClipSDF2;
////uniform sampler3D SceneDepth;
//
//
//layout(std140, binding = 8) uniform GDFUbo {
//    GlobalSDFConstants g_Gdf;
//};
//
////layout(binding = 1, r16f) uniform image2D uClipSDF0;
//
//layout(binding = 1) uniform sampler3D uClipSDF0;  
//
//layout(binding = 2, r16f) uniform writeonly image2D t_SDFDebugTexture;
//
//#include <common/CommonViewStruct.glsl>
//
//
//
//void main() {
//        //64*64
//        vec3 texCoords   =  vec3(gl_GlobalInvocationID.xyz);
//        vec3 worldPos = g_Gdf.clipmapCenter + (texCoords - g_Gdf.clipmapDimension *0.5) * g_Gdf.voxelSize;
//        
//        
//        vec4 ndcPos = g_View.matProjectionView * vec4(worldPos,1.0); // [-1,1]
//        ndcPos = ndcPos / ndcPos.w;
//        if(abs(ndcPos.x)>1 ||  abs(ndcPos.y)>1 ||abs(ndcPos.z)>1)
//            return;
//
//        vec3 uv = (ndcPos.xyz + vec3(1.0)) * 0.5;
//
//        float dist = textureLod(uClipSDF0, texCoords/g_Gdf.clipmapDimension, 0).r;
//         dist = dist*2.0 -1.0;
//         dist *= (g_Gdf.clipmapDimension.x* g_Gdf.voxelSize);
//        imageStore(t_SDFDebugTexture, ivec2(g_View.viewportSize * uv.xy), vec4(dist));
////       //  float sceneZ = ConvertFromDeviceZ(depth, g_View.zNear, g_View.zFar);
////         const vec3 P  = worldPositionFromDepth(v_TexCoord, depth, inverse(g_View.matProjectionView));
////
////
////
////    float d_raw = texture(uClipSDF, uvw).r;
////    float d = remap(d_raw);
////    FragColor = vec4(falseColor(d), 1.0);
//}
//
