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

layout(location = 0) out vec4 out_dir_tMax;
layout(location = 1) out vec4 out_SumColor;
layout(location = 2) out vec4 out_color_time;
layout(location = 3) out vec3 out_rayTracingRst;

const vec3 skyColor = vec3(0.5,0.5,1.0);

uniform sampler2D SumColor;
uniform sampler2D dir_tMax;
uniform sampler2D color_time;
uniform sampler2D RTXRst;
const float rayP = 20.0/21.0;// depth = p/(1-p) --> p = depth/(depth+1)
const float tMin = 0.001;
const float PI=3.14159;
const float FLTMAX = 99999999999999999999999999999999999999.0;
in vec2 TexCoords;

const float HitableType_Group = 0.0;
const float HitableType_BVH = 1.0;
const float HitableType_Volume = 2.0;
const float HitableType_Sphere = 3.0;
const float HitableType_TriMesh = 4.0;
const float HitableType_Triangle = 5.0;
const float HitableType_Transform = 6.0;


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
};

struct Vertex{
	vec3 pos;
	vec3 normal;
	vec2 uv;
};
struct HitRst{
	bool hit;
	struct Vertex vertex;
	float matIdx;
	float isMatCoverable;
};

uniform sampler2D u_SceneData;
uniform sampler2D u_MatData;
uniform sampler2D u_TexData;



uniform vec3 u_CameraUp;
uniform vec3 u_CameraFront;
uniform vec3 u_CameraRight;
uniform float u_CameraFov;

uniform vec3 u_CameraViewPos;
uniform vec2 u_Screen;
uniform float u_rdSeed[4];
int rdCnt = 0;

float RandXY(float x, float y);// [0.0, 1.0)
float Rand();// [0.0, 1.0)
vec2 RandInSquare();
vec2 RandInCircle();
vec3 RandInSphere();
float At(sampler2D data, float idx);
void WriteRay(int mode);
void GenRay(out struct Ray ray);
bool Scatter_Lambertian(inout struct Ray ray, struct HitRst hitable, int matIdx);
bool Scatter_Dielectric(inout struct Ray ray, struct HitRst hitable, int matIdx);
bool Scatter_Metal(inout struct Ray ray, struct HitRst hitable, int matIdx);
float FresnelSchlick(vec3 viewDir, vec3 halfway, float ratioNtNi);
void Ray_Update(inout struct Ray ray,vec3 origin, vec3 dir, vec3 attenuation);
vec3 RayTrace();
void Vertex_Load(float idx, out struct Vertex vertex);
void Camera_GenRay();



struct Vertex Vertex_InValid = struct Vertex(vec3(0),vec3(0),vec2(0));


struct HitRst Hitrst_InVaild = struct HitRst(false,Vertex_InValid,-1,0);

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




vec4 Intersect_RayTri(vec3 e, vec3 d, vec3 a, vec3 b, vec3 c){
	mat3 equation_A = mat3(vec3(a-b), vec3(a-c), d);

	//平行
	if (abs(determinant(equation_A)) < 0.00001)
		return vec4(0);

	vec3 equation_b = a - e;
	vec3 equation_X = inverse(equation_A) * equation_b;
	float alpha = 1 - equation_X[0] - equation_X[1];
	return vec4(alpha, equation_X);
}

void Vertex_Interpolate(vec3 abg, struct Vertex A, struct Vertex B, struct Vertex C, out struct Vertex vert){
	vert.uv[0] = dot(abg, vec3(A.uv[0], B.uv[0], C.uv[0]));
	vert.uv[1] = dot(abg, vec3(A.uv[1], B.uv[1], C.uv[1]));
	vert.pos = abg[0] * A.pos + abg[1] * B.pos + abg[2] * C.pos;
	vert.normal = abg[0] * A.normal + abg[1] * B.normal + abg[2] * C.normal;
}

void RayIn_Triangle(float idx, inout struct Ray ray,inout struct HitRst hitRst){
	struct Vertex A, B, C;
	float vertexABC_pack4_idx = At(SceneData, idx+3);
	Vertex_Load(vertexABC_pack4_idx  , A);
	Vertex_Load(vertexABC_pack4_idx+2, B);
	Vertex_Load(vertexABC_pack4_idx+4, C);

	vec4 abgt = Intersect_RayTri(ray.origin, ray.dir, A.pos, B.pos, C.pos);
	if (abgt == vec4(0) ||
		any(lessThan(abgt,vec4(0,0,0,tMin))) ||
		any(greaterThan(abgt,vec4(1,1,1,ray.tMax)))
		)
		return;

	hitRst.hit = true;
	Vertex_Interpolate(abgt.xyz, A, B, C, hitRst.vertex);
	float matIdx = At(SceneData, idx+1);
	float isMatCoverable = At(SceneData, idx+2);
	hitRst.matIdx = matIdx;
	hitRst.isMatCoverable = isMatCoverable;
	ray.tMax = abgt[3];
	
}



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
 


void GenRay(out struct Ray ray){
	vec2 st = TexCoords + RandInSquare() / textureSize(SumColor, 0);
	ray.origin = u_CameraViewPos;
	float focus_dist=1.0;
	float height = 2.0 * focus_dist * tan(u_CameraFov/180.0*PI*0.5);
	float radio = u_Screen.x / u_Screen.y;
	float width = radio * height;
	vec3 BL_Corner = focus_dist*(normalize(u_CameraFront)) - height*0.5*normalize(u_CameraUp) - width*0.5*normalize(u_CameraRight);

	ray.dir = u_CameraViewPos + BL_Corner + st.s*width*(normalize(u_CameraRight)) + st.t*height*normalize(u_CameraUp) - ray.origin;
	ray.dir=normalize(ray.dir);
	ray.tMax = FLTMAX;
	ray.color = vec3(1);

}

bool Scatter_Material(inout struct Ray ray, struct HitRst hitable, int matIdx){
	if(matIdx == -1){
		ray.color = vec3(0,1.0,1);
		return false;
	}
	float matType = At(MatData,matIdx);

	if(matType == MatType_Lambertian)
		return Scatter_Lambertian(ray,hitable,matIdx);
	else if (matType == MatType_Dielectric)
		return Scatter_Dielectric(ray, hitable,matIdx);
	else if(matType == MatType_Metal || matType ==MatType_Speculer)
		return Scatter_Metal(ray,hitable,matIdx);
	else{
		ray.color = vec3(1,0,0);//以此提示材质存在问题
		return false;
	}
}
bool Scatter_Metal(inout struct Ray ray, struct HitRst hitable, int matIdx){
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

	vec3 dir = reflect(ray.dir, hitable.vertex.normal);
	vec3 dirFuzz = dir + fuzz * RandInSphere();

	// 反射光线在表面之下
	if (dot(dirFuzz, hitable.vertex.normal) < 0) {
		ray.color = vec3(0);
		return false;
	}

	Ray_Update(ray, hitable.vertex.pos, dirFuzz, specular);
	return true;

}

bool Scatter_Dielectric(inout struct Ray ray, struct HitRst hitable, int matIdx){
	float refractIndex = At(MatData,matIdx+2);
	vec3 refractDir;
	vec3 reflectDir = reflect(ray.dir, hitable.vertex.normal);

	vec3 ud = normalize(ray.dir);
	vec3 un = normalize(hitable.vertex.normal);
	vec3 airViewDir;
	if (dot(ud,un) < 0) {//外部
		refractDir = refract(ud, un, 1.0f / refractIndex);
		airViewDir = -ud;
	}
	else {//内部
		refractDir = refract(ud, -un, refractIndex);
		if (refractDir == vec3(0)) {
			Ray_Update(ray, hitable.vertex.pos, reflectDir, vec3(1));
			return true;
		}
		
		airViewDir = refractDir;
	}
	
	float fresnelFactor = FresnelSchlick(airViewDir, un, refractIndex);
	vec3 dir = Rand() > fresnelFactor ? refractDir : reflectDir;
	Ray_Update(ray,hitable.vertex.pos, dir, vec3(1));
	return true;


}
float At(sampler2D data, float idx){
//	vec2 texCoords = vec2((idx+0.5)/textureSize(data, 0).x, 0.5);
//    return texture2D(data, texCoords).x;

	float row = (idx+0.5)/textureSize(data, 0).x;
	float y = (int(row)+0.5)/textureSize(data, 0).y;
	float x = (idx + 0.5 - int(row) * textureSize(data, 0).x)/textureSize(data, 0).x;
    vec2 texCoords = vec2(x, y);
    return texture2D(data, texCoords).x;
}


void Ray_Update(inout struct Ray ray, vec3 origin, vec3 dir, vec3 attenuation){
	ray.origin = origin;
	ray.dir = dir;
	ray.color *= attenuation;
	ray.tMax = FLTMAX;
}

void GetPackData(float idx, out vec4 pack){
	float row = (idx+0.5)/textureSize(PackData, 0).x;
	float y = (int(row)+0.5)/textureSize(PackData, 0).y;
	float x = (idx + 0.5 - int(row) * textureSize(PackData, 0).x)/textureSize(PackData, 0).x;
    vec2 texCoords = vec2(x, y);
	pack = texture2D(PackData, texCoords);

}

void GetPackData(float idx, out vec3 pack){
	float row = (idx+0.5)/textureSize(PackData, 0).x;
	float y = (int(row)+0.5)/textureSize(PackData, 0).y;
	float x = (idx + 0.5 - int(row) * textureSize(PackData, 0).x)/textureSize(PackData, 0).x;
    vec2 texCoords = vec2(x, y);
	pack = texture2D(PackData, texCoords).xyz;

}
//mat4 is column major
//void GetPackData(float idx, out mat4 m){
//	GetPackData(idx,m[0]);
//	GetPackData(idx+4.0,m[1]);
//	GetPackData(idx+8.0,m[2]);
//	GetPackData(idx+12.0,m[3]);
//}
//void GetPackData(float idx, out mat3 m){
//	vec4 v[3];
//	GetPackData(idx,v[0]);
//	GetPackData(idx+4.0,v[1]);
//	GetPackData(idx+8.0,v[2]);
//
//	m[0] = v[0].xyz;
//	m[1] = v[1].xyz;
//	m[2] = v[2].xyz;
//}
void GetPackData(float idx, out mat4 m){
	GetPackData(idx,m[0]);
	GetPackData(idx+1.0,m[1]);
	GetPackData(idx+2.0,m[2]);
	GetPackData(idx+3.0,m[3]);
}
void GetPackData(float idx, out mat3 m){
	vec4 v[3];
	GetPackData(idx,v[0]);
	GetPackData(idx+1.0,v[1]);
	GetPackData(idx+2.0,v[2]);

	m[0] = v[0].xyz;
	m[1] = v[1].xyz;
	m[2] = v[2].xyz;
}
void Vertex_Load(float idx, out struct Vertex vert){
	vec4 pos_u, normal_v;
	GetPackData(idx, pos_u);
	GetPackData((idx+1), normal_v);

	vert.pos    = pos_u.xyz;
	vert.normal = normal_v.xyz;
	vert.uv     = vec2(pos_u[3], normal_v[3]);

}

bool AABB_Hit(struct Ray ray,float idx){
	float AABB_pack4_idx = At(SceneData, idx);
	vec4 v4_min, v4_max;
	vec3 minP, maxP;
	GetPackData(AABB_pack4_idx,minP);
	GetPackData(AABB_pack4_idx+1.0,maxP);

	vec3 origin = ray.origin;
	vec3 dir = ray.dir;
	float local_tMin = tMin;
	float tMax = ray.tMax;
	for (int i = 0; i < 3; i++) {
		float invD = 1.0f / dir[i];
		float t0 = (minP[i] - origin[i]) * invD;
		float t1 = (maxP[i] - origin[i]) * invD;
		if (invD < 0.0f){
			float tmp = t0;
			t0 = t1;
			t1 = tmp;
		}

		local_tMin = max(t0, local_tMin);
		tMax = min(t1, tMax);
		if (tMax <= local_tMin)
			return false;
	}
	return true;
}
//struct HitRst JudgeHitSphere(float idx){
void RayIn_Sphere(float idx, inout struct Ray ray, inout struct HitRst hitRst) {
	float has_material = At(SceneData, idx+2);
	float matIdx = At(SceneData, idx+1);
	float packIdx = At(SceneData, idx+3);
	vec4 center_r;
	GetPackData(packIdx,center_r);
    vec3 center = center_r.xyz;// vec3(At(PackData, packIdx), At(PackData, packIdx+1), At(PackData, packIdx+2));
    float radius = center_r.w;// At(PackData, packIdx+3);
    
    hitRst.hit = false;
    
    vec3 oc = ray.origin - center;
    float a = dot(ray.dir, ray.dir);
    float b = dot(oc, ray.dir);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    
    if (discriminant <= 0.0 || a == 0.0 || radius == 0.0)
        return;
    
    float t = (-b - sqrt(discriminant)) / a;
    if (t > ray.tMax || t < tMin) {
        t = (-b + sqrt(discriminant)) / a;
        if (t > ray.tMax || t < tMin)
            return;
    } 
    ray.tMax = t;
    hitRst.hit = true;
    hitRst.vertex.pos = ray.origin + t * ray.dir;
    hitRst.vertex.normal = (hitRst.vertex.pos - center) / radius;
  //  hitRst.vertex.uv = Sphere2UV(hitRst.vertex.normal);
    hitRst.matIdx = matIdx;
    

}
void Ray_Transform(inout struct Ray ray, mat4 transform){
	ray.dir = mat3(transform) * ray.dir;

	vec4 originQ = transform * vec4(ray.origin, 1.0f);
	ray.origin = originQ.xyz / originQ.w;
}

void Vertex_Transform(inout struct Vertex vert, mat4 transform , mat3 normTfm){
	vec4 posQ = transform * vec4(vert.pos, 1.0);
	vert.pos = posQ.xyz / posQ.w;
	vert.normal = normalize(normTfm * vert.normal);
}

struct HitRst TraceScene(inout struct Ray ray,inout struct HitRst finalHitRst){
	finalHitRst = Hitrst_InVaild;
	Stack_Push(3.0);
	while(!Stack_Empty()){
		float pIdx = Stack_Pop();
		float idx = At(SceneData, pIdx);

		if(idx <= 0.0){
			idx = -idx;
			float type = At(SceneData,idx);
			if(type == HitableType_Group || type == HitableType_BVH || type == HitableType_TriMesh){

				
				float matIdx = At(SceneData,idx+1.0);
				if(matIdx == -1.0)
					continue;

				float in_tMax = Stack_Pop();
				if (ray.tMax < in_tMax && finalHitRst.isMatCoverable == 1.0){
					finalHitRst.matIdx = matIdx;
					finalHitRst.isMatCoverable = At(SceneData, idx+2.0);
				}
			}
			else if (type == HitableType_Transform) {

				float in_tMax = Stack_Pop();// 进入节点时的tMax
				mat4 tfmMat4;
				GetPackData(At(SceneData,idx+3), tfmMat4);
				Ray_Transform(ray, tfmMat4);

				if(ray.tMax < in_tMax){
					mat3 normTfmMat3;
					GetPackData((At(SceneData,idx+3.0)+8.0), normTfmMat3);
					Vertex_Transform(finalHitRst.vertex, tfmMat4, normTfmMat3);

					if(finalHitRst.isMatCoverable == 1.0){
						float matIdx = At(SceneData, idx+1);
						if(matIdx != -1.0){
							finalHitRst.matIdx = matIdx;
							finalHitRst.isMatCoverable = At(SceneData, idx+2);
						}
					}
				}
			}
			continue;
		}

		Stack_Push(pIdx+1.0);
		float type = At(SceneData,idx);
		if(type == HitableType_Sphere){
			RayIn_Sphere(idx, ray, finalHitRst);
		}
		else if(type == HitableType_Group){
			float matIdx = At(SceneData,idx+1.0);
			if(matIdx != -1.0)
				Stack_Push(ray.tMax);
			Stack_Push(idx+3.0);
		}
		else if(type == HitableType_BVH || type == HitableType_TriMesh){

			if(AABB_Hit(ray,idx+3.0)){
				float matIdx = At(SceneData,idx+1.0);
				if(matIdx != -1.0)
					Stack_Push(ray.tMax);
				Stack_Push(idx+4.0);
			}
		}
		else if(type == HitableType_Transform){
			mat4 invMat;
			GetPackData((At(SceneData,idx+3.0)+4.0),invMat);
			Ray_Transform(ray,invMat);
			Stack_Push(ray.tMax);
			Stack_Push(idx+4.0);

		}
		else if(type == HitableType_Triangle){
			
			RayIn_Triangle(idx, ray, finalHitRst);
		}
		else if(type == HitableType_Volume){
			//RayIn_Volume(idx, ray, finalHitRst);
		}
	}
	return finalHitRst;
}

bool Scatter_Lambertian(inout struct Ray ray, struct HitRst hitable, int matIdx){
	float texture_idx = At(MatData,matIdx+1);

	float texture_type = At(TexData,texture_idx);
	vec3 albedo;
	if(texture_type == TexT_ConstTexture){
		float packDataIdx = At(TexData,texture_idx+1) *4.0;
		albedo = vec3(At(PackData,packDataIdx),At(PackData,packDataIdx+1),At(PackData,packDataIdx+2));
	}
	else if(texture_type == TexT_ImgTexture){

	}
	else{


	}
	Ray_Update(ray, hitable.vertex.pos, hitable.vertex.normal + RandInSphere(),albedo);

	return true;
}

vec3 RayTrace(){
	struct Ray ray;
	GenRay(ray);
	while(true){
		if( Rand() > rayP){
			return vec3(0);
		}
		//judge hit
		struct HitRst finalHitable;
		TraceScene(ray,finalHitable);
		int hitMatId = 0;

		

		if(finalHitable.hit){
			//return vec3(1,0,0);
			hitMatId = int(finalHitable.matIdx);
			bool rayOut = Scatter_Material(ray,finalHitable,hitMatId);
			if(!rayOut)
				return ray.color;
		}
		else{//sky
		
//			if(normalize(ray.dir).y==0.0)
							//return vec3(0,(1.0),0);
		    //else
				//return vec3(1,(1.0),0);
			//return vec3(0,normalize(ray.dir).y,0);
			//return vec3(1,(1.0),0);
			
			float t = 0.5 * (normalize(ray.dir).y + 1.0);
			vec3 white = vec3(1.0, 1.0, 1.0);
			vec3 blue = vec3(0.5, 0.7, 1.0);
			vec3 lightColor = (1.0 - t) * white + t * blue;
			ray.color *= lightColor;
			return ray.color;
			
		}
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
	vec3 color = RayTrace();// + texture(SumColor, TexCoords).xyz;
	out_SumColor = vec4(color.x,color.y,color.z,0.0);

}