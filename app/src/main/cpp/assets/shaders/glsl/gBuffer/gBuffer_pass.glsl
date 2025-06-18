#type vertex
#version 450 core

layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;
layout(location = Slot_aNormal) in vec3 aNormal;


#if USE_TBN
layout(location = 3) in vec3 aTangent;
#endif

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

#if USE_TBN
out vec3 v_Tangent;
#endif


#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>


//uniform mat4 u_ProjectionView;
//uniform mat4 u_Model;
//uniform mat4 u_TranInverseModel;
//

void main(){

	//gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);
	
	gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);

    v_TexCoord = aTexCoords;
    v_FragPos = vec3(g_Transform.matModel* vec4(aPos,1.0));
    v_Normal =  mat3(g_Transform.matInvModel)* aNormal;
#if USE_TBN
	v_Tangent =  mat3(g_Transform.matInvModel)*aTangent;
#endif
}




#type fragment
#version 450 core



#if USE_GLES_PLS


//#extension GL_EXT_shader_pixel_local_storage : require
__pixel_local_outEXT OutPLS
{
    layout(rgb10_a2) vec4 gSceneColor;
    layout(rgba8) vec4 gGbufferA;
    layout(rgba8) vec4 gGbufferB;
    layout(rgba8) vec4 gGbufferC;
} pls;
layout(early_fragment_tests) in;
#else

/* MRT */
/* render to gBuffer */
layout (location = 0) out vec3 gSceneColor; //use for emissive color and fog
layout (location = 1) out vec4 gGbufferA;  //encode normal.xy + Encode IndirectIrradiance + reserve
layout (location = 2) out vec4 gGbufferB; // Metallic + Specular + Roughness + ShadingModelID / 255.0
layout (location = 3) out vec4 gGbufferC; // BaseColor + PrecomputedShadow

#endif


//
#include <assets/shaders/glsl/common/CommonGbufferStruct.glsl>

#include <assets/shaders/glsl/common/CommonViewStruct.glsl>

#include <assets/shaders/glsl/common/CommonTransform.glsl>

#include <assets/shaders/glsl/gBuffer/gBuffer.glsl>

#include <assets/shaders/glsl/common/CommonOctahedral.glsl>


///* 存储全局光照中的 diffuse 和specular (vec3 ambient =  (Kd*diffuse+specular) * ao;)的颜色*/
//layout (location = 4) out vec4 gAmbientGI_AO; //GIAmbient + u_Material.ao
//layout (location = 5) out vec3 gNormalMap;
in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
#if USE_TBN
in vec3	v_Tangent;
#endif


vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


void main(){
	MaterialTextureSample textures = SampleMaterialTexturesAuto(v_TexCoord);
	SurfaceGeometry geom;
    geom.position = v_FragPos;
    geom.normal = normalize(v_Normal);


#if USE_TBN
    geom.tangent = normalize(v_Tangent);
    geom.bitangent = normalize(cross(geom.normal, geom.tangent));
#else
    getTBN(v_FragPos, v_TexCoord, v_Normal, geom.tangent, geom.bitangent);
#endif


#if COOK
    MaterialSample mat = CreateCookBSDFMaterial(geom, g_Mat, v_TexCoord);
#elif (Disney)
    DisneyMaterialSample mat = CreateDisneyBSDFMaterial(geom, g_Mat, v_TexCoord);
#endif

	GBufferData GBuffer = GetGBufferFormMatetial(geom, mat);



    half IndirectIrradiance = 1.0;
	IndirectIrradiance *= mat.ao;


	GBuffer.IndirectIrradiance = IndirectIrradiance;
	//fog + emissive
	vec3 sceneColor = mat.emissive;//vec4(mat.emissive,1.0);

    half4 bufferA;
    half4 bufferB;
    half4 bufferC;

	MobileEncodeGBuffer(GBuffer, bufferA, bufferB, bufferC);



	#if USE_GLES_PLS
	    	pls.gSceneColor = vec4(sceneColor,1.0);//half4(1.0);//
	    	pls.gGbufferA = bufferA;
	    	pls.gGbufferB = bufferB;
	    	pls.gGbufferC = bufferC;

	#else
	    	gSceneColor = sceneColor;
	    	gGbufferA = bufferA;
            gGbufferB = bufferB;
            gGbufferC = bufferC;
	#endif



}






