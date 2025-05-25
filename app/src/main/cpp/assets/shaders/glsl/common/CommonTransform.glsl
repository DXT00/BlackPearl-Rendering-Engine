#ifndef  BP_COMMON_TRAANSFORM_H
#define  BP_COMMON_TRAANSFORM_H
/*
    pixelPos：[viewport size]
    z_viewSpace: linear SceneDepth in view space, get from Gbuffer.Depth if is deferred shading
*/
float3 ScreenSpaceToWorldPosition(float2 pixelPos, float z_viewSpace){
    //  world = gl_pos * (pro * view)^-1
    vec3 viewPos = (pixelPos, z_viewSpace);
    vec3 clipPos = g_View.matProjection* viewPos;
    vec3 worldPos = clipPos * inverse(g_View.matProjectionView);
    return worldPos;
}

void getTBN(in vec3 fragPos,in vec2 texCoord, in vec3 N, out vec3 T, out vec3 B)
{
    //vec3 tangentNormal =  2.0* texture(u_Material.normal, texCoord).xyz- vec3(1.0);
	//vec3 tangentNormal =  2.0* normal- vec3(1.0);

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    N   = normalize(N);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    //mat3 TBN = mat3(T, B, N);

}


#endif //COMMON_TRAANSFORM_H