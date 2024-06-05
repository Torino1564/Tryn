#include "Drawable.h"
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/third/glm/gtx/wrap.hpp>
#include <Core/third/glm/gtx/transform.hpp>
#include <Core/third/glm/glm.hpp>
#include <Core/third/glm/gtx/euler_angles.hpp>
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <ranges>
#include <format>
#include <Core/src/utl/String.h>

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
		auto& techniques = pMaterial->GetTechniques();

		gfx.Dispatch([&] {
			BindBase();
			for (auto i = 0; i < techniques.size(); i++)
			{
				if (techniqueState[i] == true)
				{
					techniques[i]->Draw(gfx, this);
				}
			}
			});
	}
	void Drawable::Submit(IGraphics& gfx, glm::mat4 transform)
	{
		extraBindPtrs = {};
		ExtraSubmitBehavior();
		this->transform = transform;
		auto& techniques = pMaterial->GetTechniques();

		for (auto i = 0 ; i < techniques.size() ; i++)
		{
			if (techniqueState[i] == true)
			{
				techniques[i]->Submit(gfx, this);
			}
		}
	}
	void Drawable::Submit(IGraphics& gfx, std::span<const glm::mat4> transforms, InstancedModelParent& instancedParent)
	{
		extraBindPtrs = {};
		ExtraSubmitBehavior();

		auto& instanceBuffer = instancedParent.RequestInstanceBuffer(ID);
		auto& constantBuffer = instanceBuffer.GetCPUBuffer();
		auto instanceArray = constantBuffer["InstanceArray"];

		if (instanceArray.Node().Size() < transforms.size())
		{
			instanceArray.Resize(transforms.size() + 10);
		}

		auto updatedInstanceArray = constantBuffer["InstanceArray"];

		memset(constantBuffer.Data(), 0, constantBuffer.ByteSize());

		for (auto i = 0; i < transforms.size(); i++)
		{
			updatedInstanceArray[i]["transform"].Get<glm::mat4>() = transpose(transforms[i]);
		}

		AddExtraBind(&instanceBuffer);
		auto& techniques = pMaterial->GetTechniques();
		for (auto i = 0; i < techniques.size(); i++)
		{
			if (techniqueState[i] == true)
			{
				techniques[i]->Submit(gfx, this, transforms, instancedParent);
			}
		}
	}
	void Drawable::BindBase() const
	{
		pVertexBuffer->Bind();
		pIndexBuffer->Bind();
		pTopology->Bind();
	}
	void Drawable::BindBase(IContext& context) const
	{
		pVertexBuffer->Bind(context);
		pIndexBuffer->Bind(context);
		pTopology->Bind(context);
	}
	void Drawable::BindExtraBinds()
	{
		for (auto bindPtr : extraBindPtrs)
		{
			if (bindPtr != nullptr)
			{
				bindPtr->Bind();
			}
		}
	}
	void Drawable::BindExtraBinds(IContext& context)
	{
		for (auto bindPtr : extraBindPtrs)
		{
			if (bindPtr != nullptr)
			{
				bindPtr->Bind(context);
			}
		}
	}
	void Drawable::AddExtraBind(IBindable* pBindable)
	{
		for (auto& bindPtr : extraBindPtrs)
		{
			if (bindPtr == nullptr)
			{
				bindPtr = pBindable;
				return;
			}
		}
		trylog.warn(L"Failed to add extra bind: the 10 slot limit was reached.");
	}
	void Drawable::BindTransformCBuf() const
	{
		pTransformCBuf->BindTransformCBuf(this);
	}
	void Drawable::BindTransformCBuf(IContext& context) const
	{
		pTransformCBuf->BindTransformCBuf(this, context);
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
	glm::mat4 Drawable::GetTransformMatrix() const
	{
		return transform;
	}
	std::uint16_t Drawable::GetID() const
	{
		return ID;
	}
	void Drawable::SetMaterial(std::shared_ptr<class Material> pMat)
	{
		pMaterial = pMat;
		techniqueState.resize(pMaterial->GetTechniques().size(), true);
	}
	Material& Drawable::GetMaterial()
	{
		return *pMaterial;
	}
	void Drawable::SetTechniqueState(const std::string& name, const bool state)
	{
		auto& techniques = pMaterial->GetTechniques();
		// find specified technique state
		for (auto [index, pTechnique] : std::ranges::enumerate_view(techniques))
		{
			if (pTechnique->GetName() == name)
			{
				techniqueState[index] = state;
				return;
			}
		}
		trylog.warn(utl::ToWide(std::format("Could not find technique [{}] when setting its state in drawable[ID:{}]", name, this->ID)));
	}
	bool Drawable::GetTechniqueState(const std::string& name) const
	{
		auto& techniques = pMaterial->GetTechniques();
		// find specified technique state
		for (auto [index, pTechnique] : std::ranges::enumerate_view(techniques))
		{
			if (pTechnique->GetName() == name)
			{
				return techniqueState[index];
			}
		}
		trylog.warn(utl::ToWide(std::format("Could not find technique [{}] when getting its state in drawable[ID:{}]", name, this->ID)));
		return false;
	}
}