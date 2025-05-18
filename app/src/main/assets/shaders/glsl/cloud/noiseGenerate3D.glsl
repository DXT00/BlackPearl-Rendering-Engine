#type compute
#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba8, binding = 0) uniform image3D u_Noise3D2;
uniform float u_Tex3D2Res;
//RWTexture3D<vec4> Noise3D2;

//---------------------------------------------------------------------------------------------------------
// Cellular Noise
//---------------------------------------------------------------------------------------------------------
float saturate(float val) {
	return clamp(val, 0.0, 1.0);
}
float setRange(float value, float low, float high) { return saturate((value - low) / (high - low)); }

vec3 voronoi_hash(vec3 x, float s) {
	x = mod(x, s);
	x = vec3(dot(x, vec3(127.1, 311.7, 74.7)),
		dot(x, vec3(269.5, 183.3, 246.1)),
		dot(x, vec3(113.5, 271.9, 124.6)));
	return fract(sin(x) * 43758.5453123);
}

vec3 voronoi(in vec3 x, float s, bool inverted) {
	x *= s;
	x += 0.5;
	vec3 p = floor(x);
	vec3 f = fract(x);

	float id = 0.0;
	vec2 res = vec2(1.0, 1.0);
	for (int k = -1; k <= 1; k++) {
		for (int j = -1; j <= 1; j++) {
			for (int i = -1; i <= 1; i++) {
				vec3 b = vec3(i, j, k);
				vec3 r = vec3(b)-f + voronoi_hash(p + b, s);
				float d = dot(r, r);

				if (d < res.x) {
					id = dot(p + b, vec3(1.0, 57.0, 113.0));
					res = vec2(d, res.x);
				}
				else if (d < res.y) {
					res.y = d;
				}
			}
		}
	}
	vec2 result = res;
	id = abs(id);
	if (inverted)
		return vec3(1.0 - result, id);
	else
		return vec3(result, id);
}

float worley3(vec3 p, float s) {
	vec3 xyz = p;

	float val1 = voronoi(xyz, 1.0 * s, true).r;
	float val2 = voronoi(xyz, 2.0 * s, false).r;
	float val3 = voronoi(xyz, 4.0 * s, false).r;

	val1 = saturate(val1);
	val2 = saturate(val2);
	val3 = saturate(val3);

	float worleyVal = val1;
	worleyVal = worleyVal - val2 * 0.3;
	worleyVal = worleyVal - val3 * 0.3;

	return worleyVal;
}

//[numthreads(8, 8, 8)]
void Noise3D2Gen()
{
	float tex2Low = -0.2;
	float tex2High = 1.0;

	vec3 xyz = vec3(gl_GlobalInvocationID.xyz/ u_Tex3D2Res);

	float worleyR = worley3(xyz, 10);
	float worleyG = worley3(xyz, 15);
	float worleyB = worley3(xyz, 20);
	float worleyA = worley3(xyz, 10);

	worleyR = setRange(worleyR, tex2Low, tex2High);
	worleyG = setRange(worleyG, tex2Low, tex2High);
	worleyB = setRange(worleyB, tex2Low, tex2High);
	worleyA = setRange(worleyA, tex2Low, tex2High);

	//u_Noise3D2[id.xyz] = vec4(worleyR, worleyG, worleyB, worleyA);

	imageStore(u_Noise3D2, ivec3(gl_GlobalInvocationID.xyz), vec4(worleyR, worleyG, worleyB, worleyA));//write a single texel into an image;

	// DEBUG
	//u_Noise3D2[id.xyz] = vec4(worleyR, worleyR, worleyR, 1.0);
	//u_Noise3D2[id.xyz] = vec4(worleyG, worleyG, worleyG, 1.0);
	//u_Noise3D2[id.xyz] = vec4(worleyB, worleyB, worleyB, 1.0);
	//u_Noise3D2[id.xyz] = vec4(worleyA, worleyA, worleyA, 1.0);
}


void main() {

	Noise3D2Gen();
}
