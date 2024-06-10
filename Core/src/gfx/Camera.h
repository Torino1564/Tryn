#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtx/euler_angles.hpp>

namespace tryn::gfx
{
	class Camera
	{
	public:
		void Bind(IGraphics& gfx);
		void Update();
		void Rotate(float dx, float dy) noexcept;
		void Submit(IGraphics& gfx);
		void Translate(glm::vec3 translation) noexcept;
		glm::vec3& GetPosition();
		glm::vec3& GetDirection();
		glm::mat4 GetViewMatrix();
		const glm::mat4& GetViewOnViewMatrix();
		void ShowDebugInfo() const;
		void ShowControls();
		void SetYaw(const float yaw);
		void SetPitch(const float pitch);
	private:
		void Reset();
		glm::vec3 position = {0.0f,0.0f,0.0f};
		glm::vec3 direction = { 1.0f,0.0f,0.0f };
		float yaw = 0;
		float pitch = 0;
		float travelSpeed = 0.005f;
		float rotationSpeed = 0.1f;
		glm::mat4 viewMatrix = {};
	};
}