#include "Flat.h"
#include <assimp/material.h>
#include <assimp/StringUtils.h>
#include <assimp/types.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Bindables/TextureResource.h>
#include <Core/src/gfx/Bindables/Rasterizer.h>
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/PixelShader.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/Bindables/Sampler.h>

namespace tryn::gfx
{
	template <bool Instanced, bool Skinned>
	FlatBase<Instanced, Skinned>::FlatBase(const std::string& name)
		:
	Technique<FlatBase, "FlatBase", Instanced, Skinned>(name)
	{
	}

	template <bool Instanced, bool Skinned>
	FlatBase<Instanced, Skinned>::FlatBase(Material& material, aiMaterial& aiMat, const IGraphics& gfx, const std::string& path)
		:
		Technique<FlatBase, "FlatBase", Instanced, Skinned>(Skinned&& Instanced ? "FlatInstSkn" : (Skinned ? "FlatSkn" : (Instanced ? "FlatInst" : "Flat")))
	{
		auto shaderRootPath = gfx.GetShaderRootPath();

		std::string shaderCode = "Flat";
		aiString tempFileName;


		auto& vLayout = this->ExtractLayoutFromMaterial(material);

		// Common
		vLayout.AppendElement(VertexLayout::Position3D);
		vLayout.AppendElement(VertexLayout::Normal);
		ConstantBufferLayout cbLayout;

		Step step("Lambertian");
		gfx.GetRenderGraph().AddRenderQueue("Lambertian");

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
			if (Skinned)
			{
				vLayout.AppendElement(VertexLayout::BoneIds);
				vLayout.AppendElement(VertexLayout::BoneWeights);
			}
			auto pvs = IVertexShader::Resolve(gfx, shaderRootPath + shaderCode + (Instanced ? "Inst" : "") + (Skinned ? "Skn" : "") + "_VS.cso");
			step.AddBindable(IInputLayout::Resolve(gfx, vLayout, *pvs));
			step.AddBindable(std::move(pvs));
			step.AddBindable(IPixelShader::Resolve(gfx, shaderRootPath + shaderCode + "_PS.cso"));
			cbLayout.Solidify();
			auto buf = IPxConstantBuffer::Resolve(gfx, std::move(cbLayout), 1);

			if ((*buf)["materialColor"].Exists())
			{
				auto& param = (*buf)["materialColor"].Get<glm::vec3>();
				aiColor3D color = { 0.45f,0.45f,0.85f };
				aiMat.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				param = reinterpret_cast<glm::vec3&>(color);
			}
			if ((*buf)["specularColor"].Exists())
			{
				auto& param = (*buf)["specularColor"].Get<glm::vec3>();
				aiColor3D color = { 0.18f,0.18f,0.18f };
				aiMat.Get(AI_MATKEY_COLOR_SPECULAR, color);
				param = reinterpret_cast<glm::vec3&>(color);
			}
			if ((*buf)["specularWeight"].Exists())
			{
				auto& param = (*buf)["specularWeight"].Get<float>();
				param = 1.0f;
			}
			if ((*buf)["specularGloss"].Exists())
			{
				auto& param = (*buf)["specularGloss"].Get<float>();
				float gloss = 8.0f;
				aiMat.Get(AI_MATKEY_SHININESS, gloss);
				param = gloss;
			}

			step.AddBindable(std::move(buf));
		}

		this->AddStep(std::move(step));
	}




	// explicit template specialization
	template FlatBase<false, false>;
	template FlatBase<true, false>;
	template FlatBase<false, true>;
	template FlatBase<true, true>;
}