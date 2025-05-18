#type compute
#version 430 core


layout (local_size_x = 8,local_size_y = 8,local_size_z=1) in;
uniform layout(binding =0,r32ui) coherent uimageBuffer u_octreeBuffer;
uniform layout(binding =0,r32ui) coherent uimageBuffer u_octreeDiffuseBuffer;

uniform int u_ThreadNum;
uniform int u_AllocStart;

void main(){

	uint thxIdx = gl_GlobalInvocationID.y*1024+gl_GlobalInvocationID.x;
	if(thxIdx>= uint(u_ThreadNum))
		return;

	//uint node = imageLoad(u_octreeBuffer,u_AllocStart+int(thxIdx)).r;
	//if((node & 0x80000000) !=0){
		imageStore(u_octreeBuffer, int( u_AllocStart + thxIdx), uvec4(0,0,0,0) );
		imageStore(u_octreeDiffuseBuffer,int( u_AllocStart + thxIdx), uvec4(0,0,0,0) );
	//}


}


