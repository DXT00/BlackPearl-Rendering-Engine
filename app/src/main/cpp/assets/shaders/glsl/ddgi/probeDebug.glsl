#type vertex
#version 430 core


layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;
layout(location = Slot_aNormal) in vec3 aNormal;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;


#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>

void main()
{
    gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);

    v_TexCoord = aTexCoords;
    v_FragPos = vec3(g_Transform.matModel* vec4(aPos,1.0));
    v_Normal =  mat3(g_Transform.matInvModel)* aNormal;
}



#type fragment
#version 430 core
out vec4 FragColor;
in vec3 v_TexCoord;
in vec3 v_Normal;

#include <light_probe_cb.h>
#include<ddgi/ddgiCommon.glsl>

layout(std140, binding = 8) uniform ProbeUBO {
    LightProbeConstants g_Probe;
};

layout(std140, binding = 9) uniform DDGIUbo {
	DDGIConstants g_volume;
};


layout(binding = 2)  uniform sampler2D  uIrradiance;

void main(){
	
	
	//int uProbeType = g_Probe.probeType;
    vec3 N   = normalize(v_Normal);
	//vec3 color=LoadSHCoeffs(SHCoeffs,N);
	vec3 color;
	//if(uProbeType == PT_DIFFUSE_PROBE)//diffuse Probe
		//color = SHDiffuse(N);
//	else  //todo::
//		color = textureLod(u_Material.cube,TexCoords,0).rgb;
//
    ivec3 baseGridCoord = baseGridCoord(g_volume, g_Probe.pos); 
    int probeIdx = gridCoordToProbeIndex(g_volume, baseGridCoord);


    
    vec2 texCoord = textureCoordFromDirection(N, probeIdx, g_volume.depthTextureWidth, g_volume.depthTextureHeight, g_volume.depthProbeSideLength);

    //float dist = length(probeToPoint);

    vec3 irradiance = textureLod(uIrradiance, texCoord, 0.0f).rgb;

    //color = vec3(1.0,1.0,1.0);
	FragColor = vec4(irradiance,1.0);

}
