/*
Copyright (c) 2021 Achal Pandey
https://github.com/achalpandeyy/OceanFFT
*/
#type compute
#version 460 core

#define COMPUTE_WORK_GROUP_DIM 32

const float PI = 3.14159265359;
const float g = 9.81;
const float KM = 370.0;
const float CM = 0.23;

layout(local_size_x = COMPUTE_WORK_GROUP_DIM, local_size_y = COMPUTE_WORK_GROUP_DIM) in;

layout(binding = 0, r32f) writeonly uniform image2D u_initial_spectrum;

uniform int u_resolution;
uniform int u_ocean_size;
uniform vec2 u_wind;

float omega(float k)
{
	return sqrt(g * k * (1.f + ((k * k) / (KM * KM))));
}

float square(float x)
{
	return x * x;
}

void main()
{
	ivec2 pixel_coord = ivec2(gl_GlobalInvocationID.xy);
	float n = (pixel_coord.x < 0.5f * u_resolution) ? pixel_coord.x : pixel_coord.x - u_resolution;
	float m = (pixel_coord.y < 0.5f * u_resolution) ? pixel_coord.y : pixel_coord.y - u_resolution;

	vec2 wave_vector = (2.f * PI * vec2(n, m)) / u_ocean_size;
	float k = length(wave_vector);

	float U10 = length(u_wind);

	float Omega = 0.84f;
	float kp = g * square(Omega / U10);

	float c = omega(k) / k;
	float cp = omega(kp) / kp;

	float Lpm = exp(-1.25 * square(kp / k));
	float gamma = 1.7;
	float sigma = 0.08 * (1.0 + 4.0 * pow(Omega, -3.0));
	float Gamma = exp(-square(sqrt(k / kp) - 1.0) / 2.0 * square(sigma));
	float Jp = pow(gamma, Gamma);
	float Fp = Lpm * Jp * exp(-Omega / sqrt(10.0) * (sqrt(k / kp) - 1.0));
	float alphap = 0.006 * sqrt(Omega);
	float Bl = 0.5 * alphap * cp / c * Fp;

	float z0 = 0.000037 * square(U10) / g * pow(U10 / cp, 0.9);
	float uStar = 0.41 * U10 / log(10.0 / z0);
	float alpham = 0.01 * ((uStar < CM) ? (1.0 + log(uStar / CM)) : (1.0 + 3.0 * log(uStar / CM)));
	float Fm = exp(-0.25 * square(k / KM - 1.0));
	float Bh = 0.5 * alpham * CM / c * Fm * Lpm;

	float a0 = log(2.0) / 4.0;
	float am = 0.13 * uStar / CM;
	float Delta = tanh(a0 + 4.0 * pow(c / cp, 2.5) + am * pow(CM / c, 2.5));

	float cosPhi = dot(normalize(u_wind), normalize(wave_vector));

	float S = (1.0 / (2.0 * PI)) * pow(k, -4.0) * (Bl + Bh) * (1.0 + Delta * (2.0 * cosPhi * cosPhi - 1.0));

	float dk = 2.0 * PI / u_ocean_size;
	float h = sqrt(S / 2.0) * dk;

	if (wave_vector.x == 0.0 && wave_vector.y == 0.0) h = 0.f;

	imageStore(u_initial_spectrum, pixel_coord, vec4(h, 0.f, 0.f, 0.f));
}