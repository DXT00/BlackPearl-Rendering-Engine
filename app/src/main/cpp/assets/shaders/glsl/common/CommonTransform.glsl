#ifndef  BP_COMMON_TRAANSFORM_H
#define  BP_COMMON_TRAANSFORM_H
/*
    pixelPos：[viewport size]
    z_viewSpace: linear SceneDepth in view space, get from Gbuffer.Depth if is deferred shading

    //  world = gl_pos * (pro * view)^-1
*/
float3 ScreenSpaceToWorldPosition(float2 pixelPos, float z_viewSpace)
{    
    float4 viewPos = float4(pixelPos.x, pixelPos.y, z_viewSpace, 1.0);
    float4 clipPos = g_View.matProjection* viewPos;
    float4 worldPos = clipPos * inverse(g_View.matProjectionView);
    return worldPos.xyz;
}
/*
考虑一个三角形，其三个顶点的位置为 P1、P2、P3，对应的纹理坐标为 UV1、UV2、UV3。我们定义两个边向量和对应的纹理坐标差值：

边向量：

Edge1 = P2 - P1

Edge2 = P3 - P1

纹理坐标差值：

DeltaUV1 = UV2 - UV1

DeltaUV2 = UV3 - UV1

我们假设边向量可以表示为切线和副切线的线性组合：


Edge1 = DeltaUV1.x * Tangent + DeltaUV1.y * Bitangent
Edge2 = DeltaUV2.x * Tangent + DeltaUV2.y * Bitangent

这是一个线性方程组，我们可以通过求解这个方程组来获得切线和副切线向量。具体的求解过程涉及线性代数中的矩阵求逆和向量运算，最终得到以下公式：


f = 1 / (DeltaUV1.x * DeltaUV2.y - DeltaUV2.x * DeltaUV1.y)
Tangent = f * (DeltaUV2.y * Edge1 - DeltaUV1.y * Edge2)
Bitangent = f * (-DeltaUV2.x * Edge1 + DeltaUV1.x * Edge2)

*/
void getTBN(in vec3 fragPos,in vec2 texCoord, in vec3 N, out vec3 T, out vec3 B)
{
    //vec3 tangentNormal =  2.0* texture(u_Material.normal, texCoord).xyz- vec3(1.0);
	//vec3 tangentNormal =  2.0* normal- vec3(1.0);

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoord);
    vec2 st2 = dFdy(texCoord);

    vec3 N_   = normalize(N);
    T  = normalize(Q1*st2.t - Q2*st1.t);
    B  = -normalize(cross(N_, T));
    //mat3 TBN = mat3(T, B, N);

}


#endif //COMMON_TRAANSFORM_H