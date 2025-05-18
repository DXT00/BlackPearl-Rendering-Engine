#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoords;
    gl_Position = vec4(aPos,1.0);

}

#type fragment
#version 430 core
 
uniform sampler2D u_LastMip;
uniform ivec2 u_LastMipSize;
uniform int u_LastMipLevel;
 
in vec2 TexCoord;
vec2 offset[4] = {
    vec2(0, 0),
    vec2(-1, 0),
    vec2(-1,-1),
    vec2(0,-1)
};
vec2 cornerOffset[5] = {
     ivec2(1,  0), //x - right
     ivec2(1, -1),//x - right bottom
     ivec2( 0, 1),// y - up
     ivec2( -1, 1),// y -up left
     ivec2( 1, 1),// x,y - up right
};
void main(void) {
    vec4 texels;
    texels.x = textureLod(u_LastMip, TexCoord + offset[0]/float(u_LastMipSize), u_LastMipLevel).x;
    texels.y = textureLod(u_LastMip, TexCoord + offset[1]/float(u_LastMipSize), u_LastMipLevel).x;
    texels.z = textureLod(u_LastMip, TexCoord + offset[2]/float(u_LastMipSize), u_LastMipLevel).x;
    texels.w = textureLod(u_LastMip, TexCoord + offset[3]/float(u_LastMipSize), u_LastMipLevel).x;
    float maxZ = max(max(texels.x, texels.y), max(texels.z, texels.w));
 
    vec3 extra;
    // if we are reducing an odd-width texture then fetch the edge texels
    if ( ((u_LastMipSize.x & 1) != 0) && (int(gl_FragCoord.x*2.0) == u_LastMipSize.x-2) ) {
        // if both edges are odd, fetch the top-left corner texel
        if (((u_LastMipSize.y & 1) != 0) && (int(gl_FragCoord.y*2.0) == u_LastMipSize.y-2)) {
            extra.z = textureLod(u_LastMip, TexCoord + cornerOffset[4]/float(u_LastMipSize),u_LastMipLevel ).x;
            maxZ = max(maxZ, extra.z);
        }
        extra.x = textureLod(u_LastMip, TexCoord+ cornerOffset[0]/float(u_LastMipSize),u_LastMipLevel).x;
        extra.y = textureLod(u_LastMip, TexCoord+ cornerOffset[1]/float(u_LastMipSize),u_LastMipLevel).x;
        maxZ = max(maxZ, max(extra.x, extra.y));
    } else
    // if we are reducing an odd-height texture then fetch the edge texels
    if (((u_LastMipSize.y & 1) != 0) && (int(gl_FragCoord.y*2.0) == u_LastMipSize.y-2)) {
        extra.x = textureLod(u_LastMip, TexCoord+ cornerOffset[2]/float(u_LastMipSize),u_LastMipLevel).x;
        extra.y = textureLod(u_LastMip, TexCoord+ cornerOffset[3]/float(u_LastMipSize),u_LastMipLevel).x;
        maxZ = max(maxZ, max(extra.x, extra.y));
    }
    gl_FragDepth = maxZ;
}