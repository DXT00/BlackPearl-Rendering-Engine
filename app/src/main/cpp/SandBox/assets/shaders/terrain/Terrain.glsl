#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;


out vec2 v_TexCoords;

void main()
{
	v_TexCoords = aTexCoords;
	gl_Position =  vec4(aPos,1.0);
}


#type tessellation_control_shader
#version 450 core
// The tessellation control shader output the tessellation levels
// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=4) out;

// varying input from vertex shader
in vec2 v_TexCoords[];
// varying output to evaluation shader
out vec2 TextureCoord[];
uniform int u_DynamicTessLevel;
uniform int u_TessLevel;

uniform mat4 u_Model;           // the model matrix
uniform mat4 u_View;            // the view matrix
/*
gl_in and gl_out which are both arrays of the following struct type:
in gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[gl_MaxPatchVertices];
*/
void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = v_TexCoords[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    // This will be result in 16x16 tessellated points to be generated.
    if (gl_InvocationID == 0)
    {
        if (u_DynamicTessLevel == 1) {
            // ----------------------------------------------------------------------
            // Step 1: define constants to control tessellation parameters
	        // set these as desired for your world scale
            const int MIN_TESS_LEVEL = 4;
            const int MAX_TESS_LEVEL = 64;
            const float MIN_DISTANCE = 20;
            const float MAX_DISTANCE = 800;

            // ----------------------------------------------------------------------
            // Step 2: transform each vertex into eye space
            vec4 eyeSpacePos00 = u_View * u_Model * gl_in[0].gl_Position;
            vec4 eyeSpacePos01 = u_View * u_Model * gl_in[1].gl_Position;
            vec4 eyeSpacePos10 = u_View * u_Model * gl_in[2].gl_Position;
            vec4 eyeSpacePos11 = u_View * u_Model * gl_in[3].gl_Position;

            // ----------------------------------------------------------------------
            // Step 3: "distance" from camera scaled between 0 and 1
            float distance00 = clamp((abs(eyeSpacePos00.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
            float distance01 = clamp((abs(eyeSpacePos01.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
            float distance10 = clamp((abs(eyeSpacePos10.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
            float distance11 = clamp((abs(eyeSpacePos11.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

            // ----------------------------------------------------------------------
            // Step 4: interpolate edge tessellation level based on closer vertex
            float tessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00) );
            float tessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01) );
            float tessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11) );
            float tessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10) );

            // ----------------------------------------------------------------------
            // Step 5: set the corresponding outer edge tessellation levels
            gl_TessLevelOuter[0] = tessLevel0;
            gl_TessLevelOuter[1] = tessLevel1;
            gl_TessLevelOuter[2] = tessLevel2;
            gl_TessLevelOuter[3] = tessLevel3;

            // ----------------------------------------------------------------------
            // Step 6: set the inner tessellation levels to the max of the two parallel edges
            gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
            gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
        }
        else {
                //four edges:
            gl_TessLevelOuter[0] = u_TessLevel;
            gl_TessLevelOuter[1] = u_TessLevel;
            gl_TessLevelOuter[2] = u_TessLevel;
            gl_TessLevelOuter[3] = u_TessLevel;

            //two dimensions;
            gl_TessLevelInner[0] = u_TessLevel;
            gl_TessLevelInner[1] = u_TessLevel;
        }

    }
}


#type tessellation_evaluation_shader
#version 450 core
layout (quads, fractional_odd_spacing, ccw) in;

struct Material{
    sampler2D height;  // the texture corresponding to our height map
};
uniform Material u_Material;
uniform mat4 u_Model;           // the model matrix
uniform mat4 u_ProjectionView;            // the view matrix

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];

// send to Fragment Shader for coloring
out float Height;

void main()
{
    // get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height and scale + shift as desired
    Height = texture(u_Material.height, texCoord).y * 64.0 - 16.0;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    p += normal * Height;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = u_ProjectionView* u_Model * p;
}



#type fragment
#version 450 core

in float Height;

out vec4 FragColor;

void main()
{
	float h = (Height + 16)/64.0f;
	FragColor = vec4(h, h, h, 1.0);
}