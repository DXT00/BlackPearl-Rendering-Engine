#type vertex
#version 430 core


layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;
layout(location = Slot_aNormal) in vec3 aNormal;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;


#include <assets/shaders/glsl/common/CommonViewStruct.glsl>
#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>

void main()
{
    gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);

    v_TexCoord = aTexCoords;
    v_FragPos = vec3(g_Transform.matModel* vec4(aPos,1.0));
    v_Normal =  mat3(g_Transform.matInvModel)* aNormal;
}



#type fragment
#version 430 core
out vec4 FragColor;
in vec3 v_TexCoord;
in vec3 v_Normal;

#include <light_probe_cb.h>

layout(std140, binding = 8) uniform ProbeUBO {
    LightProbeConstants g_Probe;
};



//uniform Material u_Material;
//
//uniform samplerCube cubeMap;
//
//uniform int u_ProbeType;


//vec3 SHDiffuse(const vec3 sh[9],const vec3 n){
//	
//		//------------------------------------------------------------------
//	// These are variables to hold x,y,z and squares and products
//
//	float x2 ;
//	float  y2 ;
//	float z2 ;
//	float xy ;
//	float  yz ;
//	float  xz ;
//	float x ;
//	float y ;
//	float z ;
//	vec3 col ;
//  //------------------------------------------------------------------       
//  // We now define the constants and assign values to x,y, and z 
//	
//	const float c1 = 0.429043 ;
//	const float c2 = 0.511664 ;
//	const float c3 = 0.743125 ;
//	const float c4 = 0.886227 ;
//	const float c5 = 0.247708 ;
//	x = n.x,y = n.y,z = n.z;
//	//------------------------------------------------------------------ 
//  // We now compute the squares and products needed 
//
//	x2 = x*x ; y2 = y*y ; z2 = z*z ;
//	xy = x*y ; yz = y*z ; xz = x*z ;
//  //------------------------------------------------------------------ 
//  // Finally, we compute equation 13
//
//	col = c1*sh[8]*(x2-y2) + c3*sh[6]*z2 + c4*sh[0] - c5*sh[6] + 2.0*c1*(sh[4]*xy + sh[7]*xz + sh[5]*yz) + 2.0*c2*(sh[3]*x+sh[1]*y+sh[2]*z) ;
//
//	return col ;
//	
//	
////	float x = normal.x;
////	float y = normal.y;
////	float z = normal.z;
////
////	vec3 result = (
////		sh[0] +
////		
////		sh[1] * x +
////		sh[2] * y +
////		sh[3] * z +
////		
////		sh[4] * z * x +
////		sh[5] * y * z +
////		sh[6] * y * x +
////		sh[7] * (3.0 * z * z - 1.0) +
////		sh[8] * (x*x - y*y)
////  );
////
////  return max(result, vec3(0.0));
////
//
//
//
//}
//uniform vec3 u_SHCoeffs[9];

vec3 SHDiffuse(const vec3 normal){
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;

	vec3 result = (
		g_Probe.SHCoeffs[0] +

		g_Probe.SHCoeffs[1] * x +
		g_Probe.SHCoeffs[2] * y +
		g_Probe.SHCoeffs[3] * z +

		g_Probe.SHCoeffs[4] * z * x +
		g_Probe.SHCoeffs[5] * y * z +
		g_Probe.SHCoeffs[6] * y * x +
		g_Probe.SHCoeffs[7] * (3.0 * z * z - 1.0) +
		g_Probe.SHCoeffs[8] * (x*x - y*y)
  ).xyz;

  return max(result, vec3(0.0));




}
//layout(rgba32f, binding=0) uniform image2D u_Image;
//bool hasNeg = false;
//vec3 LoadSHCoeffs(inout vec3 SHCoeffs[9],const vec3 n){
//	for(int i=0;i<9;i++){
//
//		SHCoeffs[i] = imageLoad(u_Image,ivec2(i,0)).rgb;
//
//		if(SHCoeffs[i].x<0.0||SHCoeffs[i].y<0.0||SHCoeffs[i].z<0.0)
//			hasNeg = true;
//		SHCoeffs[i] = SHCoeffs[i]-vec3(0.5,0.5,0.5);
//		SHCoeffs[i]*=10.0;
//	}
//
//	float x2 ;
//	float  y2 ;
//	float z2 ;
//	float xy ;
//	float  yz ;
//	float  xz ;
//	float x ;
//	float y ;
//	float z ;
//	vec3 col ;
//  //------------------------------------------------------------------       
//  // We now define the constants and assign values to x,y, and z 
//	
//	const float c1 = 0.429043 ;
//	const float c2 = 0.511664 ;
//	const float c3 = 0.743125 ;
//	const float c4 = 0.886227 ;
//	const float c5 = 0.247708 ;
//	x = n.x,y = n.y,z = n.z;
//	//------------------------------------------------------------------ 
//  // We now compute the squares and products needed 
//
//	x2 = x*x ; y2 = y*y ; z2 = z*z ;
//	xy = x*y ; yz = y*z ; xz = x*z ;
//  //------------------------------------------------------------------ 
//  // Finally, we compute equation 13
//
//	col = c1*SHCoeffs[8]*(x2-y2) + c3*SHCoeffs[6]*z2 + c4*SHCoeffs[0] - c5*SHCoeffs[6] + 2.0*c1*(SHCoeffs[4]*xy + SHCoeffs[7]*xz + SHCoeffs[5]*yz) + 2.0*c2*(SHCoeffs[3]*x+SHCoeffs[1]*y+SHCoeffs[2]*z) ;
//
//	return col;
//
//}
//binding = 0 对应 GL_TEXTURE0
void main(){
	
	
	int uProbeType = g_Probe.probeType;
    vec3 N   = normalize(v_Normal);
	//vec3 color=LoadSHCoeffs(SHCoeffs,N);
	vec3 color;
	if(uProbeType == PT_DIFFUSE_PROBE)//diffuse Probe
		color = SHDiffuse(N);
//	else  //todo::
//		color = textureLod(u_Material.cube,TexCoords,0).rgb;
//


	FragColor = vec4(color,1.0);

}
