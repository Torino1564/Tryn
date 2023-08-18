#include "Camera.h"

namespace tryn::gfx
{
    void Camera::Bind(IGraphics& gfx)
    {
        gfx.SetCamera(GetViewMatrix());
    }
    glm::vec3& Camera::GetPosition()
    {
        return position;
    }
    glm::vec3& Camera::GetAngles()
    {
        return angles;
    }
    glm::mat4 Camera::GetViewMatrix() const
    {
        constexpr auto focusPoint = glm::vec3(0, 0, 0);
        constexpr auto upDirection = glm::vec3(0, 1, 0);
        return glm::lookAtLH(position, focusPoint, upDirection);
    }
}

