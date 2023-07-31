#include "Drawable.h"
#include <Core/third/glm/gtx/wrap.hpp>

#include "Model/StaticMesh.h"

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

	Drawable::Drawable(IGraphics& gfx, const Material& mat, const aiMesh& mesh, float scale)
	{
		pMesh = std::make_shared<StaticMesh>();
	}

	void Drawable::Draw(IGraphics& gfx)
	{
		pMesh->Bind(gfx);
		pTransformCBuf->Bind();
		for (auto& technique : techniques)
		{
			technique.Draw(gfx);
		}
	}
	void Drawable::InitTransformCBuf(IGraphics& gfx)
	{
		pTransformCBuf = gfx.CreateTransformCBuf();
	}
	Mesh& Drawable::GetMesh() const
	{
		return *pMesh;
	}
	IPolyVBuffer& Drawable::GetVertexBuffer() const
	{
		return *pMesh->GetPolyVBufer();
	}
	uint32_t Drawable::GetIndexCount() const
	{
		return pMesh->GetIndexCount();
	}
	void Drawable::AddTechnique(Technique technique)
	{
		techniques.push_back(std::move(technique));
	}
	glm::mat4 Drawable::GetTransformMatrix() const
	{
		return glm::translate(glm::yawPitchRoll(angles.x, angles.y, angles.z), pos);
	}
	void Drawable::BindParent()
	{
		pTransformCBuf->BindParent(*this);
		for (auto& tech : techniques)
		{
			tech.BindParent(*this);
		}
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
