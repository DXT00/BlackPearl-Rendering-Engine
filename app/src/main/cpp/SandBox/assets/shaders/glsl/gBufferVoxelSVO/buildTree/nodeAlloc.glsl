#type compute
#version 430 core


layout (local_size_x = 64,local_size_y = 1,local_size_z=1) in;

uniform layout(binding = 0,r32ui) uimageBuffer u_octreeBuffer;

layout(binding = 0,offset =0) uniform atomic_uint u_allocCount;

uniform int u_NodeStart;
uniform int u_AllocStart;
uniform int u_ThreadNum;

void main(){
uint offset;
	uint thxId = gl_GlobalInvocationID.x;
	if(int(thxId)>=u_ThreadNum)
		return;

	uint childIdx = imageLoad(u_octreeBuffer,u_NodeStart+int(thxId)).r;

	if((childIdx & 0x80000000)!=0){
		
		offset = atomicCounterIncrement(u_allocCount);//计算并存储childIdx的子节点索引
		offset*=8u;
		offset+=uint(u_AllocStart);
		offset|=0x80000000u;

		imageStore(u_octreeBuffer,u_NodeStart+int(thxId),uvec4(offset,0,0,0));

	}

}




