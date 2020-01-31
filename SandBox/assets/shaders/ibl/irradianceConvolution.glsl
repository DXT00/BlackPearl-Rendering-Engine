#type vertex
#version 430 core


layout(location = 0) in vec3 aPos;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

out vec3 v_FragPos;
void main()
{
	v_FragPos = aPos;
	gl_Position = u_ProjectionView*vec4(aPos,1.0);

}


#type fragment
#version 430 core


in vec3 v_FragPos;
out vec4 FragColor;

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main(){
	//采样的方向等于半球的方向
	vec3 worldup = vec3(0.0,1.0,0.0);
		vec3 irradiance = vec3(0.0);
	vec3 N = normalize(v_FragPos);//TODO::
	vec3 right = cross(worldup,N);
	vec3 up = cross(N,right);


	float sampleStep=0.025;
	float nrSamples = 0.0;
	//convolution 
	for(float phi=0.0; phi< 2.0*PI;phi+=sampleStep){
		for(float theta=0.0;theta<0.5*PI;theta+=sampleStep){
			
			//spherical to catesian coordinate
			vec3 sphereCoords = vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
			// tangent space to world
			vec3 direction = sphereCoords.x*right+sphereCoords.y*up+sphereCoords.z*N;
			irradiance += texture(u_EnvironmentMap,direction).rgb*sin(theta)*cos(theta);

			nrSamples++;

		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	FragColor =vec4(irradiance, 1.0);

}