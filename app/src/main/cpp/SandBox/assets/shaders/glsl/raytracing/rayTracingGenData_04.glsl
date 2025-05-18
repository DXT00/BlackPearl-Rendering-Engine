#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main(){
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos,1.0);

}

#type fragment
#version 430 core

#define ObjectNum 5
const vec3 skyColor = vec3(0.5,0.5,1.0);
layout(location = 0) out vec4 out_origin_curRayNum;
layout(location = 1) out vec4 out_dir_tMax;
layout(location = 2) out vec4 out_color_time;
layout(location = 3) out vec3 out_rayTracingRst;

uniform sampler2D origin_curRayNum;
uniform sampler2D dir_tMax;
uniform sampler2D color_time;
uniform sampler2D RTXRst;
const float rayP = 50.0/51.0;// depth = p/(1-p) --> p = depth/(depth+1)
const float tMin = 0.001;
const float PI=3.14159;
const float FLTMAX = 99999999999999999999999999999999999999.0;
in vec2 TexCoords;

const float HitableType_Group = 0.0;
const float HitableType_BVH = 1.0;
const float HitableType_Volume = 2.0;
const float HitableType_Sphere = 3.0;


const float MatType_Lambertian = 0.0;
const float MatType_Metal = 1.0;
const float MatType_Dielectric = 2.0;
const float MatType_Diffuse = 3.0;
const float MatType_Speculer = 4.0;
const float MatType_Emission = 5.0;
const float MatType_roughness = 6.0;
const float MatType_Ao = 7.0;


const float TexT_ConstTexture = 0.0f;
const float TexT_ImgTexture = 1.0f;
const float TexT_Skybox = 2.0f;

struct Ray{
	vec3 origin;
	vec3 dir;
	float tMax;
	highp float curRayNum;
	//vec3 rtxRst;
	vec3 color;
}gRay;

struct Vertex{
	vec3 pos;
	vec3 normal;
	vec2 uv;
};
struct HitRst{
	bool hit;
	struct Vertex vertex;
	float matIdx;
};


uniform sampler2D u_SceneData;
uniform sampler2D u_MatData;
uniform sampler2D u_TexData;

//const float Materials[15] = {
//	MatType_Lambertian,	0.5, 0.5, 0.5,//0
//	MatType_Lambertian,	0.8, 0.8, 0.0,//4
//	MatType_Metal,		0.1, 0.2, 0.5, 0.0,//8
//	MatType_Dielectric,	1.5//13
//};
//
//const float Scene[30] = {
//	//type                  //matId   //pos          //radius
//	HitableType_Sphere,	13, -1,      0, -1,   0.5,
//	HitableType_Sphere,	13, -1,      0, -1, -0.45,
//	HitableType_Sphere,	 4,  0,      0, -1,   0.5,
//	HitableType_Sphere,  8,  1,      0, -1,   0.5,
//	HitableType_Sphere,  0,  0, -100.5, -1,   100
//};

uniform vec3 u_CameraUp;
uniform vec3 u_CameraFront;
uniform vec3 u_CameraRight;
uniform float u_CameraFov;

uniform vec3 u_CameraViewPos;
uniform vec2 u_Screen;
uniform float u_rayNumMax;
uniform float u_rdSeed[4];
int rdCnt = 0;

float RandXY(float x, float y);// [0.0, 1.0)
float Rand();// [0.0, 1.0)
vec2 RandInSquare();
vec2 RandInCircle();
vec3 RandInSphere();
float At(sampler2D data, float idx);

void WriteRay(int mode);
void GenRay();
bool Scatter_Lambertian(struct HitRst hitable, int matIdx);
bool Scatter_Dielectric(struct HitRst hitable, int matIdx);
bool Scatter_Metal(struct HitRst hitable, int matIdx);
float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi);
void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation);
void RayTrace();

void Camera_GenRay();



struct Vertex Vertex_InValid = struct Vertex(vec3(0),vec3(0),vec2(0));


struct HitRst Hitrst_InVaild = struct HitRst(false,Vertex_InValid,-1);

uniform sampler2D SceneData;
uniform sampler2D MatData;
uniform sampler2D TexData;
uniform sampler2D PackData;

float _Stack[100];
int _Stack_mTop = -1;
bool Stack_Empty();
float Stack_Top();
void Stack_Push(float val);
float Stack_Pop();
void Stack_Acc();

float RandXY(float x, float y){
	return fract(cos(x * (12.9898) + y * (4.1414)) * 43758.5453);
}
float Rand(){
	float a = RandXY(TexCoords.x, u_rdSeed[0]);
    float b = RandXY(u_rdSeed[1], TexCoords.y);
    float c = RandXY(rdCnt++, u_rdSeed[2]);
    float d = RandXY(u_rdSeed[3], a);
    float e = RandXY(b, c);
    float f = RandXY(d, e);

    return f;
}
float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi){
	float cosTheta = dot(viewDir, halfway);
	float R0 = pow((ratioNtNi - 1) / (ratioNtNi + 1), 2);
	float R = R0 + (1 - R0)*pow(1 - cosTheta, 5);
	return R;
}

vec2 RandInSquare(){
	return vec2(Rand(), Rand());
}
vec3 RandInSphere(){
	vec3 rst;
	do {
		rst = vec3(Rand(), Rand(), Rand())*2.0f - 1.0f;
	} while (dot(rst, rst) >= 1.0);
	return rst;
}

vec2 RandInCircle(){
    vec2 rst;
    do {
        rst = vec2(Rand(), Rand())*2.0f - 1.0f;
    } while (dot(rst, rst) >= 1.0);
    return rst;
}
 
void WriteRay(int mode){
	vec3 color = texture(RTXRst, TexCoords).xyz;

	if(mode == 0){//没有命中
		gRay.tMax = 0;
		color *= gRay.curRayNum / (gRay.curRayNum + 1);
		gRay.curRayNum = min(gRay.curRayNum + 1, u_rayNumMax);
	}
	else if(mode ==1){//击中光源
		gRay.tMax = 0;
		color = (color * gRay.curRayNum + gRay.color) / (gRay.curRayNum + 1);
		gRay.curRayNum = min(gRay.curRayNum + 1, u_rayNumMax);

	}
	else if(mode ==3){
		gRay.curRayNum = u_rayNumMax+100;
	}
	else{
		;
	}
	//out_color_time.xyz = gRay.color;
	out_origin_curRayNum = vec4(gRay.origin, gRay.curRayNum);
	out_dir_tMax = vec4(gRay.dir, gRay.tMax);
	out_color_time = vec4(gRay.color, 0.0);
	out_rayTracingRst = color;

}

void GenRay(){
	vec2 st = TexCoords + RandInSquare() / textureSize(origin_curRayNum, 0);
	gRay.origin = u_CameraViewPos;
	float focus_dist=1.0;
	float height = 2.0 * focus_dist * tan(u_CameraFov/180.0*PI*0.5);
	float radio = u_Screen.x / u_Screen.y;
	float width = radio * height;
	vec3 BL_Corner = focus_dist*(normalize(u_CameraFront)) - height*0.5*normalize(u_CameraUp) - width*0.5*normalize(u_CameraRight);

	gRay.dir = u_CameraViewPos + BL_Corner + st.s*width*(normalize(u_CameraRight)) + st.t*height*normalize(u_CameraUp) - gRay.origin;
	gRay.dir=normalize(gRay.dir);
	gRay.tMax = FLTMAX;
	gRay.color = vec3(1);
}

void SetRay(){
	gRay.curRayNum = texture(origin_curRayNum,TexCoords).w;
	if(gRay.curRayNum >=u_rayNumMax) return;
	vec4 val_dir_tMax = texture(dir_tMax,TexCoords);
	if(val_dir_tMax.w == 0) GenRay();
	else{
		gRay.origin    = texture(origin_curRayNum,TexCoords).xyz;
		gRay.dir       = texture(dir_tMax,TexCoords).xyz;
		gRay.tMax      = texture(dir_tMax,TexCoords).w;
		gRay.color     = texture(color_time,TexCoords).xyz;

	}

}

bool Scatter_Material(struct HitRst hitable, int matIdx){
	float matType = At(MatData,matIdx);
	if(matType == MatType_Lambertian)
		return Scatter_Lambertian(hitable,matIdx);
	else if (matType == MatType_Dielectric)
		return Scatter_Dielectric(hitable,matIdx);
	else if(matType == MatType_Metal || matType ==MatType_Speculer)
		return Scatter_Metal(hitable,matIdx);
	else{
		gRay.color = vec3(1,0,0);//以此提示材质存在问题
		return false;
	}
}
bool Scatter_Metal(struct HitRst hitable, int matIdx){
	float texture_idx = At(MatData,matIdx+1);

	float texture_type = At(TexData,texture_idx);
	float packDataIdx;
	vec3 specular;
	if(texture_type == TexT_ConstTexture){
		packDataIdx = At(TexData,texture_idx+1) *4.0;
		specular = vec3(At(PackData,packDataIdx),At(PackData,packDataIdx+1),At(PackData,packDataIdx+2));
	}
	else if(texture_type ==TexT_ImgTexture){

	}
	else{

	}
	
	float fuzz = At(MatData,matIdx+2);

	vec3 dir = reflect(gRay.dir, hitable.vertex.normal);
	vec3 dirFuzz = dir + fuzz * RandInSphere();

	// 反射光线在表面之下
	if (dot(dirFuzz, hitable.vertex.normal) < 0) {
		gRay.color = vec3(0);
		return false;
	}

	Ray_Update(hitable.vertex.pos, dirFuzz, specular);
	return true;

}

bool Scatter_Dielectric(struct HitRst hitable, int matIdx){
	float refractIndex = At(MatData,matIdx+2);
	vec3 refractDir;
	vec3 reflectDir = reflect(gRay.dir, hitable.vertex.normal);

	vec3 ud = normalize(gRay.dir);
	vec3 un = normalize(hitable.vertex.normal);
	vec3 airViewDir;
	if (dot(ud,un) < 0) {//外部
		refractDir = refract(ud, un, 1.0f / refractIndex);
		airViewDir = -ud;
	}
	else {//内部
		refractDir = refract(ud, -un, refractIndex);
		if (refractDir == vec3(0)) {
			Ray_Update(hitable.vertex.pos, reflectDir, vec3(1));
			return true;
		}
		
		airViewDir = refractDir;
	}
	
	float fresnelFactor = FresnelSchlick(airViewDir, un, refractIndex);
	vec3 dir = Rand() > fresnelFactor ? refractDir : reflectDir;
	Ray_Update(hitable.vertex.pos, dir, vec3(1));
	return true;


}
float At(sampler2D data, float idx){
	vec2 texCoords = vec2((idx+0.5)/textureSize(data, 0).x, 0.5);
    return texture2D(data, texCoords).x;
}

struct HitRst JudgeHitSphere(float idx){
	float has_material = At(SceneData, idx+2);
	float matIdx = At(SceneData, idx+1);
	float packIdx = At(SceneData, idx+3) * 4.0;
    vec3 center = vec3(At(PackData, packIdx), At(PackData, packIdx+1), At(PackData, packIdx+2));
    float radius = At(PackData, packIdx+3);
    
    struct HitRst hitRst;
    hitRst.hit = false;
    
    vec3 oc = gRay.origin - center;
    float a = dot(gRay.dir, gRay.dir);
    float b = dot(oc, gRay.dir);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    
    if (discriminant <= 0 || a == 0 || radius == 0)
        return hitRst;
    
    float t = (-b - sqrt(discriminant)) / a;
    if (t > gRay.tMax || t < tMin) {
        t = (-b + sqrt(discriminant)) / a;
        if (t > gRay.tMax || t < tMin)
            return hitRst;
    } 
    gRay.tMax = t;
    hitRst.hit = true;
    hitRst.vertex.pos = gRay.origin + t * gRay.dir;
    hitRst.vertex.normal = (hitRst.vertex.pos - center) / radius;
  //  hitRst.vertex.uv = Sphere2UV(hitRst.vertex.normal);
    hitRst.matIdx = matIdx;
    
    return hitRst;



}
void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation){
	gRay.origin = origin;
	gRay.dir = dir;
	gRay.color *= attenuation;
	gRay.tMax = FLTMAX;
}
struct HitRst TraceScene(){
	struct HitRst finalHit = Hitrst_InVaild;
	Stack_Push(3);
	while(!Stack_Empty()){
		float s_val = Stack_Top();
		float obj_idx = At(SceneData,s_val);
		if(obj_idx == 0){
			Stack_Pop();
			continue;
		}
		float type = At(SceneData,obj_idx);
		if(type == HitableType_Sphere){
			HitRst cur_hit = JudgeHitSphere(obj_idx);
			if(cur_hit.hit){
				finalHit = cur_hit;
			}
			Stack_Acc();
		}
		else if(type == HitableType_Group){
			Stack_Push(obj_idx + 3);
		}
	}

	return finalHit;
}

bool Scatter_Lambertian(struct HitRst hitable, int matIdx){
	float texture_idx = At(MatData,matIdx+1);

	float texture_type = At(TexData,texture_idx);
	vec3 albedo;
	if(texture_type == TexT_ConstTexture){
		float packDataIdx = At(TexData,texture_idx+1) *4.0;
		albedo = vec3(At(PackData,packDataIdx),At(PackData,packDataIdx+1),At(PackData,packDataIdx+2));
	}
	else if(texture_type ==TexT_ImgTexture){

	}
	else{


	}
	Ray_Update(hitable.vertex.pos, hitable.vertex.normal + RandInSphere(),albedo);

	return true;
}

void RayTrace(){
	SetRay();
	if(gRay.curRayNum >= u_rayNumMax) {
		WriteRay(3);
		return;
	}
	if(Rand() > rayP){
		WriteRay(0);//最终没有击中光源
		return;
	}
	//judge hit
	struct HitRst finalHitable;

	finalHitable = TraceScene();
	int hitMatId=0;
	if(finalHitable.hit){
		hitMatId = int(finalHitable.matIdx);
		Scatter_Material(finalHitable,hitMatId);

		WriteRay(2);
	}
	else{//sky
		float t = 0.5f * (normalize(gRay.dir).y + 1.0f);
		vec3 white = vec3(1.0f, 1.0f, 1.0f);
		vec3 blue = vec3(0.5f, 0.7f, 1.0f);
		vec3 lightColor = (1 - t) * white + t * blue;
		gRay.color *= lightColor;
		WriteRay(1);
		
	}

}
bool Stack_Empty(){
	return _Stack_mTop == -1;
}
float Stack_Top(){
	return _Stack[_Stack_mTop];
}
void Stack_Push(float val){
	if( _Stack_mTop == 99)
		return;
	_Stack_mTop++;
	_Stack[_Stack_mTop] = val;
}
float Stack_Pop(){
	float val =_Stack[_Stack_mTop];
	_Stack_mTop--;
	return val;
}
void Stack_Acc(){
	_Stack[_Stack_mTop] += 1.0;
}

void main(){
	RayTrace();

}