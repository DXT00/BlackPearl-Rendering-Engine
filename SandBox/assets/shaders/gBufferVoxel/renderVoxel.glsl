#type vertex
#version 450 core 

layout (location = 0) in vec4 glVertex;
layout (location = 1) in vec4 glColor;
layout (location = 2) in vec3 glNormal;

//out vec3 v_normal;
out vec4 v_vertex;
//out vec3 v_texcoord;
out vec4 v_color;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform mat4 u_Normal;
uniform int u_voxelDim;
uniform float u_halfDim;
uniform sampler3D texture3D;
uniform mat4 u_Model;
uniform vec3 u_CameraViewPos;
uniform vec3 u_CubeSize;
void main(){
	vec3 v_texcoord;
	float voxelDim = float(u_voxelDim);
	v_texcoord.x =  gl_VertexID%u_voxelDim;

	v_texcoord.z = (gl_VertexID / u_voxelDim)%u_voxelDim;
	v_texcoord.y = gl_VertexID / (u_voxelDim*u_voxelDim);
	//gl_Position = u_Proj * u_ModelView *u_Model* vec4( v_texcoord, 1.0 );
	gl_Position = vec4(v_texcoord - voxelDim*0.5, 1);

	//v_texcoord.xyz /= u_voxelDim;
	//v_vertex = vec4( v_texcoord*2-1, 1.0 );
	v_vertex = vec4( v_texcoord/float(u_voxelDim)*2.0-1, 1.0 ); //[0,1]-->[-1,1]
	v_vertex.z += u_halfDim;
	v_vertex.x -= u_halfDim;
	
	v_color = texture(texture3D,v_texcoord/voxelDim);
}

#type geometry
#version 450 core
layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

uniform float VoxelSize;
uniform int Dimensions;
uniform int TotalNumVoxels; // Dimensions^3
uniform mat4 u_Model;
uniform mat4 u_ModelView;
uniform mat4 u_Proj;

in vec4 v_color[];

out vec4 fragColor;
out vec3 normal_world;

void main() {
	fragColor = v_color[0];

	vec4 v1 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(-0.5, 0.5, 0.5, 0));
	vec4 v2 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(0.5, 0.5, 0.5, 0));
	vec4 v3 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(-0.5, -0.5, 0.5, 0));
	vec4 v4 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(0.5, -0.5, 0.5, 0));
	vec4 v5 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(-0.5, 0.5, -0.5, 0));
	vec4 v6 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(0.5, 0.5, -0.5, 0));
	vec4 v7 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(-0.5, -0.5, -0.5, 0));
	vec4 v8 = u_Proj * u_ModelView * (gl_in[0].gl_Position + vec4(0.5, -0.5, -0.5, 0));

	//
	//      v5 _____________ v6
	//        /|           /|
	//       / |          / |
	//      /  |         /  |
	//     /   |        /   |
	// v1 /____|_______/ v2 |
	//    |    |       |    |
	//    |    |_v7____|____| v8
	//    |   /        |   /
	//    |  /         |  /  
	//    | /          | /  
	// v3 |/___________|/ v4
	//

	// TODO: Optimize
	// +Z
    normal_world = vec3(0, 0, 1);
    gl_Position = v1;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();
    gl_Position = v1;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();

    // -Z
    normal_world = vec3(0, 0, -1);
    gl_Position = v6;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v7;
    EmitVertex();
    EndPrimitive();
    gl_Position = v6;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v5;
    EmitVertex();
    EndPrimitive();

    // +X
    normal_world = vec3(1, 0, 0);
    gl_Position = v2;
    EmitVertex();
    gl_Position = v4;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v2;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -X
    normal_world = vec3(-1, 0, 0);
    gl_Position = v5;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v3;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v3;
    EmitVertex();
	gl_Position = v1;
    EmitVertex();
    EndPrimitive();

    // +Y
    normal_world = vec3(0, 1, 0);
    gl_Position = v5;
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
	gl_Position = v2;
    EmitVertex();
    EndPrimitive();
    gl_Position = v5;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();
	gl_Position = v6;
    EmitVertex();
    EndPrimitive();

    // -Y
    normal_world = vec3(0, -1, 0);
    gl_Position = v3;
    EmitVertex();
    gl_Position = v7;
    EmitVertex();
	gl_Position = v8;
    EmitVertex();
    EndPrimitive();
    gl_Position = v3;
    EmitVertex();
    gl_Position = v8;
    EmitVertex();
	gl_Position = v4;
    EmitVertex();
    EndPrimitive();

}

#type fragment
#version 450 core

in vec4 fragColor;
//in vec4 f_vertex;
//in vec3 f_leafIdx;
layout(location = 0) out vec4 gl_FragColor;
//out vec4 fragColor;
void main(){

	gl_FragColor =fragColor;//vec4(f_leafIdx,1.0);//vec4(1.0,0.0,0.0,1.0);//f_color;
		//fragColor =vec4(f_leafIdx,1.0);//f_vertex;//vec4(1.0,0.0,0.0,1.0);// f_color;


}

