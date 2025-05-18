#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform mat4 u_ProjectionView;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
void main()
{
	gl_Position = vec4(aPos, 1.0); //u_ProjectionView * u_Model * vec4(aPos, 1.0);
	v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
	v_TexCoord = vec2(aTexCoords.x, aTexCoords.y);
	v_Normal = mat3(u_TranInverseModel) * aNormal;//vec3(u_Model * vec4(aNormal,1.0));

}



#type fragment
#version 430 core

uniform sampler2D u_CloudTexture;
uniform sampler2D u_FinalScreenTexture;
out vec4 FragColor;
in vec2 v_TexCoord;

void main() {
	vec4 sceneColor = texture(u_FinalScreenTexture, v_TexCoord);
	vec4 cloudColor = texture(u_CloudTexture, v_TexCoord);

	/*sceneColor.rgb *= cloudColor.a;*/
	sceneColor.rgb += cloudColor.rgb;

	FragColor = sceneColor;

}

