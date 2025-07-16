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
    gl_Position = vec4(aPos, 1.0);
}

#type fragment
#version 450 core

#include<bloom_cb.h>
#include<common/CommonMath.glsl>


out vec4 FragColor;
in vec2 v_TexCoord;


layout(binding = 8) uniform BloomUBO{
    BloomConstants g_Bloom;

}
layout(binding = 1) sampler2D srcTexture;

void main() {
    vec2 pos = gl_FragCoord.xy;
    float3 result = texture(srcTexture, pos.xy).rgb;

    for (float x = 1; x < g_Bloom.numSamples; x += 2)
    {
        float w1 = exp(square(x) * g_Bloom.argumentScale);
        float w2 = exp(square(x + 1) * g_Bloom.argumentScale);

        float w12 = w1 + w2;
        float p = w2 / w12;
        float2 offset = g_Bloom.pixstep * (x + p);

        result +=  textureLod(srcTexture， uv + offset, 0).rgb * w12;
        result +=  textureLod(srcTexture， uv - offset, 0).rgb * w12;
    }

    result *= g_Bloom.normalizationScale;


    FragColor = vec4(color, 1.0);//vec4(1.0,0.0,0.0, 1.0);

}
