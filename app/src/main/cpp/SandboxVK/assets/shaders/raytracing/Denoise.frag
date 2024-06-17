#version 450
//#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 0, rgba8) uniform  readonly image2D noiseTex;
layout(binding = 1, rgba32f) uniform readonly image2D noisePosTex;
layout(binding = 2, rgba32f) uniform readonly image2D noiseNormalTex;
layout(binding = 3, rgba8) uniform readonly image2D lastFrameTex;

layout(column_major, binding = 4) uniform DenoiseUniformBufferObject {
    vec2 screenSize;
    mat4 preProjectionView;
} ubo;

layout(std430, binding = 5) readonly buffer ObjTransObject {
    mat4[] objTrans;
 };

 //对应RT0 RT1
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outDenoiseColor;


float SqrDistance(vec3 a , vec3 b){
   return sqrt(length(a-b));
}
float SafeAcos(float v) {
    return cos(min(max(v, 0.0), 1.0));
}
const float alpha = 0.2;
const float sigmaPlane = 0.1;
const float sigmaColor = 0.6;
const float sigmaNormal = 0.1;
const float sigmaCoord = 32.0;
void FilterNoiseTex( inout vec3 filteredColor)
{

    int width = int(ubo.screenSize.x);
    int height = int(ubo.screenSize.y);

    int kernelRadius = 4;
    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);

    vec2 uv = vec2(float(x)/float(width), float(y)/float(height));

    int x_start = max(0, x - kernelRadius);
    int x_end = min(width - 1, x + kernelRadius);
    int y_start = max(0, y - kernelRadius);
    int y_end = min(height - 1, y + kernelRadius);

    vec3 center_postion = imageLoad(noisePosTex, ivec2(gl_FragCoord.xy)).xyz;
    vec3 center_normal = imageLoad(noiseNormalTex,ivec2(gl_FragCoord.xy)).xyz;
    vec3 center_color = imageLoad(noiseTex,ivec2(gl_FragCoord.xy)).xyz;

    vec3 final_color = vec3(0);
    float total_weight = 0.0;
    for (int m = x_start; m <= x_end; m++) {
        for (int n = y_start; n <= y_end; n++) {
            uv =  vec2(float(m)/float(width), float(n)/float(height));
            vec3 postion =  imageLoad(noisePosTex,ivec2(m,n)).xyz;
            vec3 normal = imageLoad(noiseNormalTex,ivec2(m,n)).xyz;
            vec3 color = imageLoad(noiseTex,ivec2(m,n)).xyz;

                    float d_position = SqrDistance(center_postion, postion) /
                                      (2.0 * sigmaCoord * sigmaCoord);
                    float d_color = SqrDistance(center_color, color) /
                                   (2.0 * sigmaColor * sigmaColor);
                    float d_normal = SafeAcos(dot(center_normal, normal));
                    d_normal *= d_normal;
                    d_normal / (2.0 * sigmaNormal * sigmaNormal);

                    float d_plane = 0.0;
                    if (d_position > 0.0) {
                        d_plane = dot(center_normal, normalize(postion - center_postion));
                    }
                    d_plane *= d_plane;
                    d_plane /= (2.0 * sigmaPlane * sigmaPlane);

                    float weight = exp(-d_plane - d_position - d_color - d_normal);
                    total_weight += weight;
                    final_color += color * weight;
        }
    }
    filteredColor = final_color/total_weight;

}

void Reprojection(inout vec3 lastFrameColor){
    int width = int(ubo.screenSize.x);
    int height = int(ubo.screenSize.y);

    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);

    vec2 uv =  vec2(float(x)/float(width), float(y)/float(height));

    vec3 curPos = imageLoad(noisePosTex,ivec2(x,y)).xyz;
    float w = imageLoad(noisePosTex,ivec2(x,y)).w;
    int objId =  int(w);
    vec4 lastScreenPos;
    if(objId == -1)
    {
      lastScreenPos =  vec4(gl_FragCoord.xy/ubo.screenSize.xy,1,0);
      lastFrameColor = vec3(0);
      return;
    }else{
     lastScreenPos= ubo.preProjectionView*objTrans[objId]*inverse(objTrans[objId])*vec4(curPos,1.0);
    }
    lastScreenPos = lastScreenPos/lastScreenPos.w;

    //TODO::判断上一帧的objID是否和这一帧一样
     

    lastFrameColor = imageLoad(lastFrameTex, ivec2(lastScreenPos.xy*ubo.screenSize.xy)).xyz;
}
void TemporalAccumulation(in vec3 filteredColor)
{
    vec3 lastFrameColor;
    Reprojection(lastFrameColor);
    vec3 outColor = filteredColor*alpha + (1.0- alpha)*lastFrameColor;

}
void main() {

     float gamma = 2.2;
    vec3 filteredColor;
    FilterNoiseTex(filteredColor);
    vec3 lastFrameColor = vec3(0);
    Reprojection(lastFrameColor);
    vec3 color = filteredColor*alpha + (1.0- alpha)*lastFrameColor;
    vec2 uv = vec2(gl_FragCoord.x/ubo.screenSize.x,gl_FragCoord.y/ubo.screenSize.y);
    
    outColor = vec4(pow(color.rgb, vec3(1.0/gamma)), 1.0);
    outDenoiseColor = vec4(color.rgb, 1.0); // vec4(filteredColor,1.0);

}

