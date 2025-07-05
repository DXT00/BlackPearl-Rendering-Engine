#type compute
#version 450 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(rgba32f, binding = 0) uniform image3D imgOutput;

void main() {
    ivec3 texelCoord = ivec3(gl_GlobalInvocationID.xyz);
    imageStore(imgOutput, texelCoord, vec4(0.0, 0.0, 0.0, 0.0));
}