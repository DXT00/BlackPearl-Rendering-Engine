#type vertex
#version 450 core 

layout (location = 0) in vec4 glVertex;
layout (location = 1) in vec4 glColor;
layout (location = 2) in vec3 glNormal;

//out vec3 v_normal;
out vec4 v_vertex;
out uvec3 v_texcoord;
//out vec4 v_color;
uniform mat4 u_Model;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform mat4 u_Normal;
uniform int u_voxelDim;
uniform float u_halfDim;
void main(){
//	
//	v_texcoord.x = uint(mod(gl_VertexID,u_voxelDim));
//	v_texcoord.z = uint(mod(gl_VertexID/u_voxelDim,u_voxelDim));
//	v_texcoord.y =uint( gl_VertexID/(u_voxelDim*u_voxelDim));
//
//	gl_Position = u_Proj*u_ModelView*vec4(v_texcoord,1.0);
//	v_vertex = vec4(float(v_texcoord)/float(u_voxelDim)*2.0-vec3(1.0),1.0);
//	v_vertex.z+= 1.0/u_voxelDim;
//	v_vertex.x-=1.0/u_voxelDim;
//	v_normal = u_Normal*glNormal;  //?
//	v_color = glColor;  //?
 //v_texcoord = glVertex.xyz/u_voxelDim;
	int voxelDim = u_voxelDim;
	v_texcoord.x =  uint(mod(gl_VertexID,voxelDim));

	v_texcoord.z = uint(mod((gl_VertexID / voxelDim) , voxelDim));
	v_texcoord.y = uint(gl_VertexID / (voxelDim*voxelDim));
	v_vertex = vec4( v_texcoord/float(u_voxelDim)*2.0-1, 1.0 ); //[0,1]-->[-1,1]
	v_vertex.z += u_halfDim;
	v_vertex.x -= u_halfDim;
	gl_Position = v_vertex;//u_Proj * u_ModelView * u_Model * vec4( v_texcoord, 1.0 );

	//v_texcoord.xyz /= u_voxelDim;
	//v_vertex = vec4( v_texcoord*2-1, 1.0 );

	//v_normal = mat3(u_Normal) * glNormal;
	//v_color = glColor; 
}

#type geometry
#version 450 core

layout (points) in;
layout (triangle_strip,max_vertices = 26) out;

in vec4 v_vertex[];
//in vec4 v_color[];
in uvec3 v_texcoord[];
//in vec3 v_normal[];

out vec4 f_vertex;
out vec4 f_color;
out vec3 f_leafIdx;

//out vec3 f_normal;

uniform float u_halfDim;
uniform int u_voxelDim;

uniform mat4 u_ModelView;
uniform mat4 u_Model;

uniform mat4 u_Proj;

uniform int u_octreeLevel;
uniform layout(binding = 0,r32ui) uimageBuffer u_octreeBuffer;
uniform layout(binding = 1,r32ui) uimageBuffer u_octreeKd;
int uOctreeLevel = u_octreeLevel;
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
bool nodeOccupied( uvec3 loc,out int leafIdx){

	bool occupied = true;
	uint voxelDim =uint(u_voxelDim);
	uint idx=0u;
	uvec3 umin = uvec3(0);
	ivec3 offset;
	for(int i=0;i<=uOctreeLevel;++i){
		leafIdx = int(idx);
		idx = imageLoad(u_octreeBuffer,int(idx)).r;
		if((idx&0x80000000u) ==0u){
			occupied = false;
			break;
		}
		else if( i == uOctreeLevel )
		{	    
		    break;
		}
		idx &= 0x7FFFFFFFu;
		if( idx == 0u)
		{
		    occupied =false;// false;
		    break;
		}
		voxelDim/=2u;

		//childIdx+=clamp(loc.x-voxelDim,0u,1u)+2u*clamp(loc.z-voxelDim,0u,1u)+4u*clamp(loc.y-voxelDim,0u,1u);
//		ivec3 tmp = ivec3(1) + ivec3(loc) - ivec3(umin) - ivec3(voxelDim);
//
//		ivec3 offset = clamp( tmp, 0, 1 );
//
//	    idx += uint(offset.x + 4*offset.y + 2*offset.z);
//	
//	    umin += voxelDim * uint(offset);
		offset =ClampVec3( ivec3(
		
		1 + int(loc.x) - int(umin.x) - int(voxelDim),
		1 + int(loc.y) - int(umin.y) - int(voxelDim),
		1 + int(loc.z) - int(umin.z) - int(voxelDim)
		
		) );
	    idx += uint(offset.x + 4*offset.y + 2*offset.z);
	
	    umin += voxelDim * uvec3(offset);
	}

	return occupied;
}

vec4 convRGBA8ToVec4( uint val )
{
    return vec4( float( (val&0x000000FF) ), float( (val&0x0000FF00)>>8U),
	             float( (val&0x00FF0000)>>16U), float( (val&0xFF000000)>>24U) );
}
void main(){

	vec4 pos,color;
	int leafIdx;
	if( nodeOccupied(v_texcoord[0],leafIdx)){
	// bool oc = nodeOccupied(v_texcoord[0],leafIdx);

	uint val = imageLoad(u_octreeKd,leafIdx).r;
	f_color = convRGBA8ToVec4(val).rgba/255.0;

	}
	else 
		return;

		//+Z front face
	//f_color = vec4(0.5, 0.5, 0.5, 1 );
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	f_vertex = v_vertex[0];
	f_leafIdx = vec3(leafIdx/255.0);

	//f_normal = v_normal[0];
	//f_color = color;
	EmitVertex();

	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;

	EmitVertex();

    pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();

	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();

	//+X
	//for degenerate purpose
	EmitVertex();
	//f_color = vec4( 0.2, 0.2, 0.2, 1 );
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	

	//-Z
	EmitVertex(); //for degenerate purpose

	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();

	//-X
	EmitVertex(); //for degenerate purpose
	//f_color = vec4( 0.5, 0.5, 0.5, 1 );
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();

	//-Y
	EmitVertex();

	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();

	//+Y
	EmitVertex();
	//f_color = color;
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	EmitVertex();

	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	 //gl_Position = u_Proj * u_ModelView * pos;
	gl_Position = u_Proj * u_ModelView *u_Model* pos;
	EmitVertex();

	EmitVertex();

	EndPrimitive();

}

#type fragment
#version 450 core

in vec4 f_color;
in vec4 f_vertex;
in vec3 f_leafIdx;
layout(location = 0) out vec4 gl_FragColor;
out vec4 fragColor;
void main(){

	gl_FragColor =f_color;//vec4(f_leafIdx,1.0);//vec4(1.0,0.0,0.0,1.0);//f_color;
		//fragColor =vec4(f_leafIdx,1.0);//f_vertex;//vec4(1.0,0.0,0.0,1.0);// f_color;


}

