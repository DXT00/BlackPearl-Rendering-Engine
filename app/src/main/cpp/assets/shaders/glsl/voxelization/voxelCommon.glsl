#ifndef BP_VOXEL_COMMON_H
#define BP_VOXEL_COMMON_H


// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

// Returns true if p is inside the unity cube (+ e) centered on (0, 0, 0).
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }


#endif