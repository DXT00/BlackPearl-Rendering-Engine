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

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler3D uClipSDF;
uniform float uSliceZ;     // 范围 [0, 1]，例如 sliceIndex / float(SDF_RES)
uniform float uScale = 1.0;
uniform float uMinDistance = -1.0;
uniform float uMaxDistance = 1.0;

vec3 falseColor(float d) {
    if (d < 0.0) return vec3(1, 0, 0);
    if (d < 0.1) return vec3(1, 1, 0);
    if (d < 0.5) return vec3(0, 1, 0);
    if (d < 1.0) return vec3(0, 1, 1);
    return vec3(0.05);
}


float remap(float d) {
    return clamp((d - uMinDistance) / (uMaxDistance - uMinDistance), 0.0, 1.0);
}



void main() {
    float d_raw = texture(uClipSDF, uvw).r;
    float d = remap(d_raw);
    FragColor = vec4(falseColor(d), 1.0);
}
