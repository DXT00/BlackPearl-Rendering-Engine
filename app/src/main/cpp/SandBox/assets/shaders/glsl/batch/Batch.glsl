#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aJointIndices;
layout(location = 6) in ivec4 aJointIndices1;
layout(location = 7) in vec4 aWeights;
layout(location = 8) in vec4 aWeights1;
layout(location = 9) in float aObjId;
layout(location = 10) in vec3 aColor;

out vec2 v_TexCoords;
flat out vec3 v_Color;
flat out float v_ObjId;

uniform mat3x4 u_Model[300];

uniform mat4 u_ProjectionView;

mat4 restoreModel(in mat3x4 model){
	
	mat4x3 tran = transpose(model);
	return mat4(tran[0], 0.0, 
				tran[1], 0.0, 
				tran[2], 0.0, 
				tran[3], 1.0);

//	return mat4(0.5*tran[0], 0.0, 
//				0.5*tran[1], 0.0, 
//				0.5*tran[2], 0.0, 
//				vec3(0,0,aObjId*2.0), 1.0);
}
void main()
{
	v_TexCoords = aTexCoords;
	v_Color = aColor;
	mat4 model = restoreModel(u_Model[int(aObjId)]);
	gl_Position = u_ProjectionView * model * vec4(aPos,1.0);

	v_ObjId = aObjId;

}

#type fragment
#version 450 core
out vec4 FragColor;
in vec2 v_TexCoords;
flat in vec3 v_Color;
flat in float v_ObjId;


void main(){
	FragColor = vec4(v_Color,1.0);

}
