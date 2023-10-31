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
		glm::mat4 GetViewMatrix() const;
		void ShowDebugInfo() const;
	private:
		glm::vec3 position = {0.0f,0.0f,0.0f};
		glm::vec3 direction = { 1.0f,0.0f,0.0f };
		float yaw;
		float pitch;
		float travelSpeed = 0.005f;
		float rotationSpeed = 0.1f;
	};
}