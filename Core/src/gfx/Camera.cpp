#include "Camera.h"
#include <Core/third/glm/gtx/matrix_decompose.hpp>
#include <imgui.h>

namespace tryn::gfx
{
    static float RollOver(float value, float min, float max)
    {
        trynass(min < max);
        if (value < min)
        {
            return max - (std::abs(value - min));
        }
        else if (value > max)
        {
            return min + (std::abs(max - value));
        }
        else
        {
            return value;
        }
    }
    void Camera::Bind(IGraphics& gfx)
    {
        auto future = gfx.Dispatch([&] {
            gfx.SetCamera(GetViewMatrix());
            });
        future.get();
    }
    void Camera::Update()
    {
        static constexpr glm::vec4 baseVector = { 0.0f,0.0f,1.0f,0.0f };
        
        direction = glm::vec3(glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f) * baseVector);
    }
    void Camera::Rotate(float dx, float dy) noexcept
    {
        const auto tempYaw = yaw + (dx * rotationSpeed);
        yaw = RollOver(tempYaw, -180.0f, 180.0f);
        pitch = std::clamp(pitch + dy * rotationSpeed, -89.0f, 89.0f);
    }
    void Camera::Submit(IGraphics& gfx)
    {
        gfx.GetRenderGraph().AddCamera(this);
        gfx.SetCamera(GetViewMatrix());
    }
    void Camera::Translate(glm::vec3 translation) noexcept
    {
        glm::vec3 finalTranslation = glm::scale(glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f), glm::vec3(travelSpeed, travelSpeed, travelSpeed)) * glm::vec4(translation, 1.0f);

        position.x += finalTranslation.x;
        position.y += finalTranslation.y;
        position.z += finalTranslation.z;
    }
    void Camera::AbsoluteTranslate(glm::vec3 translation) noexcept
    {
        position += translation * travelSpeed;
    }
    glm::vec3& Camera::GetPosition()
    {
        return position;
    }
    glm::vec3& Camera::GetDirection()
    {
        return direction;
    }
    glm::mat4 Camera::GetViewMatrix() const
    {
        constexpr auto upDirection = glm::vec3(0, 1.0f, 0);
        return glm::lookAtLH(position, position + direction, upDirection);
    }
    void Camera::ShowDebugInfo() const
    {
        if (ImGui::Begin("Camera debug info"))
        {
            ImGui::Text("Position");
            ImGui::Text("X: %f", position.x);
            ImGui::Text("Y: %f", position.y);
            ImGui::Text("Z: %f", position.z);
            ImGui::Text("Orientation");
            ImGui::Text("Yaw: %f", yaw);
            ImGui::Text("Pitch: %f", pitch);
            ImGui::Text("Direction");
            ImGui::Text("X: %f", direction.x);
            ImGui::Text("Y: %f", direction.y);
            ImGui::Text("Z: %f", direction.z);
            ImGui::End();
        }

    }
    void Camera::ShowControls()
    {
        if (ImGui::Begin("Camera parameters"))
        {
            ImGui::SliderFloat("Travel Speed", &travelSpeed, 0.001f, 0.1f);
            ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.03f, 1.0f);
            if (ImGui::Button("Reset position"))
            {
                Reset();
            }
            
            ImGui::End();
        }
    }
    void Camera::Reset()
    {
        position = { 0.0f,0.0f,0.0f };
        direction = { 1.0f,0.0f,0.0f };
        yaw = 0;
        pitch = 0;
        travelSpeed = 0.005f;
        rotationSpeed = 0.1f;
    }
}

