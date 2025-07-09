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

//#include <light_probe_cb.h>

//layout(std140, binding = 8) uniform ProbeUBO {
//    LightProbeConstants g_Probe;
//};
//

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

    color = vec3(1.0,1.0,1.0);
	FragColor = vec4(color,1.0);

}
