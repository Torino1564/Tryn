
#include "Flat.h"
#include <assimp/material.h>
#include <assimp/StringUtils.h>
#include <assimp/types.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Bindables/TextureResource.h>
#include <Core/src/gfx/Bindables/Rasterizer.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/PixelShader.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/Bindables/Sampler.h>

namespace tryn::gfx
{
	Flat::Flat(const std::string& name)
		:
	Technique(name)
	{
	}

	Flat::Flat(const std::vector<std::shared_ptr<Material>>& materials, const IGraphics& gfx, const bool instanced, const bool skinned)
		:
		Technique("Flat")
	{
		auto shaderRootPath = IGraphics::GetShaderRootPath();
		auto& material = *materials[0];
		std::string shaderCode = "Flat";



		Step step(gfx, "Lambertian");
		gfx.GetRenderGraph().AddRenderQueue("Lambertian");
		auto& vLayout = step.GetVertexLayout();

		// Common
		vLayout.AppendElement(VertexLayout::Position3D);
		vLayout.AppendElement(VertexLayout::Normal);
		ConstantBufferLayout cbLayout;

		// Albedo
		{
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3, "materialColor"));
			step.AddBindable(std::move(IRasterizer::Resolve(gfx)));
		}
		// Specular
		{
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float3, "specularColor"));
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "specularWeight"));
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "specularGloss"));
		}

		{
			if (skinned)
			{
				vLayout.AppendElement(VertexLayout::BoneIds);
				vLayout.AppendElement(VertexLayout::BoneWeights);
			}
			auto pvs = IVertexShader::Resolve(gfx, shaderRootPath + shaderCode + (instanced ? "Inst" : "") + (skinned ? "Skn" : "") + "_VS.cso");
			//step.AddBindable(IInputLayout::Resolve(gfx, vLayout, *pvs));
			step.AddBindable(std::move(pvs));
			step.AddBindable(IPixelShader::Resolve(gfx, shaderRootPath + shaderCode + "_PS.cso"));
			cbLayout.Solidify();
			auto buf = IPxConstantBuffer::Resolve(gfx, std::move(cbLayout), 1);

			if ((*buf)["materialColor"].Exists())
			{
				auto& param = (*buf)["materialColor"].Get<glm::vec3>();
				param = material.GetAttribute<glm::vec3>(AttributeType::DiffuseColor);
			}
			if ((*buf)["specularColor"].Exists())
			{
				auto& param = (*buf)["specularColor"].Get<glm::vec3>();
				param = material.GetAttribute<glm::vec3>(AttributeType::SpecularColor);
			}
			if ((*buf)["specularGloss"].Exists())
			{
				auto& param = (*buf)["specularGloss"].Get<float>();
				param = material.GetAttribute<float>(AttributeType::SpecularGloss);
			}
			if ((*buf)["specularWeight"].Exists())
			{
				auto& param = (*buf)["specularWeight"].Get<float>();
				param = 1.0f;
			}
			step.AddBindable(std::move(buf));
		}

		this->AddStep(std::move(step));
	}
}