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
		glm::vec3& GetPosition();
		glm::vec3& GetAngles();
		glm::mat4 GetViewMatrix() const;
	private:
		glm::vec3 position;
		glm::vec3 angles;
	};
}