#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main(){
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos,1.0);

}

#type fragment
#version 430 core

#define ObjectNum 2
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

const float FLTMAX = 99999999999999999999999999999999999999.0;
in vec2 TexCoords;
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
float Materials[2*3]={
	//color
	0.2,1.0,0.2,
	1.0,0.2,0.5
};
float Scene[ObjectNum*6] = {
	//type  //matId   //pos          //radius
	0,      0,        0,0,-5.0,       20.0,
	0,      0,        1.0,0, -20.0,     30.0
};
uniform vec3 u_CameraUp;
uniform vec3 u_CameraFront;
uniform vec3 u_CameraRight;

uniform vec3 u_CameraViewPos;
uniform vec2 u_Screen;
uniform float u_rayNumMax;
uniform float u_rdSeed[4];
int rdCnt = 0;
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
	else;
	//out_color_time.xyz = gRay.color;
	out_origin_curRayNum = vec4(gRay.origin, gRay.curRayNum);
	out_dir_tMax = vec4(gRay.dir, gRay.tMax);
	out_color_time = vec4(gRay.color, 0.0);
	out_rayTracingRst = color;

}

void GenRay(){
	gRay.origin = u_CameraViewPos;
	gRay.dir = u_CameraViewPos+1.0*(u_CameraFront)+TexCoords.y*u_Screen.y*0.5*(u_CameraUp)+TexCoords.x*u_Screen.x*0.5*(u_CameraRight)-gRay.origin;
	gRay.tMax = FLTMAX;
	gRay.color = vec3(1);
}

void SetRay(){
	float curRayNum = texture(origin_curRayNum,TexCoords).w;
	if(curRayNum>=u_rayNumMax) return;
	vec4 val_dir_tMax = texture(dir_tMax,TexCoords);
	if(val_dir_tMax.w == 0) GenRay();
	else{
		gRay.origin    = texture(origin_curRayNum,TexCoords).xyz;
		gRay.curRayNum = texture(origin_curRayNum,TexCoords).w;
		gRay.dir       = texture(dir_tMax,TexCoords).xyz;
		gRay.tMax      = texture(dir_tMax,TexCoords).w;
		gRay.color     = texture(color_time,TexCoords).xyz;

	}

}
 void  JudgeHit(out struct Hitable hitable,int objId){
	int matIdx = int(Scene[objId*6+1]);
	int objType = int(Scene[objId*6]);
	float radius = Scene[objId*6+5];
	if(objType ==0){//sphere
		vec3 center = vec3(Scene[objId*6+2],Scene[objId*6+3],Scene[objId*6+4]);
		vec3 oc = gRay.origin-center;
		float c = dot(oc,oc)-radius*radius;
		float b =2.0* dot(oc,gRay.dir);
		float a = dot(gRay.dir,gRay.dir);
		float delta = b*b-4.0*a*c;
		if(delta<0){
			hitable.hit = false;
		}
		else if(delta >=0){
			float t = (-b-sqrt(delta))/(2.0*a);

			if(t<0||t>gRay.tMax)return;
			t = (-b+sqrt(delta))/(2.0*a);
			if(t<0||t>gRay.tMax)return;
			hitable.hit = true;

			//float t0 = (-b+sqrt(delta))/(2.0*a);
			//float t1 = (-b-sqrt(delta))/(2.0*a);

			gRay.tMax = t;
			hitable.pos = gRay.origin+hitable.dist*gRay.dir;
			hitable.objId = objId;
			hitable.normal = (hitable.pos-center)/radius;

			hitable.matIdx = matIdx;
		}
		
	}
	
}
bool Scatter_Lambertian(struct Hitable hitable, int matIdx){
	vec3 albedo = vec3(Materials[matIdx*3+1], Materials[matIdx*3+2], Materials[matIdx*3+3]);

	gRay.dir = hitable.normal + RandInSphere();
	gRay.origin = hitable.pos;
	gRay.color = albedo;
	gRay.tMax = FLTMAX;
	return true;
}
void RayTrace(){
	SetRay();
	if(gRay.curRayNum>=u_rayNumMax) {
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
		JudgeHit(hitable,i);
		if(hitable.hit){
			//if(hitable.dist<finalHitable.dist)
				finalHitable = hitable;
		}
		


	}//for
	int hitMatId;
	vec3 hitColor;
	if(finalHitable.hit){
		hitMatId = finalHitable.matIdx;
		hitColor = vec3(Materials[hitMatId*3],Materials[hitMatId*3+1],Materials[hitMatId*3+2]);
		
		Scatter_Lambertian(finalHitable,hitMatId);
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