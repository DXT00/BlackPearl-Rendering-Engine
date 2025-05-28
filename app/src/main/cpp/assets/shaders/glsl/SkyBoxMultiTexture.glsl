#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
out vec3 TexCoords;
		
uniform mat4 u_Model;
//uniform mat4 u_ProjectionView;
uniform mat4 u_Projection;
uniform mat4 u_View;
out vec3 v_Direction;

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>

void main()
{
    v_Direction = aPos; // view direction in local cube

	TexCoords = aPos;
	vec4 pos = g_View.matProjection*mat4(mat3(g_View.matView))*vec4(aPos,1.0);


	gl_Position = pos.xyww;
	/*
	
	在坐标系统教程中，我们说过在顶点着色器运行后执行透视划分，
	即用gl_Position的xyz坐标除以它的w分量。
	我们还从深度测试教程中得知，最终分割的z分量等于那个顶点的深度值。
	利用这个信息，我们可以设置输出位置的z分量等于它的w分量，
	这将导致z分量总是等于1.0，因为当应用透视除法时，
	它的z分量转换成w / w = 1.0:
	使得sykbox的位置一直在最后头


	gl_Position 存储的是顶点在 裁剪空间 的坐标，其特点：
	坐标系范围：
	每个分量（x, y, z）的范围是 [-w, w]（w 是齐次坐标的第四分量）。
	只有满足 -w ≤ x, y, z ≤ w 的顶点才会被保留（否则被裁剪掉）。
	后续变换：
	裁剪空间的坐标会通过 透视除法（除以 w）转换到 标准化设备坐标（NDC），范围变为 [-1, 1]（OpenGL）或 [0, 1]（DirectX/Vulkan，使用反向Z时）。


	*/
}



#type fragment
#version 450 core
out vec4 FragColor;
in vec3 TexCoords;

#include <assets/shaders/glsl/common/CommonMath.glsl>

struct SkyConstants
{
    vec3 directionToLight;
    float angularSizeOfLight;

    vec3 lightColor;
    float glowSize;

    vec3 skyColor;
    float glowIntensity;

    vec3 horizonColor;
    float horizonSize;

    vec3 groundColor;
    float glowSharpness;

    vec3 directionUp;
    float lightIntensity;
    //float pad1;

    vec3 factors;
	//float pad2;

   
};

/*
	std140 对齐规则：
	标量（float/int）：对齐到 4 字节。

	vec3：对齐到 16 字节（实际占用 12 字节，后跟 4 字节填充）。

	数组/结构体：按成员最大对齐值对齐。

*/
layout(std140, binding = 2) uniform SkyConstantsUbo {

   SkyConstants g_SkyConstants;

} ;


layout(binding = 1) uniform samplerCube u_Skybox0;
layout(binding = 2) uniform samplerCube u_Skybox1;
layout(binding = 3) uniform samplerCube u_Skybox2;


in vec3 v_Direction;



const vec3 betaRayleigh = vec3(5.5e-6, 13.0e-6, 22.4e-6); // R, G, B (scattering coefficient)
const vec3 betaMie = vec3(21e-6);                         // Mie coefficient (gray)

const float mieG = 0.76; // anisotropy factor for Mie

// Phase function for Rayleigh scattering
float phaseRayleigh(float cosTheta)
{
    return (3.0 / (16.0 * PI)) * (1.0 + cosTheta * cosTheta);
}

// Phase function for Mie scattering (Henyey-Greenstein)
float phaseMie(float cosTheta)
{
    float g2 = mieG * mieG;
    return (3.0 / (8.0 * PI)) * ((1.0 - g2) * (1.0 + cosTheta * cosTheta)) / pow(1.0 + g2 - 2.0 * mieG * cosTheta, 1.5);
}

void main(){

    vec3 viewDir = normalize(v_Direction);
    vec3 totalScattering = vec3(0.0);
    float viewHeight = max(dot(viewDir, vec3(0,1,0)), 0.0);

        //散射比例， 用于混合散射颜色和skybox颜色
     float scatterStrength = 0.2;//clamp(1.0 - viewHeight, 0.0, 1.0); // near horizon = more scattering

    if (viewDir.y > 0.0) {
      float cosTheta = dot(viewDir, -1.0*normalize(g_SkyConstants.directionToLight));

    // Compute phase functions
    float rayleighPhase = phaseRayleigh(cosTheta);
    float miePhase = phaseMie(cosTheta);

    // Approximate optical depth based on view height (horizon = more atmosphere)
    float opticalDepth = exp(-1.0 / max(viewHeight, 0.01));

    // Combine scattering
    vec3 rayleigh = betaRayleigh * rayleighPhase;
    vec3 mie = betaMie * miePhase;

    vec3 totalScattering = (rayleigh + mie) * opticalDepth * g_SkyConstants.lightIntensity*g_SkyConstants.lightColor;//u_SunIntensity

 
    }else{

        totalScattering =  g_SkyConstants.groundColor;
        scatterStrength = 0.8;//
    }
  




	vec4 skyBoxColor =// texture(u_Skybox0, TexCoords);
	g_SkyConstants.factors.x * texture(u_Skybox0,TexCoords)
				+g_SkyConstants.factors.y * texture(u_Skybox1,TexCoords)
				+g_SkyConstants.factors.z * texture(u_Skybox2,TexCoords);//*vec4(u_Material.diffuseColor,1.0);


    vec3 finalColor = mix(skyBoxColor.xyz, totalScattering, scatterStrength);
    finalColor = pow(finalColor, vec3(1.0 / 2.2)); // gamma correction
    FragColor = vec4(finalColor, 1.0);

//    vec3 direction = normalize(v_Direction);
//    float angularSizeOfPixel = 0.5;//max(length(ddx(direction)), length(ddy(direction)));
//    float elevation = asin(clamp(dot(direction, g_SkyConstants.directionUp), -1.0, 1.0));
//    float top = smoothstep(0.f, g_SkyConstants.horizonSize, elevation);
//    float bottom = smoothstep(0.f, g_SkyConstants.horizonSize, -elevation);
//    vec3 environment = mix(mix(g_SkyConstants.horizonColor, g_SkyConstants.groundColor, bottom), g_SkyConstants.skyColor, top);
//
//    float angleToLight = acos(saturate(dot(direction, g_SkyConstants.directionToLight)));
//    float halfAngularSize = g_SkyConstants.angularSizeOfLight * 0.5;
//    float lightIntensity = saturate(1.0 - smoothstep(halfAngularSize - angularSizeOfPixel * 2, halfAngularSize + angularSizeOfPixel * 2, angleToLight));
//    lightIntensity = pow(lightIntensity, 4.0);
//    float glowInput = saturate(2.0 * (1.0 - smoothstep(halfAngularSize - g_SkyConstants.glowSize, halfAngularSize + g_SkyConstants.glowSize, angleToLight)));
//    float glowIntensity = g_SkyConstants.glowIntensity * pow(glowInput, g_SkyConstants.glowSharpness);
//    vec3 light = max(lightIntensity, glowIntensity) * g_SkyConstants.lightColor;
//    
//    FragColor = vec4( environment + light,1.0);
}
