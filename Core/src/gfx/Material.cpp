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
#include <fstream>

namespace tryn::gfx
{
	Material::Material(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, Techniques defaultTechnique)
		:
		modelPath(path.string())
	{
		const auto rootPath = path.parent_path().string() + "\\";
		static bool isInitialized = false;
		static std::string shaderRootPath;
		if (!isInitialized)
		{
			shaderRootPath += __FILE__;
			size_t trynPos = shaderRootPath.rfind("Tryn");
			if (trynPos != std::string::npos)
			{
				shaderRootPath.erase(trynPos + 4);
			}

			shaderRootPath += "\\bin\\Shaders\\";
			isInitialized = true;
		}

		{
			aiString tempName;
			material.Get(AI_MATKEY_NAME, tempName);
			name = tempName.C_Str();
		}

		switch (defaultTechnique)
		{
		case Techniques::Phong:
		{
			//Phong
			{
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
					auto pvs = IVertexShader::Resolve(gfx, shaderRootPath + shaderCode + "_VS.cso");
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
						material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
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
						material.Get(AI_MATKEY_COLOR_SPECULAR, color);
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
						material.Get(AI_MATKEY_SHININESS, gloss);
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

				Phong.AddStep(std::move(step));

				techniques.push_back(std::move(Phong));
			}
			break;
		}
		case Techniques::Flat:
		{
			//Flat
			{
				Technique Flat("Flat");
				std::string shaderCode = "Flat";
				aiString tempFileName;

				// Common
				vLayout.AppendElement(VertexLayout::Position3D);
				vLayout.AppendElement(VertexLayout::Normal);
				ConstantBufferLayout cbLayout;

				Step step;

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
					auto pvs = IVertexShader::Resolve(gfx, shaderRootPath + shaderCode + "_VS.cso");
					step.AddBindable(IInputLayout::Resolve(gfx, vLayout, *pvs));
					step.AddBindable(std::move(pvs));
					step.AddBindable(IPixelShader::Resolve(gfx, shaderRootPath + shaderCode + "_PS.cso"));
					cbLayout.Solidify();
					auto buf = IPxConstantBuffer::Resolve(gfx, std::move(cbLayout), 1);

					if ((*buf)["materialColor"].Exists())
					{
						auto& param = (*buf)["materialColor"].Get<glm::vec3>();
						aiColor3D color = { 0.45f,0.45f,0.85f };
						material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
						param = reinterpret_cast<glm::vec3&>(color);
					}
					if ((*buf)["specularColor"].Exists())
					{
						auto& param = (*buf)["specularColor"].Get<glm::vec3>();
						aiColor3D color = { 0.18f,0.18f,0.18f };
						material.Get(AI_MATKEY_COLOR_SPECULAR, color);
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
						material.Get(AI_MATKEY_SHININESS, gloss);
						param = gloss;
					}

					step.AddBindable(std::move(buf));
				}

				Flat.AddStep(std::move(step));

				techniques.push_back(std::move(Flat));
			}
			break;
		}
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