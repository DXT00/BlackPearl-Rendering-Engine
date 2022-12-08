#type compute
#version 430 core

layout(local_size_x = 1) in;
uniform int uMeshCounts;
struct IndirectCommand{
		uint  count; //index or vertex cnt
		uint  instanceCnt;
		uint  firstIndex;
		uint  startVertex;
		uint  startInstance;
};

layout(binding = 0, std430) buffer InCountBuffer {
	uint countBuffer[];
};

layout(binding = 4, std430) buffer IndirectCmds{
	IndirectCommand cmds[];

};
void main(){
	uint meshId = gl_GlobalInvocationID.x;
	uint baseInstance = 0;
	if (meshId < uMeshCounts) {

		for(int i = 0; i < meshId; i++) {
		
			atomicAdd(cmds[meshId].startInstance, countBuffer[i]);

		}
		cmds[meshId].instanceCnt = countBuffer[meshId];

	}
}