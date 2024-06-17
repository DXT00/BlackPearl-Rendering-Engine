#type vertex
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform mat4 u_ProjectionView;

out vec2 v_TexCoord;
out vec3 v_FragPos;
void main()
{
	gl_Position = u_ProjectionView * u_Model * vec4(aPos, 1.0);
	v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
	v_FragPos = v_FragPos * 0.5 + 0.5;
	v_TexCoord = vec2(aTexCoords.x, aTexCoords.y);
	//v_Normal = mat3(u_TranInverseModel) * aNormal;//vec3(u_Model * vec4(aNormal,1.0));

}

#type fragment
#version 450 core

in vec3 v_FragPos;
in vec2 v_TexCoord;

out vec4 FragColor;
uniform sampler3D u_Texture3DNoise;

void main() {
	vec3 tex3DUvw = v_FragPos;// *0.5 + 0.5;
	vec4 colorSample = texture(u_Texture3DNoise, tex3DUvw,0);// _VolumeTex.SampleLevel(sampler_VolumeTex, tex3DUvw, _mipLevel);


	FragColor = colorSample;
}