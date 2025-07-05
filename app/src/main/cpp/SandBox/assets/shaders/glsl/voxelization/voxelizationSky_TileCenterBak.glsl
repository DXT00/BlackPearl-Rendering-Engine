#type vertex
#version 450 core


layout(location = Slot_aPos) in vec3 aPos;



//uniform mat4 u_Model;
//uniform mat4 u_ProjectionView;
//uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
//uniform int  u_IsSkybox;
//uniform mat4 u_Projection;
//uniform mat4 u_View;
//uniform vec3 u_CubeSize;

out vec3 worldPositionGeom;
//out vec3 normalGeom;
out vec3 texCoordGeom;

#include <assets/shaders/glsl/common/CommonTransformStruct.glsl>



void main(){

	worldPositionGeom = vec3(g_Transform.matModel * vec4(aPos,1.0));
	texCoordGeom = aPos;
    gl_Position = g_Transform.matModel * vec4(aPos,1.0);//vec4(worldPositionGeom, 1.0);

	//}
}

#type geometry
#version 450 core

layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;

in vec3 worldPositionGeom[];
//in vec3 normalGeom[];
in vec3 texCoordGeom[];

out vec3 worldPositionFrag;
//out vec3 normalFrag;
out vec3 texCoordFrag;//v_TexCoord;

#include <voxel_cb.h>


/*几何着色器后会进入光栅化阶段，因此需要把坐标转换的 [-1,1]的裁剪平面！！*/

layout(std140, binding = 8) uniform VoxelUBO {
    VoxelConstants g_Voxel;
};

vec3 GetVoxelLocalPos(vec3 origin){
     float voxelMinX = g_Voxel.center.x - g_Voxel.areaExtent.x * 0.5;
     float voxelMinY = g_Voxel.center.y - g_Voxel.areaExtent.y * 0.5;
     float voxelMinZ = g_Voxel.center.z - g_Voxel.areaExtent.z * 0.5;

     return vec3(origin.x - voxelMinX,
                 origin.y - voxelMinY,
                 origin.z - voxelMinZ);

}

void main(){


	vec3 p1 = worldPositionGeom[1] - worldPositionGeom[0];
	vec3 p2 = worldPositionGeom[2] - worldPositionGeom[0];
	vec3 p = abs(cross(p1, p2)); 

	for(uint i = 0; i < 3; ++i){
		worldPositionFrag = worldPositionGeom[i];
		vec3 normal_worldPositionFrag = (worldPositionGeom[i]- g_Voxel.center) / g_Voxel.areaExtent; //GetVoxelLocalPos(worldPositionGeom[i]) / g_Voxel.areaExtent;//
		//normalFrag = normalGeom[i];
		texCoordFrag = vec3(texCoordGeom[i].x,texCoordGeom[i].y,texCoordGeom[i].z);

		if(p.z > p.x && p.z > p.y){
			//axisIndex=0;
			gl_Position = vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.y, 0, 1);
			// gl_Position =vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.y, normal_worldPositionFrag.z, 1);
		} 
		else if (p.x > p.y && p.x > p.z){
		//	axisIndex=1;
			gl_Position =vec4(normal_worldPositionFrag.y, normal_worldPositionFrag.z, 0, 1);
			//gl_Position =vec4(normal_worldPositionFrag.y, normal_worldPositionFrag.z, normal_worldPositionFrag.x, 1);
		} 
		else {
			//axisIndex=2;
			gl_Position = vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.z, 0, 1);
			//gl_Position =vec4(normal_worldPositionFrag.x, normal_worldPositionFrag.z, normal_worldPositionFrag.y, 1);
		}
		EmitVertex();
	}
    EndPrimitive();
}


#type fragment
#version 450 core


#include <voxel_cb.h>
#include <sky_cb.h>

#include <assets/shaders/glsl/common/CommonMaterialStruct.glsl>

layout(std140, binding = 8) uniform VoxelUBO {
    VoxelConstants g_Voxel;
};

layout(rgba8, binding = 0) uniform image3D texture3D;


layout(std140, binding = 2) uniform SkyConstantsUbo {

   SkyConstants g_SkyConstants;

};

layout(binding = 1) uniform samplerCube u_Skybox0;
layout(binding = 2) uniform samplerCube u_Skybox1;
layout(binding = 3) uniform samplerCube u_Skybox2;

const float PI=3.14159;


in vec3 worldPositionFrag;
//in vec3 normalFrag;
in vec3 texCoordFrag;

vec3 scaleAndBias(vec3 p) { return 0.5 * p + vec3(0.5); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

vec3 GetVoxelLocalPos(vec3 origin){
     float voxelMinX = g_Voxel.center.x - g_Voxel.areaExtent.x * 0.5;
     float voxelMinY = g_Voxel.center.y - g_Voxel.areaExtent.y * 0.5;
     float voxelMinZ = g_Voxel.center.z - g_Voxel.areaExtent.z * 0.5;

     return vec3(origin.x - voxelMinX,
                 origin.y - voxelMinY,
                 origin.z - voxelMinZ);

}

void main(){
    //skybox 画在最外层

	vec3 normalWorldPositionFrag = worldPositionFrag - g_Voxel.center;//GetVoxelLocalPos(worldPositionFrag);//// vec3(worldPositionFrag.x-u_CubePos.x,worldPositionFrag.y-u_CubePos.y,worldPositionFrag.z-u_CubePos.z);
	normalWorldPositionFrag = normalWorldPositionFrag / g_Voxel.areaExtent;
	
    if(!isInsideCube(normalWorldPositionFrag, 0.01)) return;
	
    
    vec4 skyBoxColor =
	g_SkyConstants.factors.x * texture(u_Skybox0,texCoordFrag)
				+ g_SkyConstants.factors.y * texture(u_Skybox1,texCoordFrag)
				+ g_SkyConstants.factors.z * texture(u_Skybox2,texCoordFrag);

	vec3 voxel = scaleAndBias(normalWorldPositionFrag);// normalWorldPositionFrag;// scaleAndBias(normalWorldPositionFrag);

	ivec3 dim = imageSize(texture3D);// retrieve the dimensions of an image
	vec4 res = vec4(vec3(skyBoxColor), 1);//vec4(1.0,1.0,0.0,1.0);


	imageStore(texture3D, ivec3(dim * voxel), res);//write a single texel into an image;

}