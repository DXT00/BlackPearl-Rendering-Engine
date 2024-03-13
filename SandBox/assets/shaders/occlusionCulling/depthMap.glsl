#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

//out vec2 TexCoords;
uniform mat4 u_Model;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
uniform mat4 u_ProjectionView;

void main()
{
	//TexCoords = aTexCoords;
	gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);
}

#type fragment
#version 450 core
//out vec4 color;

float LinearizeDepth(float depth)
{
    float near = 0.1;
    float far = 100.0;
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near) / (far + near - z * (far - near));
}

void main()
{
//    float depth = LinearizeDepth(gl_FragCoord.z);
//
//    color = vec4(vec3(1.0,1.0,0.0), 1.0f);
//
       gl_FragDepth = gl_FragCoord.z;

}