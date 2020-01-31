#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

out vec3 v_LocalPos;
void main()
{
	v_LocalPos = aPos;
	gl_Position = u_ProjectionView*vec4(aPos,1.0);

}

#type fragment
#version 430 core

uniform sampler2D hdrTexture;
out vec4 FragColor;
in vec3 v_LocalPos;
const vec2 invAtan = vec2(0.1591, 0.3183);

//TODO::»¹Ã»¸ãÇå³þ
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{       
    vec2 uv = SampleSphericalMap(normalize(v_LocalPos)); // make sure to normalize localPos
    vec3 color = texture(hdrTexture, uv).rgb;

    FragColor = vec4(color, 1.0);
}




