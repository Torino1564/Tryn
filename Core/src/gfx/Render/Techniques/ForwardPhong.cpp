#include "ForwardPhong.h"
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
	ForwardPhongBase<Instanced, Skinned>::ForwardPhongBase(const std::string& name)
		:
	Technique<ForwardPhongBase, "ForwardPhongBase", Instanced, Skinned>(name)
	{
	}

	template<bool Instanced, bool Skinned>
	ForwardPhongBase<Instanced, Skinned>::ForwardPhongBase(Material& material, aiMaterial& aiMat, const IGraphics& gfx, const std::string& rootPath)
		:
		Technique<ForwardPhongBase, "ForwardPhongBase", Instanced, Skinned>(Skinned && Instanced ? "PhongInstSkn" : (Skinned ? "PhongSkn" : (Instanced ? "PhongInst" : "Phong")))
	{
		auto shaderRootPath = gfx.GetShaderRootPath();

		std::string shaderCode = "Phong";
		aiString tempFileName;

		auto& vLayout = this->ExtractLayoutFromMaterial(material);

		// Common
		vLayout.AppendElement(VertexLayout::Position3D);
		vLayout.AppendElement(VertexLayout::Normal);
		ConstantBufferLayout cbLayout;
		bool isTextured = false;
		bool usesGlossAlphaChannel = false;

		// Lambertian
		Step step("Lambertian");
		gfx.GetRenderGraph().AddRenderQueue("Lambertian");

		// Albedo
		{
			bool hasAlpha = false;
			if (aiMat.GetTexture(aiTextureType_DIFFUSE, 0, &tempFileName) == aiReturn_SUCCESS)
			{
				isTextured = true;
				shaderCode += "Tex";
				vLayout.AppendElement(VertexLayout::UV);
				auto tex = ITexture::Resolve(gfx, rootPath + tempFileName.C_Str(), 0);
				if (tex->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				step.AddBindable(std::move(tex));
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
			if (aiMat.GetTexture(aiTextureType_SPECULAR, 0, &tempFileName) == aiReturn_SUCCESS)
			{
				isTextured = true;
				shaderCode += "Spc";
				vLayout.AppendElement(VertexLayout::UV);
				auto tex = ITexture::Resolve(gfx, rootPath + tempFileName.C_Str(), 1);
				usesGlossAlphaChannel = tex->HasAlpha();
				step.AddBindable(std::move(tex));

				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Bool, "useGlossAlpha"));
				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Bool, "useSpecularMap"));
			}
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float3, "specularColor"));
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "specularWeight"));
			cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "specularGloss"));
		}
		// Normal
		{
			if (aiMat.GetTexture(aiTextureType_NORMALS, 0, &tempFileName) == aiReturn_SUCCESS)
			{
				isTextured = true;
				shaderCode += "Nrm";
				vLayout.AppendElement(VertexLayout::UV);
				vLayout.AppendElement(VertexLayout::Tangent);
				vLayout.AppendElement(VertexLayout::Bitangent);
				step.AddBindable(ITexture::Resolve(gfx, rootPath + tempFileName.C_Str(), 2));
				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Bool, "useNormalMap"));
				cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Float, "normalMapWeight"));
			}
		}
		// Common
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
			if (isTextured)
			{
				step.AddBindable(ISampler::Resolve(gfx));
			}
			cbLayout.Solidify();
			auto buf = IPxConstantBuffer::Resolve(gfx, std::move(cbLayout), 1);
			if ((*buf)["materialColor"].Exists())
			{
				auto& param = (*buf)["materialColor"].Get<glm::vec3>();
				aiColor3D color = { 0.45f,0.45f,0.85f };
				aiMat.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				param = reinterpret_cast<glm::vec3&>(color);
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



	// explicit template specialization
	template ForwardPhongBase<true, true>;
	template ForwardPhongBase<true, false>;
	template ForwardPhongBase<false, true>;
	template ForwardPhongBase<false, false>;
}