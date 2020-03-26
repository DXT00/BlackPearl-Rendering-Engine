#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

#define CORE_SIZE 21
out vec2 TexCoords;
out vec2 blurTexCoords[CORE_SIZE];
uniform float u_ScreenWidth;
void main()
{
	TexCoords = aTexCoords;

	vec2 center = (aPos*0.5+vec3(0.5)).xy;
	float pixelSize = 1.0/u_ScreenWidth;
	for(int i=-CORE_SIZE/2;i<=CORE_SIZE/2;i++){
		blurTexCoords[i+CORE_SIZE/2] = center + vec2(0.0,float(pixelSize*i));

	}
	gl_Position = vec4(aPos,1.0);
}

#type fragment
#version 430 core

out vec4 FragColor;
//in vec2 TexCoords;
//CORE_SIZE必须是奇数
#define CORE_SIZE 21

in vec2 blurTexCoords[CORE_SIZE];

uniform sampler2D gPosition;
uniform sampler2D u_FinalScreenTexture;
uniform sampler2D u_BlurHTexture;

uniform Settings u_Settings;
const float offset = 1.0 / 300.0; 

//sigma = 2 ,kernel size = 11
//Guassian calculator: http://dev.theomader.com/gaussian-kernel-calculator/
float kernel[11]={
	0.0093,
	0.028002,
	0.065984,
	0.121703,
	0.175713,
	0.198596,
	0.175713,
	0.121703,	
	0.065984,
	0.028002,
	0.0093
};
//sigma = 3 ,kernel size = 21
float kernel21[21]={
	
	0.000539,
	0.001533,
	0.003908,
	0.008925,
	0.018255,
	0.033446,
	0.054891,
	0.080693,
	0.106259,
	0.125337,
	0.132429,
	0.125337,
	0.106259,
	0.080693,
	0.054891,
	0.033446,
	0.018255,
	0.008925,
	0.003908,
	0.001533,
	0.000539


};
//sigma =5 ,kernel size = 31

float kernel31[31]={
	0.0009,
	0.001604,
	0.002748,
	0.004523,
	0.007154,
	0.010873,
	0.01588,
	0.022285,
	0.030051,
	0.038941,
	0.048488,
	0.058016,
	0.066703,
	0.073694,
	0.078235,
	0.07981,
	0.078235,
	0.073694,
	0.066703,
	0.058016,
	0.048488,
	0.038941,
	0.030051,
	0.022285,
	0.01588,
	0.010873,
	0.007154,
	0.004523,
	0.002748,
	0.001604,
	0.0009	
};
//sigma = 6 ,kernel size = 41
float kernel41[41]={
	0.00026 ,
	0.000447,
	0.000746,
	0.001212,
	0.001914,
	0.002941,
	0.004396,
	0.00639	,
	0.009035,
	0.012427,
	0.016624,	
	0.021631,	
	0.027377,	
	0.033702,	
	0.040354,	
	0.046998,	
	0.053241,	
	0.058664,	
	0.062872,	
	0.065541,	
	0.066456,	
	0.065541,	
	0.062872,	
	0.058664,	
	0.053241,
	0.046998,
	0.040354,
	0.033702,	
	0.027377,	
	0.021631,	
	0.016624,	
	0.012427,	
	0.009035,	
	0.00639	,
	0.004396,
	0.002941,
	0.001914,	
	0.001212,	
	0.000746,	
	0.000447,	
	0.00026	

};
void main(){

	vec3 color =texture(u_BlurHTexture, blurTexCoords[CORE_SIZE/2]).rgb;
	int Guassian_filter = int(texture(u_FinalScreenTexture, blurTexCoords[CORE_SIZE/2]).a);
	int ObjectId = int(mod(texture(gPosition,blurTexCoords[CORE_SIZE/2]).a ,256));
	int isPbrObj = int(texture(gPosition,blurTexCoords[CORE_SIZE/2]).a /256);

	//PBR object不做滤波
	if(Guassian_filter==1 && isPbrObj==0){
		if(u_Settings.guassian_vertical){
			if(u_Settings.showBlurArea)
				color=vec3(1,0,0);
			else{
				color = vec3(0.0);
				for(int i=0;i<CORE_SIZE;i++){
					int i_objectId = int(mod(texture(gPosition,blurTexCoords[i]).a ,256));

					if(texture(u_FinalScreenTexture, blurTexCoords[i]).a == 1 && i_objectId == ObjectId)//排除specular和diffuse边界模糊
						color+=texture(u_BlurHTexture, blurTexCoords[i]).rgb*kernel21[i];
					else
						color+=texture(u_BlurHTexture, blurTexCoords[CORE_SIZE/2]).rgb*kernel21[i];

			}
			}
		}
	}

	if(u_Settings.hdr){
	//HDR tonemapping
	 color = color / (color + vec3(1.0));
	//gamma correction
    color = pow(color, vec3(1.0/2.2));  //1.0/2.2
	}
	FragColor = vec4(color, 1.0);

}
