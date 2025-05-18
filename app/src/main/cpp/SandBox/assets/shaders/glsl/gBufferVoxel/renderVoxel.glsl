#type vertex
#version 450 core 



//out vec3 v_normal;
out vec4 v_vertex;
out vec3 v_texcoord;
out vec4 v_color;
uniform mat4 u_Model;
uniform mat4 u_ProjectionView;
uniform sampler3D texture3D;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform mat4 u_Normal;
uniform int u_voxelDim;
uniform float u_halfDim;
uniform vec3   u_CameraViewPos;
uniform vec3 u_CubeSize;
void main(){

	int voxelDim = u_voxelDim;
//	v_texcoord.x =  (mod(gl_VertexID,voxelDim));
//
//	v_texcoord.z = (mod((gl_VertexID / voxelDim) , voxelDim));
//	v_texcoord.y = (gl_VertexID / (voxelDim*voxelDim));
	v_texcoord.x = float( gl_VertexID%u_voxelDim);
	v_texcoord.z =float( (gl_VertexID / u_voxelDim)%u_voxelDim);
	v_texcoord.y =float( gl_VertexID / (u_voxelDim*u_voxelDim));
	v_vertex = vec4( v_texcoord/float(u_voxelDim)*2.0-vec3(1.0), 1.0 ); //[0,1]-->[-1,1]
	//v_vertex.z += u_halfDim;
	//v_vertex.x -= u_halfDim;

	//vec4 temp = textureLod(texture3D,v_texcoord/256.0,0);
	//vec4 temp = texture(texture3D,v_vertex.xyz);

	//if(temp.r!=0||temp.g!=0||temp.b!=0)
		v_color = texture(texture3D,v_texcoord/256.2);
	//else
		//v_color = vec4( 1,0,0, 1.0 );//vec4( v_texcoord/256.0, 1.0 );
	gl_Position = u_ProjectionView*u_Model*v_vertex;

}

#type geometry
#version 450 core

layout (points) in;
layout (triangle_strip,max_vertices = 26) out;
uniform sampler3D texture3D;

in vec4 v_vertex[];
in vec4 v_color[];
in vec3 v_texcoord[];
//in vec3 v_normal[];
uniform vec3   u_CameraViewPos;

out vec4 f_vertex;
out vec4 f_color;

out vec3 worldPositionFrag;

uniform float u_halfDim;
uniform int u_voxelDim;

uniform mat4 u_ModelView;
uniform mat4 u_Model;
uniform vec3 u_CubeSize;

uniform mat4 u_Proj;

uniform mat4 u_ProjectionView;


ivec3 ClampVec3(ivec3 v){
	ivec3 res = ivec3(0);
	if(v.x>=1) res.x = 1;
	else if(v.x<=0)  res.x = 0;
	else res.x = v.x;

	if(v.y>=1) res.y = 1;
	else if(v.y<=0)  res.y = 0;
	else res.y = v.y;

	if(v.z>=1) res.z = 1;
	else if(v.z<=0)  res.z = 0;
	else res.z = v.z;

	return res;
}


void main(){

	vec3 pos,color;
	//f_color = textureLod(texture3D,v_vertex[0].xyz,0);
	worldPositionFrag = vec3(u_CubeSize.x*v_vertex[0].x,u_CubeSize.y*v_vertex[0].y,u_CubeSize.z*v_vertex[0].z)+u_CameraViewPos;
	// bool oc = nodeOccupied(v_texcoord[0],leafIdx);

	f_color =v_color[0];// convRGBA8ToVec4(val).rgba/255.0;



		//+Z front face
	//f_color = vec4(0.5, 0.5, 0.5, 1 );
	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim);
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	//gl_Position = u_ProjectionView*vec4(u_CubeSize*pos+u_CameraViewPos,1.0);
	//f_vertex = v_vertex[0];

	//f_normal = v_normal[0];
	//f_color = color;
	EmitVertex();

	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim );
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	//gl_Position = u_ProjectionView*vec4(u_CubeSize*pos+u_CameraViewPos,1.0);

	EmitVertex();

    pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim );
    //gl_Position = u_ProjectionView* pos;
	gl_Position = u_ProjectionView*vec4(u_CubeSize*pos+u_CameraViewPos,1.0);
	EmitVertex();

	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim );
    //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();

	//+X
	//for degenerate purpose
	EmitVertex();
	//f_color = vec4( 0.2, 0.2, 0.2, 1 );
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim );
	//gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim );
	//gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim );
	//gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	

	//-Z
	EmitVertex(); //for degenerate purpose

	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();

	//-X
	EmitVertex(); //for degenerate purpose
	//f_color = vec4( 0.5, 0.5, 0.5, 1 );
	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim );
	//gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim );
	//gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();

	//-Y
	EmitVertex();

	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim );
	//gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();

	//+Y
	EmitVertex();
	//f_color = color;
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim );
	//gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	EmitVertex();

	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();
	pos = vec3( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim );
	 //gl_Position = u_ProjectionView* pos;
    gl_Position = u_ProjectionView*u_Model*vec4( pos,1.0);
	EmitVertex();

	EmitVertex();

	EndPrimitive();

}

#type fragment
#version 450 core
in vec3 worldPositionFrag;
in vec4 f_color;
in vec4 f_vertex;
out vec4 fragColor;
//out vec4 fragColor;
uniform sampler3D texture3D;
uniform vec3  u_CameraViewPos;
uniform vec3 u_CubeSize;

void main(){

//	vec3 pos = (worldPositionFrag-u_CameraViewPos)/u_CubeSize;
	fragColor =vec4(1.0,0.0,0.0,1.0);//f_color;// vec4(texture(texture3D,pos).rgb,1.0);//vec4(textureLod(texture3D,pos,0).rgb,1.0);
	
	
	//vec4(f_leafIdx,1.0);//vec4(1.0,0.0,0.0,1.0);//f_color;
		//fragColor =vec4(f_leafIdx,1.0);//f_vertex;//vec4(1.0,0.0,0.0,1.0);// f_color;


}

