#include "Camera.h"

namespace tryn::gfx
{
    void Camera::Bind(IGraphics& gfx)
    {
        gfx.SetCamera(GetViewMatrix());
    }
    void Camera::Update()
    {
        direction.x = cos(glm::radians(angles.x)) * cos(glm::radians(angles.y));
        direction.y = sin(glm::radians(angles.y));
        direction.z = sin(glm::radians(angles.x)) * cos(glm::radians(angles.y));
    }
    glm::vec3& Camera::GetPosition()
    {
        return position;
    }
    glm::vec3& Camera::GetDirection()
    {
        return direction;
    }
    glm::vec3& Camera::GetAngles()
    {
        return angles;
    }
    glm::mat4 Camera::GetViewMatrix() const
    {
        constexpr auto upDirection = glm::vec3(0, 1.0f, 0);
        return glm::lookAtLH(position, position + direction, upDirection);
    }
}

