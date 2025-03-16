#pragma once
#include <core/src/gfx/Render/Technique.h>
#include <Core/src/gfx/Vertex.h>
#include <filesystem>
#include <format>
#include <Core/src/gfx/IBufferFwd.h>
#include <concepts>
#include <Core/src/gfx/Render/Techniques/ForwardPhong.h>
#include <Core/src/gfx/Render/Techniques/Flat.h>

template <typename T>
concept TechniqueClass = std::derived_from<T, class tryn::gfx::TechniqueBase>;

struct aiMaterial;
struct aiMesh;
struct aiScene;

namespace tryn::gfx
{
	class IGraphics;
	class Material
	{
		friend class TechniqueBase;
	public:
		template <TechniqueClass... T>
		static Material Make(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path)
		{
			// TODO:
			// assert uniqueness

			const auto rootPath = path.parent_path().string() + "\\";

			Material mat;
			// add techniques
			if constexpr (sizeof...(T) == 0)
			{
				mat.AddTechnique<0, ForwardPhong>(gfx, material, rootPath);
			}
			else
			{
				mat.AddTechnique<0, T...>(gfx, material, rootPath);
			}

			return mat;
		}
		static Material MakeDefault(const IGraphics& gfx);
		
		static Material Make(const IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path, std::span<utl::UUID_t> techniqueUUIDs, const bool instanced = false,const bool skinned = false);
		VertexBuffer ExtractVertices(const aiMesh& mesh, ani::Skeleton* skeleton = nullptr) const noexcept;
		IndexBuffer ExtractIndices(const aiMesh& mesh) const noexcept;
		VertexBuffer ExtractVertices(const Shape3D& mesh) const noexcept;
		IndexBuffer ExtractIndices(const Shape3D& mesh) const noexcept;
		std::vector<std::shared_ptr<TechniqueBase>> GetTechniques() const noexcept;

		void AddTechnique(utl::UUID_t techniqueUUID, const IGraphics& gfx, const aiMaterial& material, const std::string& path, bool instanced, bool skinned);

		template <unsigned N = 0, TechniqueClass... T>
		void AddTechnique(const IGraphics& gfx, const aiMaterial& material, const std::string& path)
		{
			if constexpr (N < sizeof...(T))
			{
				using TechniqueType = std::tuple_element_t<N, std::tuple<T...>>;
				pTechniques.push_back(std::move(std::make_shared<TechniqueType>(*this, material, gfx, path)));
				return AddTechnique<N + 1, T...>(gfx, material, path);
			}
			return;
		}
		aiScene* pScene = nullptr;

	private:
		VertexLayout vLayout;
		std::vector<std::shared_ptr<TechniqueBase>> pTechniques;
		std::string name;

	};
}
