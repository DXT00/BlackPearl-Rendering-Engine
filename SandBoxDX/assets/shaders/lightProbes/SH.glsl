/******************************************************************************

envmaprender.in: Shaders used for rendering irradiance environment
                 maps with spherical harmonic coefficients.  This file 
		 is designed to be used with the Stanford Real-Time
		 programmable shading system.  This system is available 
		 from http://graphics.stanford.edu/projects/shading/

Other files    : The other files required are grace.scene which
                 defines a simple scene.  You will also need to add
		 the following 2 lines to scviewer.in when using scviewer

		 file envmaprender.in
		 scene grace.scene

Functions      : We provide general functions for rendering, plus the
                 spherical harmonic coefficients for the Grace
		 Cathedral as an example.  These coefficients were
		 generated using prefilter.c  from the environment 
		 available from http://www.debevec.org

Usage          : When running scviewer, simply choose the scene Grace
                 Cathedral.  It should be fairly obvious how to modify 
		 the source to handle your own environments.

Reference      : This is an implementation of the method described by
                 Ravi Ramamoorthi and Pat Hanrahan in their SIGGRAPH 2001 
	         paper, "An Efficient Representation for Irradiance
	         Environment Maps".

Author         : Ravi Ramamoorthi

Date           : Last modified on April 14, 2001

******************************************************************************/
#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
out vec2 TexCoords;
		
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
}

#type fragment
#version 430 core

float areaElement (float x, float y) {
  return atan(x * y, sqrt(x * x + y * y + 1.0));
}
float texelSolidAngle (float aU, float aV, float width, float height) {
  // transform from [0..res - 1] to [- (1 - 1 / res) .. (1 - 1 / res)]
  // ( 0.5 is for texel center addressing)
  float U = (2.0 * (aU + 0.5) / width) - 1.0;
  float V = (2.0 * (aV + 0.5) / height) - 1.0;

  // shift from a demi texel, mean 1.0 / size  with U and V in [-1..1]
  float invResolutionW = 1.0 / width;
  float invResolutionH = 1.0 / height;

  // U and V are the -1..1 texture coordinate on the current face.
  // get projected area for this texel
  float x0 = U - invResolutionW;
  float y0 = V - invResolutionH;
  float x1 = U + invResolutionW;
  float y1 = V + invResolutionH;
  float angle = areaElement(x0, y0) - areaElement(x0, y1) - areaElement(x1, y0) + areaElement(x1, y1);

  return angle;
}



// irradcoeffs is an alternative form, corresponding to equation 13 in 
// our paper.  The inputs are 9 RGB values (i.e. 3 vectors) for the 9
// spherical harmonic lighting coefficients.  The vector n is the
// surface normal.

vec3 irradcoeefs(vec3 L00, vec3 L1_1, vec3 L10, vec3 L11, 
      vec3 L2_2, vec3 L2_1, vec3 L20, vec3 L21, vec3 L22,
      vec3 n){
      
	//------------------------------------------------------------------
	// These are variables to hold x,y,z and squares and products

	float x2 ;
	float  y2 ;
	float z2 ;
	float xy ;
	float  yz ;
	float  xz ;
	float x ;
	float y ;
	float z ;
	vec3 col ;
  //------------------------------------------------------------------       
  // We now define the constants and assign values to x,y, and z 
	
	const float c1 = 0.429043 ;
	const float c2 = 0.511664 ;
	const float c3 = 0.743125 ;
	const float c4 = 0.886227 ;
	const float c5 = 0.247708 ;
	x = n.x,y = n.y,z = n.z;
	//------------------------------------------------------------------ 
  // We now compute the squares and products needed 

	x2 = x*x ; y2 = y*y ; z2 = z*z ;
	xy = x*y ; yz = y*z ; xz = x*z ;
  //------------------------------------------------------------------ 
  // Finally, we compute equation 13

	col = c1*L22*(x2-y2) + c3*L20*z2 + c4*L00 - c5*L20 + 2*c1*(L2_2*xy + L21*xz + L2_1*yz) + 2*c2*(L11*x+L1_1*y+L10*z) ;

	return col ;

}




void updateCoeffs(inout vec3 coeffs[9],vec3 color,float domega,float x,float y,float z){
	
//	vec3 coeffs[9];
//	/*initial coeffs[9]*/
//	for(int i= 0;i<9;i++){
//		coeffs[i]= vec3(0.0,0.0,0.0);
//	}
	float c1 = 0.282095 ,c2 = 0.488603, c3 = 1.092548,c4 = 0.315392 ,c5 = 0.546274 ;
	/* L_{00}.  Note that Y_{00} = 0.282095 */
	coeffs[0]+=color*c1*domega;
	 /* L_{1m}. -1 <= m <= 1.  The linear terms */

    coeffs[1] += color*(c2*y)*domega ;   /* Y_{1-1} = 0.488603 y  */
    coeffs[2] += color*(c2*z)*domega ;   /* Y_{10}  = 0.488603 z  */
    coeffs[3] += color*(c2*x)*domega ;   /* Y_{11}  = 0.488603 x  */

    /* The Quadratic terms, L_{2m} -2 <= m <= 2 */

    /* First, L_{2-2}, L_{2-1}, L_{21} corresponding to xy,yz,xz */
    coeffs[4]+= color*(c3*x*y)*domega ; /* Y_{2-2} = 1.092548 xy */ 
    coeffs[5]+= color*(c3*y*z)*domega ; /* Y_{2-1} = 1.092548 yz */ 
    coeffs[7]+= color*(c3*x*z)*domega ; /* Y_{21}  = 1.092548 xz */ 

    /* L_{20}.  Note that Y_{20} = 0.315392 (3z^2 - 1) */
    coeffs[6] += color*(c4*(3*z*z-1))*domega ; 

    /* L_{22}.  Note that Y_{22} = 0.546274 (x^2 - y^2) */
    coeffs[8] += color*(c5*(x*x-y*y))*domega ;



}

in vec3 v_FragPos;
in vec2 TexCoords;
//out vec4 FragColor;

uniform samplerCube u_EnvironmentMap;
uniform float u_MapSize;
const float PI = 3.14159265359;
uniform int u_FaceIdx;
//binding = 1 ¶ÔÓ¦ GL_TEXTURE1
layout(rgba32f, binding=1) uniform image2D u_Image;

vec3 GetFaceColor(int faceNum,vec2 mapTexCoords){
		vec3 color;
		switch(faceNum){
			case 0://posx
				color= texture(u_EnvironmentMap,vec3(1.0,mapTexCoords.xy)).rgb;
				break;
			case 1://negx
				color= texture(u_EnvironmentMap,vec3(-1.0,mapTexCoords.xy)).rgb;
				break;
			case 2://posy
				color= texture(u_EnvironmentMap,vec3(mapTexCoords.x,1.0,mapTexCoords.y)).rgb;
				break;
			case 3://negy
				color= texture(u_EnvironmentMap,vec3(mapTexCoords.x,-1.0,mapTexCoords.y)).rgb;
				break;
			case 4://posz
				color= texture(u_EnvironmentMap,vec3(mapTexCoords.xy,1.0)).rgb;
				break;
			case 5://negz
				color= texture(u_EnvironmentMap,vec3(mapTexCoords.xy,-1.0)).rgb;
				break;

		}

		return color;
}

float sinc(float x) {               /* Supporting sinc function */
  if (abs(x) < 1.0e-4) return 1.0 ;
  else return(sin(x)/x) ;
}

void main(){
vec3 coeffs[9];
	/*initial coeffs[9]*/
	for(int i= 0;i<9;i++){
		coeffs[i]= vec3(0.0,0.0,0.0);
	}
		//posx
	vec2 mapTexCoords = 2.0*TexCoords-1.0;

	//for(int i=0;i<6;i++){
		vec3 color = GetFaceColor(u_FaceIdx,mapTexCoords);

		float u = mapTexCoords.x;
		float v = mapTexCoords.y;
		float r = sqrt(u*u+v*v);
		float theta = PI*r;
		float phi = atan(v,u);

		float x = sin(theta)*cos(phi);
		float y = sin(theta)*sin(phi);
		float z = cos(theta);


		float domega = (2.0*PI/u_MapSize)*(2.0*PI/u_MapSize)*sinc(theta);

		updateCoeffs(coeffs,color,domega,x,y,z);
		
	//}
	for(int i= 0;i<9;i++){
		vec4 origin= imageLoad(u_Image,ivec2(i,0));	
		vec3 biasCoeffs = coeffs[i]+vec3(0.5*10.0/(u_MapSize*u_MapSize));
		biasCoeffs/=10.0;
		imageStore(u_Image,ivec2(i,0),origin+vec4(biasCoeffs,0.0));
//		if((origin+vec4(biasCoeffs,0.0)).x<0.0||(origin+vec4(biasCoeffs,0.0)).y<0.0||(origin+vec4(biasCoeffs,0.0)).z<0.0)
//			imageStore(u_Image,ivec2(i,0),vec4(1.0,0.0,0.0,1.0));
//		else
//			imageStore(u_Image,ivec2(i,0),vec4(0.0,1.0,0.0,1.0));

//	if(color.x<1.0&&color.y<1.0&&color.z<1.0)
//			imageStore(u_Image,ivec2(i,0),vec4(1.0,0.0,0.0,1.0));
//		else
//			imageStore(u_Image,ivec2(i,0),vec4(0.0,1.0,0.0,1.0));
//	}
		
	}
		



}
