#pragma once
#include <string>
#include "TechniqueProbe.h"
#include "Step.h"
#include <Core/src/utl/StringHasher.h>
#include <Core/src/utl/StatefulMeta/TemplateData.h>

#include <unordered_map>

struct aiMaterial;

namespace tryn::gfx
{
	class TechniqueBase;
	template <typename OriginalInstanciation, bool NewParam1, bool NewParam2> struct ReplaceTemplateParam;

	template <template<bool, bool> class Tech, bool OldParam1, bool OldParam2, bool NewParam1, bool NewParam2>
	struct ReplaceTemplateParam<Tech<OldParam1, OldParam2>, NewParam1, NewParam2> {
		using type = Tech<NewParam1, NewParam2>;
	};

	enum class Techniques
	{
		Phong,
		Flat,
		Garaoud,
	};

	class TechniquePool
	{
	public:
		static TechniquePool& Get()
		{
			static TechniquePool singleton;
			return singleton;
		}
		bool RegisterTechnique(utl::UUID_t UUID, std::unique_ptr<TechniqueBase>&& pTechnique);

		std::shared_ptr<TechniqueBase> ConstructTechnique(utl::UUID_t techniqueUUID, class Material& material, aiMaterial& aiMaterial, class IGraphics& gfx, const std::string& path, const bool instanced = false, const bool skeleton = false) const;
	private:
		TechniquePool() = default;
		std::unordered_map<utl::UUID_t, std::unique_ptr<TechniqueBase>> techniqueMap;
	};

	class TechniqueBase
	{
		friend class TechniquePool;
	public:
		TechniqueBase(const std::string& name);
		void AddStep(Step step);
		void Draw(class IGraphics& gfx, Drawable* parent);
		void Submit(class IGraphics& gfx, Drawable* parent);
		void Submit(class IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instancedParent);
		void Accept(class TechniqueProbe& probe);
		virtual std::shared_ptr<TechniqueBase> ConstructDerived(class Material& material, aiMaterial& aiMaterial, class IGraphics& gfx, const std::string& path, bool instanced, bool skinned) = 0;
	protected:
		class VertexLayout& ExtractLayoutFromMaterial(class Material& mat);
		std::string name;
		std::vector<Step> steps;
	};

	template <typename T, utl::StaticString Name, bool Instanced, bool Skinned>
	class Technique : public TechniqueBase
	{
	public:
		Technique(const std::string& name)
			:
		TechniqueBase(name) {}

		std::shared_ptr<TechniqueBase> ConstructDerived(class Material& material, aiMaterial& aiMaterial, class IGraphics& gfx, const std::string& path, const bool instanced = false, const bool skinned = false) override
		{
			if (instanced && skinned)
			{
				return std::make_shared<Technique<T, Name, true, true>>(material, aiMaterial, gfx, path);
			}
			else if (instanced)
			{
				return std::make_shared<Technique<T, Name, true, false>>(material, aiMaterial, gfx, path);
			}
			else if (skinned)
			{
				return std::make_shared<Technique<T, Name, false, true>>(material, aiMaterial, gfx, path);
			}
			else
			{
				return std::make_shared<Technique<T, Name, false, false>>(material, aiMaterial, gfx, path);
			}
		}
	protected:
		static constexpr auto UUID = ZT_STRING_HASH(Name.v);
		static constexpr bool registered = TechniquePool::Get().RegisterTechnique(UUID, std::move(std::make_unique<Technique<T, Name>>(Name.v)));
	};
}
