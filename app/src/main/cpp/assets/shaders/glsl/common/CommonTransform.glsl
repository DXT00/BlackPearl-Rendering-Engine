
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