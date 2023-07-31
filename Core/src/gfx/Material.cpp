#include "Material.h"
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx
{
	Material::Material(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path)
		:
		modelPath(path.string())
	{
		const auto rootPath = path.parent_path().string() + "\\";

		{
			const aiString tempName;
			// ReSharper disable once CppExpressionWithoutSideEffects
			material.Get(AI_MATKEY_NAME, tempName);
			name = tempName.C_Str();
		}

		// Default Techniques 

		{
			// Phong
			Technique Phong("Phong");
			Step step;
			std::string shaderCode = "Phong";
			aiString tempFileName;

			// Common
			vLayout.AppendElement(VertexLayout::Position3D);
			vLayout.AppendElement(VertexLayout::Normal);
			ConstantBufferLayout cbLayout;
			bool isTextured;
			bool usesGlossAlphaChannel;

			// Albedo
			{
				bool hasAlpha = false;
				if (material.GetTexture(aiTextureType_DIFFUSE, 0, &tempFileName) == aiReturn_SUCCESS)
				{
					isTextured = true;
					shaderCode += "Dif";
					vLayout.AppendElement(VertexLayout::UV);
				}
			}
		}
	}
}
