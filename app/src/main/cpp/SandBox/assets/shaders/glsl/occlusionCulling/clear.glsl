
#type compute
#version 430 core

layout(local_size_x = 1) in;

struct IndirectCommand{
		uint  count; //index or vertex cnt
		uint  instanceCnt;
		uint  firstIndex;
		uint  startVertex;
		uint  startInstance;
};

layout(binding = 0, std430) buffer OutCounts {
	uint countBuff[];
};

layout(binding = 1, std430) buffer Dispatchs {
	uvec3 dispatchObjsCnt;
};

layout(binding = 4, std430) buffer IndirectCmds {
	IndirectCommand cmds[];
};


uniform uint uMeshCounts;

void main() {
	uint meshid = gl_GlobalInvocationID.x;
	
	dispatchObjsCnt.x = 0;
	dispatchObjsCnt.y = 1;
	dispatchObjsCnt.z = 1;
						  
	if(meshid < uMeshCounts) {
		cmds[meshid].instanceCnt = 0;
		cmds[meshid].startInstance = 0;
		countBuff[meshid] = 0;
	}
	
}
