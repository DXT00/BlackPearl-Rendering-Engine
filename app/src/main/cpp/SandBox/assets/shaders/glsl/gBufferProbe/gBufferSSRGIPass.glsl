#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

/* draw quad */
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
out vec2 v_TexCoords;


void main()
{
	v_TexCoords = aTexCoords;

	gl_Position =vec4(aPos,1.0);
}

#type fragment
#version 430 core

in vec2 v_TexCoords;

//layout (location = 1) out vec4 hitPosition;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse_Roughness;
uniform sampler2D gSpecular_Mentallic;
uniform sampler2D gAmbientGI_AO;
uniform sampler2D gNormalMap;
uniform sampler2D depthTexture;


out vec4 FragColor;
uniform vec2 gScreenSize;
uniform Settings u_Settings;
uniform mat4 u_ProjectionView;
uniform vec3 u_CameraViewPos;

const int SAMPLE_NUM = 20;
#define TWO_PI 6.283185307
#define INV_PI 0.31830988618
#define INV_TWO_PI 0.15915494309


vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}


float InitRand(vec2 uv) {
	vec3 p3  = fract(vec3(uv.xyx) * .1031);
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.x + p3.y) * p3.z);
}

vec3 GetGbufferNormal(vec2 uv)
{

	float isPBRObject = texture(gAmbientGI_AO,uv).a;
	if(isPBRObject == 0.0){
		return texture(gNormal,uv).rgb;
	}else{
	   return texture(gNormalMap,uv).rgb;

	}
}
vec3 GetGbufferDiffuse(vec2 uv)
{
	return texture(gDiffuse_Roughness,uv).rgb;
}

vec3 GetGbufferPos(vec2 uv)
{
	return texture(gPosition,uv).rgb;
}


/*
取得两个切线向量，然后组成TBN矩阵把这个局部向量转换成世界坐标下的方向向量，也就是我们世界空间下的步进方向
*/
void LocalBasis(vec3 n, out vec3 b1, out vec3 b2) {
  float sign_ = sign(n.z);
  if (n.z == 0.0) {
    sign_ = 1.0;
  }
  float a = -1.0 / (sign_ + n.z);
  float b = n.x * n.y * a;
  b1 = vec3(1.0 + sign_ * n.x * n.x * a, sign_ * b, -sign_ * n.x);
  b2 = vec3(b, sign_ + n.y * n.y * a, -n.y);
}

float Rand1(inout float p) {
  p = fract(p * .1031);
  p *= p + 33.33;
  p *= p + p;
  return fract(p);
}


vec2 Rand2(inout float p) {
  return vec2(Rand1(p), Rand1(p));
}


//均匀采样上半球 https://zhuanlan.zhihu.com/p/360420413
vec3 SampleHemisphereUniform(inout float s, out float pdf) {
  vec2 uv = Rand2(s);
  float z = uv.x;
  float phi = uv.y * TWO_PI;
  float sinTheta = sqrt(1.0 - z*z);
  vec3 dir = vec3(sinTheta * cos(phi), sinTheta * sin(phi), z);
  pdf = INV_TWO_PI;
  return dir;
}


//按cos值加权来采样上半球 https://zhuanlan.zhihu.com/p/360420413
vec3 SampleHemisphereCos(vec2 xy, out float pdf){
	float s = InitRand(xy);
	vec2 uv = Rand2(s);
	float z = sqrt(1.0 - uv.x);
	float phi = uv.y * TWO_PI;
	float sinTheta = sqrt(uv.x);
	vec3 dir = vec3(sinTheta * cos(phi), sinTheta * sin(phi), z);
	pdf = z * INV_PI;
	return dir;
}

vec3 TransformWorldToScreenSpace(vec3 pos){
	vec4 glPos = u_ProjectionView * vec4(pos,1.0);
	vec4 NDCPos = glPos/glPos.w;
	vec3 ret = (NDCPos.xyz + vec3(1.0))*0.5;
	return ret;
}


vec3 EvalDiffuse(vec3 wi, vec2 uv) {
  vec3 albedo  = GetGbufferDiffuse(uv);
  vec3 normal = GetGbufferNormal(uv);
  float cos = max(0., dot(normal, wi));
  return albedo * cos * INV_PI;
}

float GetDepth(vec3 posWorld) {
  float depth = (u_ProjectionView * vec4(posWorld, 1.0)).w;
  return depth;
}

bool RayMarch(vec3 origin, vec3 dir, inout vec3 hitPos, vec3 view, inout vec4 hitPosition) {
	
	float step = 1;
	float t = 1 * step;
	const int totalStepTimes = 500; 
	float curStepCnt = 0;
	if(dir.z>0)
		return false;
	//if(dot(dir,view)<0)
	//	return false;
	while(curStepCnt < totalStepTimes){
	
		vec3 p = origin + t * dir;
		vec3 uvw = TransformWorldToScreenSpace(p);
		float depth = texture(depthTexture, uvw.xy).r;
		float curDepth = GetDepth(p);
		if(curDepth-depth>0.0001){
			hitPos = p;
			hitPosition = vec4(curDepth,0,0,0);
			return true;
		}
		t += step;
		curStepCnt++;

	}
	hitPosition = vec4(0);
	return false;

}

//vec4 CalculateSSR(){
//	vec2 uv = CalcTexCoord();
//	vec3 L_ind = vec3(0);
//	vec3 worldPos = GetGbufferPos(uv);
//
//	vec3 view = normalize(u_CameraViewPos - worldPos);
//	vec4 hitpos = vec4(0);
//	for(int i = 0; i < SAMPLE_NUM; i++){
//		float pdf;
//		vec3 RandomHemisphereDir = SampleHemisphereCos(uv, pdf);
//		vec3 normal = GetGbufferNormal(uv);
//		vec3 b1, b2;
//		LocalBasis(normal, b1, b2);
//		vec3 dir = normalize(mat3(b1, b2, normal) * RandomHemisphereDir);
//
//		vec3 position_1;
//		
//		if(RayMarch(worldPos, dir, position_1, view, hitpos)){
//			vec2 hitScreenUV = TransformWorldToScreenSpace(position_1).xy;
//			L_ind += pdf * EvalDiffuse(dir, hitScreenUV);
//			//L_ind += pdf * EvalDiffuse(dir, hitScreenUV) * EvalDirectionalLight(hitScreenUV);
//		}
//
//	}
//
//	//L_ind /= float(SAMPLE_NUM);
//	//vec3 color = pow(clamp(L_ind, vec3(0.0), vec3(1.0)), vec3(1.0 / 2.2));
//	//return color;
//	return hitpos;
//}

vec4 CalculateSSRReflect(){
	vec2 uv = CalcTexCoord();
	vec3 L_d = vec3(0);
	vec3 worldPos = GetGbufferPos(uv);
	vec3 worldNormal = GetGbufferNormal(uv);

	vec3 view = normalize(u_CameraViewPos - worldPos);
	float pdf;
	vec3 RandomHemisphereDir = SampleHemisphereCos(uv, pdf);
	vec3 normal = GetGbufferNormal(uv);
	
	vec3 reflectDir = normalize(reflect(-view, worldNormal));//normalize(mat3(b1, b2, normal) * RandomHemisphereDir);

	vec3 position_1;
	vec4 hitpos;
	if(RayMarch(worldPos, reflectDir, position_1, view, hitpos)){
		vec2 hitScreenUV = TransformWorldToScreenSpace(position_1).xy;
		L_d +=  texture(gDiffuse_Roughness,uv).rgb * texture(gDiffuse_Roughness,hitScreenUV).rgb;//EvalDiffuse(reflectDir, hitScreenUV);
	}else{
	   return vec4(0);
	}
	//return hitpos;
	

	vec3 color = L_d;//pow(clamp(L_d, vec3(0.0), vec3(1.0)), vec3(1.0 / 2.2));
	return vec4(color,1.0);
}

void main(){
	vec2 texCoords = CalcTexCoord();
	vec4 outColor = u_Settings.SSRGICoeffs * CalculateSSRReflect();
	FragColor = vec4(outColor.rgb,1.0);
}