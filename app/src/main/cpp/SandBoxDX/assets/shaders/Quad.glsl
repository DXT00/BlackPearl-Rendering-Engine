#type vertex
#version 430 core
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
#version 430 core

out vec4 FragColor;
in vec2 TexCoords;


uniform Material u_Material;
const float offset = 1.0 / 300.0; 
void main(){

	//FragColor = texture(u_Material.diffuse,TexCoords);
//	vec3 col = texture(u_Material.diffuse,TexCoords).rgb;
//	FragColor = vec4(col,1.0);

//
//	FragColor = texture(u_Material.diffuse, TexCoords);
//    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//    FragColor = vec4(average, average, average, 1.0);

//vec2 offsets[9] = vec2[](
//        vec2(-offset,  offset), // ����
//        vec2( 0.0f,    offset), // ����
//        vec2( offset,  offset), // ����
//        vec2(-offset,  0.0f),   // ��
//        vec2( 0.0f,    0.0f),   // ��
//        vec2( offset,  0.0f),   // ��
//        vec2(-offset, -offset), // ����
//        vec2( 0.0f,   -offset), // ����
//        vec2( offset, -offset)  // ����
//    );
//
//    float kernel[9] =float[](
//    1.0 / 16, 2.0 / 16, 1.0 / 16,
//    2.0 / 16, 4.0 / 16, 2.0 / 16,
//    1.0 / 16, 2.0 / 16, 1.0 / 16  
//);
//
//    vec3 sampleTex[9];
//    for(int i = 0; i < 9; i++)
//    {
//        sampleTex[i] = vec3(texture(u_Material.diffuse, TexCoords.st + offsets[i]));
//    }
//    vec3 col = vec3(0.0);
//    for(int i = 0; i < 9; i++)
//        col += sampleTex[i] * kernel[i];
//
//    FragColor = vec4(col, 1.0);
	vec3 color =texture(u_Material.diffuse, TexCoords).rgb;

	FragColor = vec4(color, 1.0);

//FragColor = vec4(vec3(1.0 - texture(u_Material.diffuse, TexCoords)), 1.0);
}
