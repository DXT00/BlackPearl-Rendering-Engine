#include "pch.h"
#include "Component/CameraComponent/OrthographicCamera.h"

namespace BlackPearl {
    void OrthographicCamera::SetRange(float left, float right, float top, float bottom)
    {
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, -1.0f, 1.0f);
        RecalculateViewMatrix();

    }
}