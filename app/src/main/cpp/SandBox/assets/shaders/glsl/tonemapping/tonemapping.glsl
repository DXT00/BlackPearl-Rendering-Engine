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

out vec4 FragColor;
in vec2 v_TexCoord;

#include <assets/shaders/glsl/common/CommonMath.glsl>
#include <tonemapping_cb.h>
//#if USE_GLES_PLS
//
//__pixel_localEXT FragData {
//    layout(rgb10_a2)  vec4 t_gSceneColor;
//} pls;
//
//#else
    layout(binding = 0) uniform sampler2D t_gSceneColor;
//#endif
    layout(std140, binding = 8) uniform ToneMappingUBO {
        ToneMappingConstants g_ToneMapping;
    };

    
// ACES tone mapping curve fit to go from HDR to LDR
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x + b)) / (x*(c*x + d) + e));
}

float3 LessThan(float3 f, float value)
{
    return float3(
        (f.x < value) ? 1.f : 0.f,
        (f.y < value) ? 1.f : 0.f,
        (f.z < value) ? 1.f : 0.f);
}

float3 LinearToSRGB(float3 rgb)
{
    rgb = clamp(rgb, 0.f, 1.f);
    return mix(
        pow(rgb * 1.055f, vec3(1.f / 2.4f)) - vec3(0.055f),
        rgb * 12.92f,
        LessThan(rgb, 0.0031308f)
    );
}

void main(){

//#if USE_GLES_PLS
//    vec3 color = pls.t_gSceneColor.rgb;
//#else
    vec3 color = texture(t_gSceneColor, v_TexCoord).rgb;



    // Tonemapping
     if ((g_ToneMapping.flag & PostProcess_Tonemapping) != 0u)
     {
         color = ACESFilm(color);
     }


    // Gamma correction
    if ((g_ToneMapping.flag  & PostProcess_Gamma) != 0u)
    {
        color = LinearToSRGB(color);
    }




//#endif

//	bool hdr = u_Settings.hdr;
//	if(hdr){
//	//HDR tonemapping
//	 color = color / (color + vec3(1.0));
//	//gamma correction
//    color = pow(color, vec3(1.0/2.2));  //1.0/2.2
//	}
	FragColor = vec4(color,1.0);//vec4(1.0,0.0,0.0, 1.0);

}
