#include "pch.h"
#include "KeyCodes.h"
#include "BlackPearl/RHI/DynamicRHI.h"
namespace BlackPearl {
	//extern DynamicRHI::Type g_RHIType;

	std::map<int, int> KeyCodes::s_OpenGLKeyMap = {
		{BP_KEY_SPACE              ,32},
		{BP_KEY_APOSTROPHE         ,39}, /* ' */
		{BP_KEY_COMMA              ,44}, /* , */
		{BP_KEY_MINUS              ,45}, /* - */
		{BP_KEY_PERIOD             ,46}, /* . */
		{BP_KEY_SLASH              ,47}, /* / */
		{BP_KEY_0                  ,48},
		{BP_KEY_1                  ,49},
		{BP_KEY_2                  ,50},
		{BP_KEY_3                  ,51},
		{BP_KEY_4                  ,52},
		{BP_KEY_5                  ,53},
		{BP_KEY_6                  ,54},
		{BP_KEY_7                  ,55},
		{BP_KEY_8                  ,56},
		{BP_KEY_9                  ,57},
		{BP_KEY_SEMICOLON          ,59}, /* ; */
		{BP_KEY_EQUAL              ,61}, /* = */
		{BP_KEY_A                  ,65},
		{BP_KEY_B                  ,66},
		{BP_KEY_C                  ,67},
		{BP_KEY_D                  ,68},
		{BP_KEY_E                  ,69},
		{BP_KEY_F                  ,70},
		{BP_KEY_G                  ,71},
		{BP_KEY_H                  ,72},
		{BP_KEY_I                  ,73},
		{BP_KEY_J                  ,74},
		{BP_KEY_K                  ,75},
		{BP_KEY_L                  ,76},
		{BP_KEY_M                  ,77},
		{BP_KEY_N                  ,78},
		{BP_KEY_O                  ,79},
		{BP_KEY_P                  ,80},
		{BP_KEY_Q, 81},
		{ BP_KEY_R                  ,82},
		{ BP_KEY_S                  ,83},
		{ BP_KEY_T                  ,84},
		{ BP_KEY_U                  ,85},
		{ BP_KEY_V                  ,86},
		{ BP_KEY_W                  ,87},
		{ BP_KEY_X                  ,88},
		{ BP_KEY_Y                  ,89},
		{ BP_KEY_Z                  ,90},
		{ BP_KEY_LEFT_BRACKET       ,91}, /* [ */
		{ BP_KEY_BACKSLASH          ,92}, /* \ */
		{ BP_KEY_RIGHT_BRACKET      ,93}, /* ] */
		{ BP_KEY_GRAVE_ACCENT       ,96}, /* ` */
		{ BP_KEY_WORLD_1            ,161}, /* non-US #1 */
		{ BP_KEY_WORLD_2            ,162}, /* non-US #2 */
		/* Function keys */
		{ BP_KEY_ESCAPE             ,256},
		{ BP_KEY_ENTER              ,257},
		{ BP_KEY_TAB                ,258},
		{ BP_KEY_BACKSPACE          ,259},
		{ BP_KEY_INSERT             ,260},
		{ BP_KEY_DELETE             ,261},
		{ BP_KEY_RIGHT              ,262},
		{ BP_KEY_LEFT               ,263},
		{ BP_KEY_DOWN               ,264},
		{ BP_KEY_UP                 ,265},
		{ BP_KEY_PAGE_UP            ,266},
		{ BP_KEY_PAGE_DOWN          ,267},
		{ BP_KEY_HOME               ,268},
		{ BP_KEY_END                ,269},
		{ BP_KEY_CAPS_LOCK          ,280},
		{ BP_KEY_SCROLL_LOCK        ,281},
		{ BP_KEY_NUM_LOCK           ,282},
		{ BP_KEY_PRINT_SCREEN       ,283},
		{ BP_KEY_PAUSE              ,284},
		{ BP_KEY_F1                 ,290},
		{ BP_KEY_F2                 ,291},
		{ BP_KEY_F3                 ,292},
		{ BP_KEY_F4                 ,293},
		{ BP_KEY_F5                 ,294},
		{ BP_KEY_F6                 ,295},
		{ BP_KEY_F7                 ,296},
		{ BP_KEY_F8                 ,297},
		{ BP_KEY_F9                 ,298},
		{ BP_KEY_F10                ,299},
		{ BP_KEY_F11                ,300},
		{ BP_KEY_F12                ,301},
		{ BP_KEY_F13                ,302},
		{ BP_KEY_F14                ,303},
		{ BP_KEY_F15                ,304},
		{ BP_KEY_F16                ,305},
		{ BP_KEY_F17                ,306},
		{ BP_KEY_F18                ,307},
		{ BP_KEY_F19                ,308},
		{ BP_KEY_F20                ,309},
		{ BP_KEY_F21                ,310},
		{ BP_KEY_F22                ,311},
		{ BP_KEY_F23                ,312},
		{ BP_KEY_F24                ,313},
		{ BP_KEY_F25                ,314},
		{ BP_KEY_KP_0               ,320},
		{ BP_KEY_KP_1               ,321},
		{ BP_KEY_KP_2               ,322},
		{ BP_KEY_KP_3               ,323},
		{ BP_KEY_KP_4               ,324},
		{ BP_KEY_KP_5               ,325},
		{ BP_KEY_KP_6               ,326},
		{ BP_KEY_KP_7               ,327},
		{ BP_KEY_KP_8               ,328},
		{ BP_KEY_KP_9               ,329},
		{ BP_KEY_KP_DECIMAL         ,330},
		{ BP_KEY_KP_DIVIDE          ,331},
		{ BP_KEY_KP_MULTIPLY        ,332},
		{ BP_KEY_KP_SUBTRACT        ,333},
		{ BP_KEY_KP_ADD             ,334},
		{ BP_KEY_KP_ENTER           ,335},
		{ BP_KEY_KP_EQUAL           ,336},
		{ BP_KEY_LEFT_SHIFT         ,340},
		{ BP_KEY_LEFT_CONTROL       ,341},
		{ BP_KEY_LEFT_ALT           ,342},
		{ BP_KEY_LEFT_SUPER         ,343},
		{ BP_KEY_RIGHT_SHIFT        ,344},
		{ BP_KEY_RIGHT_CONTROL      ,345},
		{ BP_KEY_RIGHT_ALT          ,346},
		{ BP_KEY_RIGHT_SUPER        ,347},
		{ BP_KEY_MENU               ,348},
		{ BP_MOUSE_BUTTON_RIGHT     ,1}
	};

	std::map<int, int> KeyCodes::s_D3D12KeyMap = {
		{BP_KEY_A      ,VK_A},
		{BP_KEY_D      ,VK_D},
		{BP_KEY_E      ,VK_E},
		{BP_KEY_Q      ,VK_Q},
		{BP_KEY_S      ,VK_S},
		{BP_KEY_W      ,VK_W},
		{BP_MOUSE_BUTTON_RIGHT, VK_MOUSERIGHT}
	};
	int KeyCodes::Get(int key)
	{
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {
			if (s_OpenGLKeyMap.find(key) != s_OpenGLKeyMap.end()) {
				return s_OpenGLKeyMap[key];

			} else{
				GE_CORE_ERROR("can't find key" + std::to_string(key));
				return -1;
			}
	}
		else if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
			if (s_D3D12KeyMap.find(key) != s_D3D12KeyMap.end()) {
				return s_D3D12KeyMap[key];
			}
			else {
				GE_CORE_ERROR("can't find key" + std::to_string(key));
				return -1;
			}
		}
}

}