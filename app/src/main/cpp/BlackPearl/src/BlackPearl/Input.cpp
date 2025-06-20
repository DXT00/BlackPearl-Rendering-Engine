#include "pch.h"
#include "Input.h"
//#include <GLFW/glfw3.h>
#include "Application.h"
namespace BlackPearl {

	bool Input::s_FirstMouse = true;

    float Input::m_LastMouseX = 0.0;
    float Input::m_LastMouseY = 0.0;

	bool Input::IsKeyPressed(int keycode)
	{
		return Application::Get().GetWindow().IsKeyPressed(keycode);
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		return Application::Get().GetWindow().IsMouseButtonPressed(button);
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		return Application::Get().GetWindow().GetMousePosition();
	}

	float Input::GetMouseX()
	{
		auto[x, y] = GetMousePosition();
		return x;
	}

	float Input::GetMouseY()
	{
		auto[x, y] = GetMousePosition();
		return y;
	}



    void Input::Update(MainCamera* mainCamera, float ts) {

        glm::vec3 m_CameraPosition = mainCamera->GetPosition();

        CameraRotation m_CameraRotation(mainCamera->Yaw(), mainCamera->Pitch());
        // todo::android  platform
#ifdef GE_PLATFORM_WINDOWS
            float maxMoveDelta = 5 * mainCamera->GetMoveSpeed();
        float moveDelta = mainCamera->GetMoveSpeed();// *ts;

        if (moveDelta > maxMoveDelta)
            moveDelta = maxMoveDelta;

        if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_W))) {
            /*if (DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
                m_CameraPosition -= mainCamera->Front() * moveDelta;
            }*/
            //else {
            m_CameraPosition += mainCamera->Front() * moveDelta;
            //}
        }
        else if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_S))) {
            /*if (DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
                m_CameraPosition += mainCamera->Front() * moveDelta;
            }
            else {*/
            m_CameraPosition -= mainCamera->Front() * moveDelta;
            //}
        }
        if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_A))) {
            if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
                m_CameraPosition -= (-mainCamera->Right()) * moveDelta;
            }
            else if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL ||
                DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
                m_CameraPosition -= mainCamera->Right() * moveDelta;
            }
        }
        else if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_D))) {
            if (DynamicRHI::g_RHIType == DynamicRHI::Type::D3D12) {
                m_CameraPosition += (-mainCamera->Right()) * moveDelta;
            }
            else if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL ||
                DynamicRHI::g_RHIType == DynamicRHI::Type::Vulkan) {
                m_CameraPosition += mainCamera->Right() * moveDelta;
            }
        }
        if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_E))) {
            m_CameraPosition += mainCamera->Up() * moveDelta;
        }
        else if (Input::IsKeyPressed(KeyCodes::Get(BP_KEY_Q))) {
            m_CameraPosition -= mainCamera->Up() * moveDelta;
        }
        // ---------------------Rotation--------------------------------------

        float posx = Input::GetMouseX();
        float posy = Input::GetMouseY();
        if (Input::IsMouseButtonPressed(KeyCodes::Get(BP_MOUSE_BUTTON_RIGHT))) {
            if (Input::IsFirstMouse()) {
                Input::SetFirstMouse(false);
                m_LastMouseX = posx;
                m_LastMouseY = posy;
            }
            float diffx = posx - m_LastMouseX;
            float diffy = -posy + m_LastMouseY;

            m_LastMouseX = posx;
            m_LastMouseY = posy;
            math::vector<int, 2> windowSize = Application::Get().GetWindow().GetCurWindowSize();

            float degreesPerPixelX = (mainCamera->Fov() / (float)windowSize.x);// *camera.data.aspect;
            float degreesPerPixelY = (mainCamera->Fov() / (float)windowSize.y);



            float deltaX = diffx * degreesPerPixelX * mainCamera->GetRotateSpeed();//ts *
            float deltaY = diffy * degreesPerPixelY * mainCamera->GetRotateSpeed();//ts *

            //float maxRotDelta = 3 * mainCamera->GetRotateSpeed();
            //if (deltaX > maxRotDelta)
            //	deltaX = maxRotDelta;
            //if (deltaY > maxRotDelta)
            //	deltaY = maxRotDelta;

            //GE_CORE_INFO("Cam deltaX = " + std::to_string(deltaX) + "Cam deltaY =" + std::to_string(deltaY));

            float raoteSpeed = mainCamera->GetRotateSpeed();
            m_CameraRotation.Yaw += deltaX;
            m_CameraRotation.Pitch += deltaY;

            /*	if (m_CameraRotation.Pitch > 89.0f)
                    m_CameraRotation.Pitch = 89.0f;
                if (m_CameraRotation.Pitch < -89.0f)
                    m_CameraRotation.Pitch = -89.0f;*/

                    /*	if (m_CameraRotation.Yaw > 0.0f)
                            m_CameraRotation.Yaw = 0.0f;
                        if (m_CameraRotation.Yaw < -360.0f)
                            m_CameraRotation.Yaw = -360.0f;*/
            if (math::abs(m_CameraRotation.Pitch) >= 90) m_CameraRotation.Pitch = 0.f;

            if (math::abs(m_CameraRotation.Yaw) >= 360.0) m_CameraRotation.Yaw = 0.f;



            mainCamera->SetRotation({ m_CameraRotation.Pitch, m_CameraRotation.Yaw, 0.0f });
        }
        else {
            m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
            m_LastMouseY = posy;
        }

        mainCamera->SetPosition(m_CameraPosition);

        //GE_CORE_INFO("Cam Pos = " + std::to_string(m_CameraPosition.x) + "," + std::to_string(m_CameraPosition.y) + "," + std::to_string(m_CameraPosition.z));
        //GE_CORE_INFO("Cam Pitch = " + std::to_string(m_CameraRotation.Pitch) + "Cam Yaw =" + std::to_string(m_CameraRotation.Yaw));
#else

            AndroidInputManager::GetInstance()->Tick(mainCamera, m_CameraPosition, m_CameraRotation);
#endif

    }
}