#type compute
#version 450 core

layout (local_size_x = 8,local_size_y = 8,local_size_z = 1) in;

uniform layout(binding = 0,r32ui) uimageBuffer u_octreeIdx;
uniform layout(binding = 1,r32ui) uimageBuffer u_octreeKd;

uniform layout(binding = 2,rgb10_a2ui) uimageBuffer u_voxelPos;
uniform layout(binding = 3,rgba8 ) imageBuffer u_voxelKd;

uniform int u_numVoxelFrag;
uniform int u_level;
uniform int u_voxelDim;
void imageAtomicRGBA8Avg( vec4 val, int coord, layout(r32ui) uimageBuffer buf );
uint convVec4ToRGBA8( vec4 val );
vec4 convRGBA8ToVec4( uint val );
void main(){

	uint thxId = gl_GlobalInvocationID.y*1024u+gl_GlobalInvocationID.x;
	if(thxId>=uint(u_numVoxelFrag))
		return;
	uvec4 loc = imageLoad(u_voxelPos,int(thxId));
	int childIdx = 0;
	uint node;
	bool bFlag = true;
	uint voxelDim = uint(u_voxelDim);
	uvec3 umin = uvec3(0,0,0);

	node = imageLoad(u_octreeIdx,childIdx).r;

	//将颜色值插入到对应的叶子节点中
	for(int i=0;i<u_level;i++){
		voxelDim /=2u;
		if((node & 0x80000000) ==0){
			bFlag = false;
			break;
		}
		childIdx = int(node& 0x7FFFFFFF);
		 if( ( loc.x >= umin.x && loc.x < umin.x+voxelDim ) &&
		    ( loc.y >= umin.y && loc.y < umin.y+voxelDim ) &&
			( loc.z >= umin.z && loc.z < umin.z+voxelDim )
		  )
	    {
		    
		}
		else if(
            (loc.x >= umin.x+voxelDim && loc.x < umin.x + 2u*voxelDim) &&
		    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
			(loc.z >= umin.z && loc.z < umin.z+voxelDim )  
		)
		{
		    childIdx += 1;
		    umin.x = umin.x+voxelDim;
	    }
		else if(
		    (loc.x >= umin.x && loc.x < umin.x+voxelDim) &&
		    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + 2u*voxelDim )
		)
		{
		    childIdx += 2;
			umin.z += voxelDim;
		}
		else if(
		    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2u*voxelDim) &&
		    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + 2u*voxelDim) 
		)
		{
		    childIdx += 3;
			umin.x += voxelDim;
			umin.z += voxelDim;
		}
		else if(
		    (loc.x >= umin.x && loc.x < umin.x + voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2u*voxelDim) &&
			(loc.z >= umin.z && loc.z < umin.z + voxelDim )
		)
		{
		    childIdx += 4;
			umin.y += voxelDim;
		
		}
		else if(
		    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2u*voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2u*voxelDim) &&
			(loc.z >= umin.z && loc.z < umin.z + voxelDim) 
		)
		{
		    childIdx += 5;
			umin.x += voxelDim;
			umin.y += voxelDim;
		}
		else if(
		    (loc.x >= umin.x && loc.x < umin.x + voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2u*voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2u) 
		)
		{
		    childIdx += 6;
			umin.z += voxelDim;
			umin.y += voxelDim;
		}
		else if(
		    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2u*voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2u*voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2u) 
		)
	    {
		    childIdx += 7;
			umin += voxelDim;
		}
		else
		{
		    bFlag = false;
			break;
		}
		node = imageLoad( u_octreeIdx, childIdx ).r;

	}
	vec4 color = imageLoad(u_voxelKd,int(thxId));
	

	//Use a atomic running average method to prevent buffer saturation
	//From OpenGL Insight ch. 22
	imageAtomicRGBA8Avg( color, childIdx, u_octreeKd );

//	//标志为叶子节点
//	uint leafnode = imageLoad(u_octreeIdx,childIdx).r;
//	leafnode|=0x40000000u;
//	imageStore(u_octreeIdx,int(childIdx),uvec4(leafnode,0,0,0));


}
//UINT atomic running average method
//From OpenGL Insight ch. 22
vec4 convRGBA8ToVec4( in uint val )
{
    return vec4( float( (val&0x000000FFu) ), float( (val&0x0000FF00u)>>8u),
	             float( (val&0x00FF0000u)>>16u), float( (val&0xFF000000u)>>24u) );
}

uint convVec4ToRGBA8( in vec4 val )
{
    return ( uint(val.w)&0x000000FFu)<<24u | (uint(val.z)&0x000000FFu)<<16u | (uint(val.y)&0x000000FFu)<<8u | (uint(val.x)&0x000000FFu);
}
void imageAtomicRGBA8Avg( vec4 val, int coord, layout(r32ui) uimageBuffer buf )
{
    val.rgb *= 255.0;
	val.a = 1;

	uint newVal = convVec4ToRGBA8( val );
	uint prev = 0u;
	uint cur;
	
	
//	uint imageAtomicCompSwap(gimage1D image,
// 	int P,
// 	uint compare,
// 	uint data);
//	

	//imageAtomicCompSwap atomically compares the value of compare with that of the texel at coordinate
	//P and sample (for multisampled forms) in the image bound to uint image. If the values are equal, 
	//data is stored into the texel, otherwise it is discarded.
	//It returns the original value of the texel regardless of the result of the comparison operation.
	while( (cur = imageAtomicCompSwap( buf, coord, prev, newVal ) ) != prev )
   {
       prev = cur;
	   vec4 rval = convRGBA8ToVec4( cur );
	   rval.xyz = rval.xyz*rval.w;
	   vec4 curVal = rval +  val;
	   curVal.xyz /= curVal.w;
	   newVal = convVec4ToRGBA8( curVal );
   }

     
}
