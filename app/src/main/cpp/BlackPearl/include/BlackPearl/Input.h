#pragma once
#include <utility>
namespace BlackPearl {
    class MainCamera;
	class Input {

	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		inline static bool IsFirstMouse() { return s_FirstMouse; }
		inline static void SetFirstMouse(bool flag) { s_FirstMouse = flag; }

        static void Update(MainCamera* mainCamera, float ts);
	private:
		static bool s_FirstMouse;//第一次获取鼠标位置


        static float m_LastMouseX;
        static float m_LastMouseY;


	};
}