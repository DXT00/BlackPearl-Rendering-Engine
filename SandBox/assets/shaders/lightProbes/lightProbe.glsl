#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 TexCoords;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!

void main()
{
	TexCoords = aPos;
	v_Normal = aNormal;//mat3(u_TranInverseModel)* aNormal;

	gl_Position = u_ProjectionView*u_Model*vec4(aPos,1.0);

	//gl_Position = pos.xyww;
	/*
	
	在坐标系统教程中，我们说过在顶点着色器运行后执行透视划分，
	即用gl_Position的xyz坐标除以它的w分量。
	我们还从深度测试教程中得知，最终分割的z分量等于那个顶点的深度值。
	利用这个信息，我们可以设置输出位置的z分量等于它的w分量，
	这将导致z分量总是等于1.0，因为当应用透视除法时，
	它的z分量转换成w / w = 1.0:
	使得sykbox的位置一直在最后头
	
	*/
}



#type fragment
#version 430 core
out vec4 FragColor;
in vec3 TexCoords;
in vec3 v_Normal;

uniform struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;
	samplerCube cube;
	float shininess;

}u_Material;

uniform samplerCube cubeMap;
float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth){
	float z = depth*2.0-1.0;//Back to NDC coordinate
	return 2.0*near*far /(far+near - z*(far - near));

}
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
uniform vec3 u_SHCoeffs[9];

vec3 SHDiffuse(const vec3 normal){
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;

	vec3 result = (
		u_SHCoeffs[0] +
		
		u_SHCoeffs[1] * x +
		u_SHCoeffs[2] * y +
		u_SHCoeffs[3] * z +
		
		u_SHCoeffs[4] * z * x +
		u_SHCoeffs[5] * y * z +
		u_SHCoeffs[6] * y * x +
		u_SHCoeffs[7] * (3.0 * z * z - 1.0) +
		u_SHCoeffs[8] * (x*x - y*y)
  );

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
	
	

    vec3 N   = normalize(v_Normal);
	//vec3 color=LoadSHCoeffs(SHCoeffs,N);
	vec3 color =SHDiffuse(N);//imageLoad(u_Image,ivec2(4,0)).rgb;//
	
//	vec3 color =texture(u_Material.cube,TexCoords).rgb;
//	color = color/(color+vec3(1.0));
//	color = pow(color,vec3(1.0/2.2));
//
//	if(color.x<=0.0||color.y<=0.0||color.z<=0.0)
//		FragColor = vec4(1.0,0.0,0.0,1.0);
//		else
//				FragColor = vec4(0.0,1.0,0.0,1.0);
	 color = pow(color,vec3(1.0/2.2));

	FragColor = vec4(color,1.0);

}
