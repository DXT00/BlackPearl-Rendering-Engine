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


void GerstnerLevelOne(out half3 offsets, out half3 normal, half3 vertex, half3 sVertex,
	half amplitude, half frequency, half steepness, half speed,
	half4 directionAB, half4 directionCD)
{
	half3 offs = 0;

	for (int i = 0; i < 5; i++)
	{
		offs.x += steepness * amplitude * steeps[i] * amps[i] * dir[i].x * cos(frequency * fs[i] * dot(sVertex.xz, dir[i]) + speeds[i] * frequency * fs[i] * _Time.y);
		offs.z += steepness * amplitude * steeps[i] * amps[i] * dir[i].y * cos(frequency * fs[i] * dot(sVertex.xz, dir[i]) + speeds[i] * frequency * fs[i] * _Time.y);
		offs.y += amplitude * amps[i] * sin(frequency * fs[i] * dot(sVertex.xz, dir[i].xy) + speeds[i] * frequency * fs[i] * _Time.y);
	}

	offsets = offs;

	normal = half3(0, 1, 0);
	for (int i = 0; i < 5; i++)
	{
		normal.x -= dir[i].x * frequency * fs[i] * amplitude * amps[i] * cos(dot(offs, frequency * fs[i] * dir[i]) + speeds[i] * frequency * fs[i] * _Time.y);
		normal.z -= dir[i].y * frequency * fs[i] * amplitude * amps[i] * cos(dot(offs, frequency * fs[i] * dir[i]) + speeds[i] * frequency * fs[i] * _Time.y);
		normal.y -= steepness * steeps[i] * frequency * fs[i] * amps[i] * amplitude * sin(dot(offs, frequency * fs[i] * dir[i]) + speeds[i] * frequency * fs[i] * _Time.y);
	}
}
void main()
{
	

	imageStore(u_initial_spectrum, pixel_coord, vec4(h, 0.f, 0.f, 0.f));
}