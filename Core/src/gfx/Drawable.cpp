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

	void Drawable::Draw(IGraphics& gfx, glm::mat4 transform)
	{
		this->transform = transform;

		auto future = gfx.Dispatch([&] {
			pVertexBuffer->Bind();
			pIndexBuffer->Bind();
			pTopology->Bind();
			pTransformCBuf->BindTransformCBuf(this);

			for (auto& technique : techniques)
			{
				technique.Draw(gfx, this);
			}
			});
		future.get();
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
		return transform;
	}
}