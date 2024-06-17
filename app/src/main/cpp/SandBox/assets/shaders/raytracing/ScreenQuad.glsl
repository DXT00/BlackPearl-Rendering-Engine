#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;


void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos,1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;
in vec2 TexCoords;


uniform sampler2D u_FinalScreenTexture;
uniform Settings u_Settings;
bool hdr = u_Settings.hdr;
uniform float u_Num;
uniform int u_isDepth;
uniform int u_isMipmap;

uniform int u_Lod;

void main(){
	vec3 color;

	if(u_isDepth == 1){
		if(u_isMipmap == 1)
			color = vec3(textureLod(u_FinalScreenTexture, TexCoords, u_Lod).r);
		else
			color = vec3(texture(u_FinalScreenTexture, TexCoords).r);
	} else{
		color = texture(u_FinalScreenTexture, TexCoords).rgb/u_Num;
	}
	FragColor = vec4(color, 1.0);

//	if(hdr){
//	//HDR tonemapping
//	 color = color / (color + vec3(1.0));
//	//gamma correction
//   
//	}
//	color = pow(color, vec3(1.0/2.2));  //1.0/2.2

}
