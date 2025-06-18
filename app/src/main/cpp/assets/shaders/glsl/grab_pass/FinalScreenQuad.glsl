#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aNormal) in vec3 aNormal;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;


void main()
{
	v_TexCoord = aTexCoords;
	gl_Position = vec4(aPos,1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;
in vec2 v_TexCoord;


#if USE_GLES_PLS

__pixel_localEXT FragData {
    layout(rgb10_a2)  vec4 t_gSceneColor;
} pls;

#else
    layout(binding = 0) uniform sampler2D t_gSceneColor;
#endif


void main(){

#if USE_GLES_PLS
    vec3 color = pls.t_gSceneColor.rgb;
#else
    vec3 color = texture(t_gSceneColor, v_TexCoord).rgb;
#endif

//	bool hdr = u_Settings.hdr;
//	if(hdr){
//	//HDR tonemapping
//	 color = color / (color + vec3(1.0));
//	//gamma correction
//    color = pow(color, vec3(1.0/2.2));  //1.0/2.2
//	}
	FragColor = vec4(color,1.0);//vec4(1.0,0.0,0.0, 1.0);

}
