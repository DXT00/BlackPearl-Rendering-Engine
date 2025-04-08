#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aPrePos;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aNormal;

out vec2 TexCoords;
		
layout(std140, binding = 0) uniform ForwardShadingViewConstants {
   //mat4 viewMatrix;
   //mat4 projectionMatrix;
   //vec3 cameraPosition;
   //float padding; 
    vec2      viewportOrigin;
    vec2      viewportSize;

    mat4 matProjectionView;
    mat4 matView;
    mat4 matProjection;
    vec3 cameraPos;
    vec3 cameraRot;
} g_ForwardView;

		
layout(std140, binding = 1) uniform TransformConstants {
    mat4 matModel;

} g_Transform;



uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
	TexCoords = aTexCoords;
	//gl_Position = u_ProjectionView* u_Model * vec4(aPos,1.0);
    gl_Position = g_ForwardView.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);
}


#type fragment
#version 430 core
out vec4 FragColor;
in vec2 TexCoords;


uniform Material u_Material;

float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth){
	float z = depth*2.0-1.0;//Back to NDC coordinate
	return 2.0*near*far /(far+near - z*(far - near));

}
void main(){
    MaterialTextureSample texSamples = SampleMaterialTexturesAuto(TexCoords);
	FragColor = texSamples.baseOrDiffuse;//vec4(1.0,1.0,0.0,1.0);// vec4(u_Material.diffuseColor,0.2);
//	FragColor =  vec4(1.0,0.0,0.0,1.0);//texture(u_Material.diffuse,TexCoords);//*vec4(u_Material.diffuseColor,1.0);
//	float depth = LinearizeDepth(gl_FragCoord.z)/far;
//	FragColor=vec4(vec3(depth),1.0);
}
