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

const float HitableType_Sphere = 0.0;
const float MatType_Lambertian = 0.0;
const float MatType_Metal = 1.0;
const float MatType_Dielectric = 2.0;

struct Ray{
	vec3 origin;
	vec3 dir;
	float tMax;
	highp float curRayNum;
	//vec3 rtxRst;
	vec3 color;
}gRay;

struct Hitable{
	bool hit;
	vec3 pos;
	vec3 dir;
	float dist;
	int objId;
	vec3 normal;
	int matIdx;


};

const float Materials[15] = {
	MatType_Lambertian,	0.5, 0.5, 0.5,//0
	MatType_Lambertian,	0.8, 0.8, 0.0,//4
	MatType_Metal,		0.1, 0.2, 0.5, 0.0,//8
	MatType_Dielectric,	1.5//13
};

const float Scene[30] = {
	//type                  //matId   //pos          //radius
	HitableType_Sphere,	13, -1,      0, -1,   0.5,
	HitableType_Sphere,	13, -1,      0, -1, -0.45,
	HitableType_Sphere,	 4,  0,      0, -1,   0.5,
	HitableType_Sphere,  8,  1,      0, -1,   0.5,
	HitableType_Sphere,  0,  0, -100.5, -1,   100
};

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
vec3 RandInSphere();
void WriteRay(int mode);
void GenRay();
struct Hitable JudgeHit(int objId);
bool Scatter_Lambertian(struct Hitable hitable, int matIdx);
bool Scatter_Dielectric(struct Hitable hitable, int matIdx);
bool Scatter_Metal(struct Hitable hitable, int matIdx);
float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi);
void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation);
void RayTrace();

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
struct Hitable JudgeHit(int objId){
	struct Hitable hitRst;
	hitRst.hit = false;

	int matIdx = int(Scene[objId*6+1]);
	int objType = int(Scene[objId*6]);
	float radius = Scene[objId*6+5];
	if(objType ==0){//sphere
		vec3 center = vec3(Scene[objId*6+2],Scene[objId*6+3],Scene[objId*6+4]);
		vec3 oc = gRay.origin-center;
		float c = dot(oc,oc)-radius*radius;
		float b = dot(oc,gRay.dir);
		float a = dot(gRay.dir,gRay.dir);
		float delta = b*b-a*c;
		if(delta<=0){
			return hitRst;
		}
		else{
			float t = (-b-sqrt(delta))/a;

			if(t < tMin || t> gRay.tMax){
				t = (-b + sqrt(delta)) /a;
				if(t < tMin || t>gRay.tMax)
					return hitRst;

			}
			
			hitRst.hit = true;

			gRay.tMax = t;
			hitRst.pos = gRay.origin+t*gRay.dir;
			hitRst.objId = objId;
			hitRst.normal = (hitRst.pos-center)/radius;
			hitRst.matIdx = matIdx;
			return hitRst;
		}
		
	}
	
}

bool Scatter_Material(struct Hitable hitable, int matIdx){
	float matType = Materials[matIdx];
	if(matType == MatType_Lambertian)
		return Scatter_Lambertian(hitable,matIdx);
	else if (matType == MatType_Dielectric)
		return Scatter_Dielectric(hitable,matIdx);
	else if(matType == MatType_Metal)
		return Scatter_Metal(hitable,matIdx);
	else{
		gRay.color = vec3(1,0,0);//以此提示材质存在问题
		return false;
	}
}
bool Scatter_Metal(struct Hitable hitable, int matIdx){
	vec3 specular = vec3(Materials[matIdx], Materials[matIdx + 1], Materials[matIdx+2]);
	float fuzz = Materials[matIdx+4];

	vec3 dir = reflect(gRay.dir, hitable.normal);
	vec3 dirFuzz = dir + fuzz * RandInSphere();

	// 反射光线在表面之下
	if (dot(dirFuzz, hitable.normal) < 0) {
		gRay.color = vec3(0);
		return false;
	}

	Ray_Update(hitable.pos, dirFuzz, specular);
	return true;

}

bool Scatter_Dielectric(struct Hitable hitable, int matIdx){
	float refractIndex = Materials[matIdx+1];

	vec3 refractDir;
	vec3 reflectDir = reflect(gRay.dir, hitable.normal);

	vec3 ud = normalize(gRay.dir);
	vec3 un = normalize(hitable.normal);
	vec3 airViewDir;
	if (dot(ud,un) < 0) {//外部
		refractDir = refract(ud, un, 1.0f / refractIndex);
		airViewDir = -ud;
	}
	else {//内部
		refractDir = refract(ud, -un, refractIndex);
		if (refractDir == vec3(0)) {
			Ray_Update(hitable.pos, reflectDir, vec3(1));
			return true;
		}
		
		airViewDir = refractDir;
	}
	
	float fresnelFactor = FresnelSchlick(airViewDir, un, refractIndex);
	vec3 dir = Rand() > fresnelFactor ? refractDir : reflectDir;
	Ray_Update(hitable.pos, dir, vec3(1));
	return true;


}

void Ray_Update(vec3 origin, vec3 dir, vec3 attenuation){
	gRay.origin = origin;
	gRay.dir = dir;
	gRay.color *= attenuation;
	gRay.tMax = FLTMAX;
}


bool Scatter_Lambertian(struct Hitable hitable, int matIdx){
	vec3 albedo = vec3(Materials[matIdx], Materials[matIdx + 1], Materials[matIdx+2]);
	Ray_Update( hitable.pos, hitable.normal + RandInSphere(),albedo);
//	gRay.dir = hitable.normal + RandInSphere();
//	gRay.origin = hitable.pos;
//	gRay.color *= albedo;
//	gRay.tMax = FLTMAX;
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
	struct Hitable finalHitable;
	finalHitable.hit = false;
	for(int i=0;i<ObjectNum;i++){
		struct Hitable hitable;
		hitable.hit = false;
		hitable = JudgeHit(i);
		if(hitable.hit){
			finalHitable = hitable;
		}
	}//for
	int hitMatId=0;
	if(finalHitable.hit){
		hitMatId = finalHitable.matIdx;
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

void main(){
	RayTrace();

}