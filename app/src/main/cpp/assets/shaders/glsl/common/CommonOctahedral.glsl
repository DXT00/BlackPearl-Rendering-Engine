

#ifndef BP_COMMON_OCTAHEDRAL_H
#define BP_COMMON_OCTAHEDRAL_H
#include <assets/shaders/glsl/common/CommonMath.glsl>

// Octahedron Normal Vectors
// [Cigolle 2014, "A Survey of Efficient Representations for Independent Unit Vectors"]
//			bits	     Mean	Max
// oct		8:8			0.33709 0.94424
// snorm	8:8:8		0.17015 0.38588
// oct		10:10		0.08380 0.23467
// snorm	10:10:10	0.04228 0.09598
// oct		12:12		0.02091 0.05874

//doc: https://blog.csdn.net/qq_32095699/article/details/148279336
float2 UnitVectorToOctahedron(float3 N)
{
	N.xy /= dot( float3(1.0), abs(N) );
	if( N.z <= 0 )
	{
		N.xy = ( float2(1.0) - abs(N.yx) ) * select( bool2(N.x>0 , N.y>0), float2(1,1), float2(-1,-1) );
	}
	return N.xy;
}

float3 OctahedronToUnitVector( float2 Oct )
{
	float3 N = float3( Oct, 1 - dot( float2(1.0), abs(Oct) ) );
	float t = max( -N.z, 0 );
	N.xy += select( bool2(N.x>0 , N.y>0), float2(-t, -t), float2(t, t));
	return normalize(N);
}


float2 UnitVectorToHemiOctahedron(float3 N)
{
	N.xy /= dot( float3(1.0), abs(N) );
	return float2( N.x + N.y, N.x - N.y );
}

#endif //COMMON_OCTAHEDRAL_H