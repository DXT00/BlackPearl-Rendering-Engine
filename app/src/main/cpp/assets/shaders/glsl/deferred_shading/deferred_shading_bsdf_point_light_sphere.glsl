#type vertex

#version 450 core

#ifdef GL_ES
precision mediump float;  // 必须声明精度（ES 要求）
#endif

layout(location = Slot_aPos) in vec3 aPos;
layout(location = Slot_aNormal) in vec3 aNormal;
layout(location = Slot_aTexCoords) in vec2 aTexCoords;

//out vec2 v_TexCoord;
//out vec3 v_Normal;
//out vec3 v_FragPos;


#include <common/CommonViewStruct.glsl>
#include <common/CommonTransformStruct.glsl>

void main()
{
    gl_Position = g_View.matProjectionView * g_Transform.matModel * vec4(aPos,1.0);

    //v_TexCoord = aTexCoords;


}


#type fragment
#version 450 core




out vec4 FragColor;


//in vec2 v_TexCoord;



void main(){
    // 不需要实际输出颜色（禁用颜色写入时会被忽略）
    FragColor = vec4(0.0); 
}
