#pragma once
#include <core/src/gfx/IGraphics.h>
#include <core/src/gfx/Render/Technique.h>
#include <Core/src/gfx/Vertex.h>
#include <filesystem>
#include <format>
#include <Core/src/gfx/IBufferFwd.h>
#include <concepts>
#include <Core/src/gfx/Render/Techniques/ForwardPhong.h>

template <typename T>
concept TechniqueClass = std::derived_from<T, class tryn::gfx::Technique>;

struct aiMaterial;
struct aiMesh;

namespace tryn::gfx
{
	class Material
	{
		friend class Technique;
	public:

		template <TechniqueClass... T>
		static Material Make(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path)
		{
			// TODO:
			// assert uniqueness
			
			Material mat;
			// add techniques
			if constexpr (sizeof...(T) == 0)
			{
				mat.AddTechnique<ForwardPhong>(std::forward<IGraphics>(gfx), std::forward<aiMaterial>(material), std::forward< std::filesystem::path>(path));
			}
			else
			{
				mat.AddTechnique<0, T...>(std::forward<IGraphics>(gfx), std::forward<aiMaterial>(material), std::forward< std::filesystem::path>(path));
			}

			return mat;
		}
		VertexBuffer ExtractVertices(const aiMesh& mesh, ani::Skeleton* skeleton = nullptr) const noexcept;
		IndexBuffer ExtractIndices(const aiMesh& mesh) const noexcept;
		std::vector<std::shared_ptr<Technique>> GetTechniques() const noexcept;

		template <unsigned N = 0, TechniqueClass... T>
		void AddTechnique(IGraphics& gfx, const aiMaterial& material, const std::filesystem::path& path)
		{
			if constexpr (N < sizeof...(T))
			{
				using TechniqueType = std::tuple_element_t<N, std::tuple<T...>>;
				pTechniques.push_back(std::move(std::make_shared<TechniqueType>(*this, material, gfx, path)));
				return AddTechnique<N + 1, T...>(std::forward<IGraphics>(gfx), std::forward<aiMaterial>(material), std::forward< std::filesystem::path>(path));
			}
			return;
		}
	private:
		VertexLayout vLayout;
		std::vector<std::shared_ptr<Technique>> pTechniques;
		std::string name;
	};
}
