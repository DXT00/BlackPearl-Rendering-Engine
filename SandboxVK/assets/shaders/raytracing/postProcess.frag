#version 450
//#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;

layout (input_attachment_index = 0, binding = 0) uniform subpassInput DenoiseColor;

layout(location = 0) out vec4 outColor;


void main() {
    float t = 0.5;
    vec4 fragCol = subpassLoad(DenoiseColor);
    float gamma = 2.2;

    outColor = fragCol;//vec4(pow(fragCol.rgb, vec3(1.0/gamma)), 1.0);

}
