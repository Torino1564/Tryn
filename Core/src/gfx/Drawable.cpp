#include "Drawable.h"
#include <Core/src/gfx/Bindables/VertexBuffer.h>
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/third/glm/gtx/wrap.hpp>
#include <Core/third/glm/gtx/transform.hpp>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtx/euler_angles.hpp>

namespace tryn::gfx
{
	template<typename T>
	T wrap_angle(T theta) noexcept
	{
		constexpr T twoPi = T(2) * T(glm::pi<double>());
		const T mod = T(fmod(theta, twoPi));
		if (mod > T(glm::pi<double>()))
		{
			return mod - twoPi;
		}
		else if (mod < -static_cast<T>(glm::pi<double>()))
		{
			return mod + twoPi;
		}
		return mod;
	}

	void Drawable::Draw(IGraphics& gfx)
	{
		pVertexBuffer->Bind();
		pIndexBuffer->Bind();
		pTopology->Bind();
		pTransformCBuf->BindTransformCBuf(this);

		for (auto& technique : techniques)
		{
			technique.Draw(gfx,this);
		}
	}
	void Drawable::InitTransformCBuf(IGraphics& gfx)
	{
		pTransformCBuf = gfx.CreateTransformCBuf();
	}
	IVertexBuffer& Drawable::GetVertexBuffer() const
	{
		return *pVertexBuffer;
	}
	uint32_t Drawable::GetIndexCount() const
	{
		return indexCount;
	}
	void Drawable::AddTechnique(Technique technique)
	{
		techniques.push_back(std::move(technique));
	}
	glm::mat4 Drawable::GetTransformMatrix() const
	{
		return glm::translate(glm::yawPitchRoll(angles.x, angles.y, angles.z), pos);
	}

	void Drawable::SetYaw(const float yaw)
	{
		angles.x = std::clamp(yaw, -glm::pi<float>(), glm::pi<float>());
	}

	void Drawable::SetPitch(const float pitch)
	{
		angles.x = std::clamp(pitch, -glm::half_pi<float>(), glm::half_pi<float>());
	}

	void Drawable::SetRoll(const float roll)
	{
		angles.x = std::clamp(roll, -glm::pi<float>(), glm::pi<float>());
	}

	float& Drawable::GetYaw()
	{
		return angles.x;
	}

	float& Drawable::GetPitch()
	{
		return angles.y;
	}

	float& Drawable::GetRoll()
	{
		return angles.z;
	}
}