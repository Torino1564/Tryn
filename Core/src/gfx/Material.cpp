#include "Material.h"
#include <assimp/material.h>
#include <assimp/types.h>
#include <Core/src/gfx/Bindables/Texture.h>
#include <Core/src/gfx/Bindables/Rasterizer.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>
#include <Core/src/gfx/Bindables/VertexBuffer.h>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/PixelShader.h>
#include <Core/src/gfx/Bindables/InputLayout.h>
#include <Core/src/gfx/Bindables/Sampler.h>

namespace tryn::gfx
{
	Material::Material(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path)
		:
		modelPath(path.string())
	{
		const auto rootPath = path.parent_path().string() + "\\";

		{
			aiString tempName;
			// ReSharper disable once CppExpressionWithoutSideEffects
			material.Get(AI_MATKEY_NAME, tempName);
			name = tempName.C_Str();
		}

		// Default Techniques

		{
			// Phong
			Technique Phong("Phong");
			std::string shaderCode = "Phong";
			aiString tempFileName;

			// Common
			vLayout.AppendElement(VertexLayout::Position3D);
			vLayout.AppendElement(VertexLayout::Normal);
			ConstantBufferLayout cbLayout;
			bool isTextured = false;
			bool usesGlossAlphaChannel = false;

			Step step;

			// Albedo
			{
				bool hasAlpha = false;
				if (material.GetTexture(aiTextureType_DIFFUSE, 0, &tempFileName) == aiReturn_SUCCESS)
				{
					isTextured = true;
					shaderCode += "Dif";
					vLayout.AppendElement(VertexLayout::UV);
					const auto tex = ITexture::Resolve(gfx, path.string(), 0);
					if (tex->HasAlpha())
					{
						hasAlpha = true;
					}
					step.AddBindable(std::move(tex));
				}
				else
				{
					cbLayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3, "materialColor"));
				}
				step.AddBindable(std::move(IRasterizer::Resolve(gfx, hasAlpha)));
			}
			// Specular
			{
				if (material.GetTexture(aiTextureType_SPECULAR, 0, &tempFileName) == aiReturn_SUCCESS)
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
				if (material.GetTexture(aiTextureType_NORMALS, 0, &tempFileName) == aiReturn_SUCCESS)
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
				auto pvs = IVertexShader::Resolve(gfx, shaderCode + "_VS.cso");
				step.AddBindable(IInputLayout::Resolve(gfx, vLayout, *pvs));
				step.AddBindable(std::move(pvs));
				step.AddBindable(IPixelShader::Resolve(gfx, shaderCode + "_PS.cso"));
				if (isTextured)
				{
					step.AddBindable(ISampler::Resolve(gfx));
				}
				cbLayout.Solidify();
				auto buf = IPxConstantBuffer::Resolve(gfx, std::move(cbLayout), 1);
				if (auto param = (*buf)["materialColor"]; param.Exists())
				{
					aiColor3D color = { 0.45f,0.45f,0.85f };
					material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
					param = reinterpret_cast<glm::vec3&>(color);
				}
				if (auto param = (*buf)["useGlossAlpha"]; param.Exists())
				{
					param = usesGlossAlphaChannel;
				}
				if (auto param = (*buf)["useSpecularMap"]; param.Exists())
				{
					param = true;
				}
				if (auto param = (*buf)["specularColor"]; param.Exists())
				{
					aiColor3D color = { 0.18f,0.18f,0.18f };
					material.Get(AI_MATKEY_COLOR_SPECULAR, color);
					param = reinterpret_cast<glm::vec3&>(color);
				}
				if (auto param = (*buf)["specularWeight"]; param.Exists())
				{
					param = 1.0f;
				}
				if (auto param = (*buf)["specularGloss"]; param.Exists())
				{
					float gloss = 8.0f;
					material.Get(AI_MATKEY_SHININESS, gloss);
					param = gloss;
				}
				if (auto param = (*buf)["useNormalMap"]; param.Exists())
				{
					param = true;
				}
				if (auto param = (*buf)["normalMapWeight"]; param.Exists())
				{
					param = 1.0f;
				}

				step.AddBindable(std::move(buf));
			}

			Phong.AddStep(std::move(step));

			techniques.push_back(std::move(Phong));
		}
	}
	VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
	{
		return { vLayout,mesh };
	}
	std::vector<int> Material::ExtractIndices(const aiMesh& mesh) const noexcept
	{
		std::vector<int> indices;
		indices.resize(mesh.mNumFaces * 3);

		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& triangle = mesh.mFaces[i];
			indices[3 * i] = triangle.mIndices[0];
			indices[(3 * i) + 1] = triangle.mIndices[1];
			indices[(3 * i) + 2] = triangle.mIndices[2];
		}
		return indices;
	}
	std::vector<Technique> Material::GetTechniques() const noexcept
	{
		return techniques;
	}
}