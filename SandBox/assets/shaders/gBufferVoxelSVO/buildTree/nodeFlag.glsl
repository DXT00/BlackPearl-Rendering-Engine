#type compute
#version 430 core

layout(local_size_x = 8,local_size_y = 8,local_size_z=1) in;


uniform int u_level;
uniform int u_numVoxelFrag;
uniform int u_voxelDim;

uniform layout(binding = 0,rgb10_a2ui) uimageBuffer u_voxelPos;
uniform layout(binding = 1,r32ui) uimageBuffer u_octreeBuffer;
uniform layout(binding = 2,rg16ui) uimageBuffer u_debugBuffer;

int Clamp(int x){
	if(x>=1) return 1;
	else if(x<=0) return 0;
	else
		return x;
}
void main(){

	uint thxId = gl_GlobalInvocationID.y*1024+gl_GlobalInvocationID.x;
	
	if(thxId>=uint(u_numVoxelFrag))return;


	bool flag = true;
	uint node;
	int subnode=0;
	int childIdx = 0;
	uint voxelDim = uint(u_voxelDim);

	uvec3 umin,umax;
	uvec4 loc;

	loc = imageLoad(u_voxelPos,int(thxId));
	//decide max and min coord for the root node
	umin = uvec3(0,0,0);
	umax = uvec3( voxelDim, voxelDim, voxelDim );

	node = imageLoad(u_octreeBuffer,0).r;

	for(int i=0;i<u_level;i++){
		voxelDim/=2u;
		if((node & 0x80000000u)==0u){
			flag = false;
			break;
		}
		childIdx =int(node & 0x7FFFFFFFu); 

		subnode = Clamp(1+int(loc.x)-int(umin.x)-int(voxelDim));
		subnode += 4*Clamp(1+int(loc.y)-int(umin.y)-int(voxelDim));
		subnode += 2*Clamp(1+int(loc.z)-int(umin.z)-int(voxelDim));

		childIdx = childIdx+subnode;
		umin.x += voxelDim * uint(Clamp(1+int(loc.x)-int(umin.x)-int(voxelDim)));
	    umin.y += voxelDim * uint(Clamp(1+int(loc.y)-int(umin.y)-int(voxelDim)));
	    umin.z += voxelDim * uint(Clamp(1+int(loc.z)-int(umin.z)-int(voxelDim)));

		node = imageLoad( u_octreeBuffer, childIdx).r;
	}
	if(flag){
		node|=0x80000000u;
		imageStore(u_octreeBuffer,int(childIdx),uvec4(node,0,0,0));
		imageStore(u_debugBuffer,int(childIdx),uvec4(uint(childIdx),uint(subnode),0,0));

	}

}
