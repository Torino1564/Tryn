
#include "ForwardPhong.h"
#include <assimp/types.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Bindables/TextureResource.h>
#include <Core/src/gfx/Bindables/Rasterizer.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/PixelShader.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/Bindables/Sampler.h>
#include <Core/src/gfx/Bindables/ConstantBufferResource.h>

namespace tryn::gfx
{
	ForwardPhong::ForwardPhong(const std::string& name)
		:
	Technique(name)
	{
	}

	ForwardPhong::ForwardPhong(const std::vector<std::shared_ptr<Material>>& materials, const IGraphics& gfx, bool instanced, bool skinned)
		:
		Technique("ForwardPhong")
	{
		usedMaterials.emplace_back(materials[0]);
		auto& material = *materials[0];
		auto shaderRootPath = tryn::gfx::IGraphics::GetShaderRootPath();

		std::string shaderCode = "Phong";
		aiString tempFileName;


		ConstantBufferLayout cbLayout;
		bool isTextured = false;
		bool usesGlossAlphaChannel = false;

		// Lambertian
		Step step(gfx, "Lambertian");
		gfx.GetRenderGraph().AddRenderQueue("Lambertian");
		auto& vLayout = step.GetVertexLayout();
		vLayout.AppendElement(VertexLayout::Position3D);
		vLayout.AppendElement(VertexLayout::Normal);

		// Albedo
		{
			bool hasAlpha = false;
			if (material.HasTexture(TextureType::Diffuse) || material.HasTexture(TextureType::MetallicRoughnessBaseColor))
			{
				isTextured = true;
				shaderCode += "Tex";
				std::shared_ptr<Texture> pTexture;
				if (material.HasTexture(TextureType::Diffuse))
				{
					pTexture = material.GetTexture(TextureType::Diffuse);
				}
				else if (material.HasTexture(TextureType::MetallicRoughnessBaseColor))
				{
					pTexture = material.GetTexture(TextureType::MetallicRoughnessBaseColor);
				}
				if (pTexture->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				auto pTextureBindable = ITexture::Resolve(gfx, pTexture, 0);
				step.AddBindable(std::move(pTextureBindable));
			}
			else
			{
				shaderCode += "Flat";
				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3, "materialColor"));
			}
			step.AddBindable(std::move(IRasterizer::Resolve(gfx, hasAlpha)));
		}
		// Specular
		{
			if (material.HasTexture(TextureType::Specular))
			{
				isTextured = true;
				shaderCode += "Spc";
				auto pTexture = material.GetTexture(TextureType::Specular);
				usesGlossAlphaChannel = pTexture->HasAlpha();
				auto pTextureBindable = ITexture::Resolve(gfx, pTexture, 1);
				step.AddBindable(std::move(pTextureBindable));

				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Bool, "useGlossAlpha"));
				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Bool, "useSpecularMap"));
			}
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float3, "specularColor"));
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "specularWeight"));
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "specularGloss"));
		}
		// Normal
		{
			if (material.HasTexture(TextureType::Normal))
			{
				isTextured = true;
				shaderCode += "Nrm";
				vLayout.AppendElement(VertexLayout::Tangent);
				vLayout.AppendElement(VertexLayout::Bitangent);
				auto pTexture = material.GetTexture(TextureType::Normal);
				auto pTextureBindable = ITexture::Resolve(gfx, pTexture, 2);
				step.AddBindable(std::move(pTextureBindable));
				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Bool, "useNormalMap"));
				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "normalMapWeight"));
			}
		}
		// Common
		{
			if (skinned)
			{
				vLayout.AppendElement(VertexLayout::BoneIds);
				vLayout.AppendElement(VertexLayout::BoneWeights);
			}
			auto pvs = IVertexShader::Resolve(gfx, shaderRootPath + shaderCode + (instanced ? "Inst" : "") + (skinned ? "Skn" : "") + "_VS.cso");
			if (isTextured)
			{
				vLayout.AppendElement(VertexLayout::UV);
				step.AddBindable(ISampler::Resolve(gfx));
			}
			step.AddBindable(std::move(pvs));
			step.AddBindable(IPixelShader::Resolve(gfx, shaderRootPath + shaderCode + "_PS.cso"));

			cbLayout.Solidify();
			auto buf = IConstantBufferResource::Resolve(gfx, std::move(cbLayout), IConstantBufferResource::Type::Pixel, 1);
			if ((*buf)["materialColor"].Exists())
			{
				auto& param = (*buf)["materialColor"].Get<glm::vec3>();
				param = material.GetAttributeOr<glm::vec3>(AttributeType::DiffuseColor, {0.45f, 0.45, 0.45f});
			}
			if ((*buf)["useGlossAlpha"].Exists())
			{
				auto& param = (*buf)["useGlossAlpha"].Get<bool>();
				param = usesGlossAlphaChannel;
			}
			if ((*buf)["useSpecularMap"].Exists())
			{
				auto& param = (*buf)["useSpecularMap"].Get<bool>();
				param = true;
			}
			if ((*buf)["specularColor"].Exists())
			{
				auto& param = (*buf)["specularColor"].Get<glm::vec3>();
				param = material.GetAttributeOr<glm::vec3>(AttributeType::SpecularColor, {0.18f, 0.18f, 0.18f});
			}
			if ((*buf)["specularWeight"].Exists())
			{
				auto& param = (*buf)["specularWeight"].Get<float>();
				param = 1.0f;
			}
			if ((*buf)["specularGloss"].Exists())
			{
				auto& param = (*buf)["specularGloss"].Get<float>();
				param = material.GetAttributeOr<float>(AttributeType::SpecularGloss, 8.0f);
			}
			if ((*buf)["useNormalMap"].Exists())
			{
				auto& param = (*buf)["useNormalMap"].Get<bool>();
				param = true;
			}
			if ((*buf)["normalMapWeight"].Exists())
			{
				auto& param = (*buf)["normalMapWeight"].Get<float>();
				param = 1.0f;
			}

			step.AddBindable(std::move(buf));
		}

		this->AddStep(std::move(step));
	}
}
