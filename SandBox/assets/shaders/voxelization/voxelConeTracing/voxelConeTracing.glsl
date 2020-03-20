#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

out vec3 worldPositionFrag;
out vec3 normalFrag;

void main(){
	worldPositionFrag = vec3(u_Model*vec4(aPos,1));
	normalFrag = normalize(mat3(transpose(inverse(u_Model)))*aNormal);
	gl_Position = u_ProjectionView*vec4(worldPositionFrag,1);
}

#type fragment
#version 430 core


#define TSQRT2 2.828427
#define SQRT2 1.414213
#define ISQRT2 0.707106
// --------------------------------------
// Light (voxel) cone tracing settings.
// --------------------------------------
#define MIPMAP_HARDCAP 5.4f /* Too high mipmap levels => glitchiness, too low mipmap levels => sharpness. */
#define VOXEL_SIZE (1/256.0) /* Size of a voxel. 128x128x128 => 1/128 = 0.0078125. */
//#define SHADOWS 1 /* Shadow cone tracing. */
//#define DIFFUSE_INDIRECT_FACTOR 0.52f /* Just changes intensity of diffuse indirect lighting. */
// --------------------------------------
// Other lighting settings.
// --------------------------------------
#define SPECULAR_MODE 1 /* 0 == Blinn-Phong (halfway vector), 1 == reflection model. */
#define SPECULAR_FACTOR 4.0f /* Specular intensity tweaking factor. */
#define SPECULAR_POWER 65.0f /* Specular power in Blinn-Phong. */
#define DIRECT_LIGHT_INTENSITY 0.96f /* (direct) point light intensity factor. */
#define MAX_LIGHTS 5 /* Maximum number of lights supported. */

// Lighting attenuation factors. See the function "attenuate" (below) for more information.
#define DIST_FACTOR 1.1f /* Distance is multiplied by this when calculating attenuation. */
#define CONSTANT 1
#define LINEAR 0 /* Looks meh when using gamma correction. */
#define QUADRATIC 1

// Other settings.
#define GAMMA_CORRECTION 1 /* Whether to use gamma correction or not. */

// Basic point light.
struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct Material{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	sampler2D normal;
	sampler2D height;

	float diffuseReflectivity;
	float specularReflectivity;
	float transparency;
	float emissivity;
	float refractiveIndex;
	float specularDiffusion;

	float shininess;
	bool isBlinnLight;
	int  isTextureSample;//判断是否使用texture,或者只有color

};

struct Settings{
	bool indirectSpecularLight;
	bool indirectDiffuseLight;
	bool directLight;
	bool shadows;
};

uniform Material u_Material;
uniform Settings u_Settings;
uniform vec3 u_CameraViewPos;
uniform int u_PointLightNums;
uniform PointLight u_PointLights[5];
uniform int u_State;
uniform sampler3D texture3D;
uniform vec3 u_CubeSize; //m_CubeObj的大小，控制体素化范围
//uniform vec3 u_CubePos; //m_CubeObj的大小，控制体素化范围

in vec3 worldPositionFrag;

in vec3 normalFrag;




out vec4 color;
vec3 normal = normalize(normalFrag);
float MAX_DISTANCE = distance(vec3(abs(worldPositionFrag)), vec3(-1));

float attenuate(float dist){ dist *= DIST_FACTOR; return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); }

// Returns a vector that is orthogonal to u.
vec3 orthogonal(vec3 u){
	u = normalize(u);
	vec3 v = vec3(0.99146, 0.11664, 0.05832); // Pick any normalized vector.
	return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}
vec3 scaleAndBias(vec3 p){
	return 0.5f * p + vec3(0.5f); 
}

// Returns true if the point p is inside the unity cube. 
//bool isInsideCube(const vec3 p, float e) { return abs(p.x) < u_CubeSize.x + e && abs(p.y) < u_CubeSize.y + e && abs(p.z) < u_CubeSize.z + e; }
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }


// Returns a soft shadow blend by using shadow cone tracing.
// Uses 2 samples per step, so it's pretty expensive.
//		shadowBlend = traceShadowCone(worldPositionFrag, lightDirection, distanceToLight);

float traceShadowCone(vec3 from, vec3 lightPos){
	from = from-u_CameraViewPos;
	lightPos = lightPos-u_CameraViewPos;
	
	vec3 fragPosToLight = lightPos-from;
	vec3 direction = fragPosToLight/u_CubeSize;
	from = from/u_CubeSize;
	//targetDistance = targetDistance/u_CubeSize.x;

	from += normal * 0.005; // Removes artifacts but makes self shadowing for dense meshes meh.

	float acc = 0;

	float dist = 3 * VOXEL_SIZE;
	// I'm using a pretty big margin here since I use an emissive light ball with a pretty big radius in my demo scenes.
	const float STOP = length(direction) - 16 * VOXEL_SIZE;//16
	direction = normalize(direction);
	while(dist < STOP && acc < 1){	
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
		c = scaleAndBias(c);
		float l = pow(dist, 2); // Experimenting with inverse square falloff for shadows.

		vec4 color1 = textureLod(texture3D, c,( 1 + 0.75 * l));
		vec4 color2 = textureLod(texture3D, c, (2.5 * l));

		if(color1.a>=0&&color2.a>=0){
			float s1 = 0.062 * textureLod(texture3D, c,( 1 + 0.75 * l)).a;
			float s2 = 0.135 * textureLod(texture3D, c, (4.5 * l)).a;
			//		float s1 = 0.062 * textureLod(texture3D, c, 1 + 0.75 * l).a;
			//		float s2 = 0.135 * textureLod(texture3D, c, 4.5 * l).a;
			float s = s1+ s2;
		
			acc += (1 - acc) * s;

		}
		
		dist += 0.9 * VOXEL_SIZE * (1 + 0.05 * l);//0.9 * VOXEL_SIZE * (1 + 0.05 * l);
	}
	return 1 - pow(smoothstep(0, 1, acc * 1.4), 1.0 / 1.4);
}	


vec3 traceDiffuseVoxelCone(const vec3 from,vec3 direction){
	direction = normalize(direction);
	const float CONE_SPEEAD = 0.325;

	vec4 acc = vec4(0.0f);

	
	// Controls bleeding from close surfaces.
	// Low values look rather bad if using shadow cone tracing.
	// Might be a better choice to use shadow maps and lower this value.
	float dist = 0.1953125;

	//Trace
	while(dist<SQRT2 && acc.a<1){
		
		vec3 c = from + dist * direction;
		c = scaleAndBias(c);
		float l = (1+ CONE_SPEEAD * dist/VOXEL_SIZE); //跨过了多少Voxel
		float level = log2(l);
		float ll = (level+1)*(level+1);
		
		vec4 voxel = textureLod(texture3D,c,min(MIPMAP_HARDCAP,level));
		if(voxel.a>=0)
			acc+=0.075*ll*voxel*pow(1-voxel.a,2);
		dist+= ll*VOXEL_SIZE*2;

	}

	return pow(acc.rgb*2.0,vec3(1.5));


}

// Traces a specular voxel cone.
vec3 traceSpecularVoxelCone(vec3 from, vec3 direction){
	from = (from-u_CameraViewPos)/u_CubeSize;
	direction = normalize(direction);

	const float OFFSET = 8 * VOXEL_SIZE;
	const float STEP = VOXEL_SIZE;

	from += OFFSET * normal;
	
	vec4 acc = vec4(0.0f);
	float dist = OFFSET;

	// Trace.
	while(dist < MAX_DISTANCE && acc.a < 1){ 
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
		c = scaleAndBias(c); 
		
		float level = 0.1 * u_Material.specularDiffusion * log2(1 + dist / VOXEL_SIZE);
		vec4 voxel = textureLod(texture3D, c, min(level, MIPMAP_HARDCAP));
		if(voxel.a>=0){

			float f = 1 - acc.a;
			acc.rgb += 0.25 * (1 + u_Material.specularDiffusion) * voxel.rgb * voxel.a * f;
			acc.a += 0.25 * voxel.a * f;
		}
		
		dist += STEP * (1.0f + 0.125f * level);
	}
	return 1.0 * pow(u_Material.specularDiffusion + 1, 0.8) * acc.rgb;
}

// Calculates indirect diffuse light using voxel cone tracing.
// The current implementation uses 9 cones. I think 5 cones should be enough, but it might generate
// more aliasing and bad blur.
vec3 indirectDiffuseLight(){
	//MY
	vec3 worldPositionFrag_=(worldPositionFrag-u_CameraViewPos)/u_CubeSize;

	const float ANGLE_MIX = 0.5f; // Angle mix (1.0f => orthogonal direction, 0.0f => direction of normal).

	const float w[3] = {1.0, 1.0, 1.0}; // Cone weights.

	// Find a base for the side cones with the normal as one of its base vectors.
	const vec3 ortho = normalize(orthogonal(normal));
	const vec3 ortho2 = normalize(cross(ortho, normal));

	// Find base vectors for the corner cones too.
	const vec3 corner = 0.5f * (ortho + ortho2);
	const vec3 corner2 = 0.5f * (ortho - ortho2);

	// Find start position of trace (start with a bit of offset).
	const vec3 N_OFFSET = normal * (1 + 4 * ISQRT2) * VOXEL_SIZE;
	const vec3 C_ORIGIN = worldPositionFrag_ + N_OFFSET;

	// Accumulate indirect diffuse light.
	vec3 acc = vec3(0);

	// We offset forward in normal direction, and backward in cone direction.
	// Backward in cone direction improves GI, and forward direction removes
	// artifacts.
	const float CONE_OFFSET = -0.01;

	// Trace front cone
	acc += w[0] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * normal, normal);

	// Trace 4 side cones.
	const vec3 s1 = mix(normal, ortho, ANGLE_MIX);
	const vec3 s2 = mix(normal, -ortho, ANGLE_MIX);
	const vec3 s3 = mix(normal, ortho2, ANGLE_MIX);
	const vec3 s4 = mix(normal, -ortho2, ANGLE_MIX);

	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho, s1);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho, s2);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho2, s3);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho2, s4);

	// Trace 4 corner cones.
	const vec3 c1 = mix(normal, corner, ANGLE_MIX);
	const vec3 c2 = mix(normal, -corner, ANGLE_MIX);
	const vec3 c3 = mix(normal, corner2, ANGLE_MIX);
	const vec3 c4 = mix(normal, -corner2, ANGLE_MIX);

	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner, c1);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner, c2);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner2, c3);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner2, c4);

	// Return result.
	return DIFFUSE_INDIRECT_FACTOR * u_Material.diffuseReflectivity * acc * (u_Material.diffuseColor + vec3(0.001f));
}

// Calculates indirect specular light using voxel cone tracing.
vec3 indirectSpecularLight(vec3 viewDirection){
	const vec3 reflection = normalize(reflect(viewDirection, normal));
	return u_Material.specularReflectivity * u_Material.specularColor * traceSpecularVoxelCone(worldPositionFrag, reflection);
}

// Calculates refractive light using voxel cone tracing.
vec3 indirectRefractiveLight(vec3 viewDirection){
	const vec3 refraction = refract(viewDirection, normal, 1.0 / u_Material.refractiveIndex);
	const vec3 cmix = mix(u_Material.specularColor, 0.5 * (u_Material.specularColor + vec3(1)), u_Material.transparency);
	return cmix * traceSpecularVoxelCone(worldPositionFrag, refraction);
}

// Calculates diffuse and specular direct light for a given point light.  
// Uses shadow cone tracing for soft shadows.
vec3 calculateDirectLight(const PointLight light, const vec3 viewDirection){
	vec3 lightDirection = light.position - worldPositionFrag;
	//MY 
	vec3 ligtdir = lightDirection/u_CubeSize;
	float distToLight = length(ligtdir);

	const float distanceToLight = length(lightDirection);
	lightDirection = lightDirection / distanceToLight;
	const float lightAngle = dot(normal, lightDirection);
	
	// --------------------
	// Diffuse lighting.
	// --------------------
	float diffuseAngle = max(lightAngle, 0.0f); // Lambertian.	
	
	// --------------------
	// Specular lighting.
	// --------------------
#if (SPECULAR_MODE == 0) /* Blinn-Phong. */
	const vec3 halfwayVector = normalize(lightDirection + viewDirection);
	float specularAngle = max(dot(normal, halfwayVector), 0.0f);
#endif
	
#if (SPECULAR_MODE == 1) /* Perfect reflection. */
	const vec3 reflection = normalize(reflect(viewDirection, normal));
	float specularAngle = max(0, dot(reflection, lightDirection));
#endif

	float refractiveAngle = 0;
	if(u_Material.transparency > 0.01){
		vec3 refraction = refract(viewDirection, normal, 1.0 / u_Material.refractiveIndex);
		refractiveAngle = max(0, u_Material.transparency * dot(refraction, lightDirection));
	}

	// --------------------
	// Shadows.
	// --------------------
	float shadowBlend = 1;
	if(diffuseAngle * (1.0f - u_Material.transparency) > 0 && u_Settings.shadows)
		shadowBlend = traceShadowCone(worldPositionFrag,light.position);



	// --------------------
	// Add it all together.
	// --------------------
	diffuseAngle = min(shadowBlend, diffuseAngle);
	specularAngle = min(shadowBlend, max(specularAngle, refractiveAngle));
	const float df = 1.0f / (1.0f + 0.25f * u_Material.specularDiffusion); // Diffusion factor.
	const float specular = SPECULAR_FACTOR * pow(specularAngle, df * SPECULAR_POWER);
	const float diffuse = diffuseAngle * (1.0f - u_Material.transparency);

	const vec3 diff = u_Material.diffuseReflectivity * u_Material.diffuseColor * diffuse;
	const vec3 spec = u_Material.specularReflectivity * u_Material.specularColor * specular;
	const vec3 total = light.diffuse * diff + light.specular* spec;
	return attenuate(distanceToLight) * total;
};

// Sums up all direct light from point lights (both diffuse and specular).
vec3 directLight(vec3 viewDirection){
	vec3 direct = vec3(0.0);
	//int maxLights = u_PointLightNums;// min(u_PointLightNums, MAX_LIGHTS);
	for(int i = 0; i < u_PointLightNums; ++i)
	{ direct += calculateDirectLight(u_PointLights[i], viewDirection);}
	direct *= DIRECT_LIGHT_INTENSITY;
	return direct;
}

void main(){
	color = vec4(0,0,0,1);
	const vec3 viewDirection = normalize(worldPositionFrag-u_CameraViewPos);

	//Indirect diffuse light
	if(u_Settings.indirectDiffuseLight && u_Material.diffuseReflectivity*(1.0-u_Material.transparency)>0.01)
		color.rgb += indirectDiffuseLight();

//	//Indirect specular light (glossy reflection)
//	if(u_Settings.indirectSpecularLight && u_Material.specularReflectivity*(1.0f-u_Material.transparency)>0.01f)
//		color.rgb += indirectSpecularLight(viewDirection);
//
//	//Emissivity
//	color.rgb += u_Material.emissivity * u_Material.diffuseColor;
//
//	//Transparency
//	if(u_Material.transparency>0.01f)
//		color.rgb = color.rgb*(1-u_Material.transparency) + indirectRefractiveLight(viewDirection)*u_Material.transparency;
//
	//Direct light
	if(u_Settings.directLight)
		color.rgb += directLight(viewDirection);

#if (GAMMA_CORRECTION == 1)
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
#endif

}