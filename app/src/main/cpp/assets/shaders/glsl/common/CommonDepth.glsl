
#ifndef BP_COMMON_DEPTH_H
#define BP_COMMON_DEPTH_H

// also see ConvertToDeviceZ()
// @param DeviceZ value that is stored in the depth buffer (Z/W) range: [0,1]
// @return SceneDepth (linear in view Space) range: [near, far]

//https://blog.csdn.net/qq_32095699/article/details/148117198?sharetype=blogdetail&sharerId=148117198&sharerefer=PC&sharesource=qq_32095699&spm=1011.2480.3001.8118

float ConvertFromDeviceZ(float DeviceZ, float near, float far)
{
	float z = DeviceZ * 2.0 - 1.0;//Back to NDC coordinate
	float z_view =  2.0 * near * far /(far + near - DeviceZ * (far - near));
	//float zInworldUnit = (zInworld - near)/(far -near);

	return z_view;
}



// inverse operation of ConvertFromDeviceZ()
// @param SceneDepth (linear in world units, W) range: [near, far]
// @return DeviceZ (Z/W) range: [0,1]
/*

calculate according to the projection matrix:

M_proj = 
		n/r  0  0  0 
		0  n/t  0  0 
		0  0  -(f+n)/(f-n)  -(2fn)/(f-n)
		0  0  -1  0 

		z_c = { -(f+n)/(f-n)} * z_v  -(2fn)/(f-n)

*/
float ConvertToDeviceZ(float SceneDepth, int near, int far)
{  
    float z_view = SceneDepth;
    float A = -(far + near) / (far - near);
    float B = -2.0 * far * near / (far - near);
    float z_ndc = (A * z_view + B) / (-z_view);
	float zInBuffer =  (z_ndc + 1.0) / 2.0; // 转换到 [0, 1]
    return zInBuffer;
}

#endif