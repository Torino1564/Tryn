#pragma once
#include <string>
#include "TechniqueProbe.h"
#include "Step.h"
#include <Core/src/utl/StringHasher.h>
#include <Core/src/utl/StatefulMeta/TemplateData.h>

#include <unordered_map>

#include "Core/src/utl/String.h"

struct aiMaterial;

namespace tryn::gfx
{
	class IGraphics;

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

		static bool RegisterTechnique(utl::UUID_t UUID)
		{
			return true;
		}

		static std::shared_ptr<TechniqueBase> ConstructTechnique(utl::UUID_t techniqueUUID, class Material& material, aiMaterial& aiMaterial, const IGraphics& gfx, const std::string& path, const bool instanced = false, const bool skeleton = false);

	private:
		static TechniquePool& Get()
		{
			static TechniquePool singleton;
			return singleton;
		}
		TechniquePool() = default;
		std::unordered_map<utl::UUID_t, std::unique_ptr<TechniqueBase>> techniqueMap;
	};

	class TechniqueBase
	{
		friend class TechniquePool;
	public:
		TechniqueBase(const std::string& name);
		virtual ~TechniqueBase() = default;
		void AddStep(Step step);
		void Draw(const IGraphics& gfx, Drawable* parent);
		void Submit(const IGraphics& gfx, Drawable* parent);
		void Submit(const IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instancedParent);
		void Accept(class TechniqueProbe& probe);
		virtual std::shared_ptr<TechniqueBase> ConstructDerived(class Material& material, aiMaterial& aiMaterial, const IGraphics& gfx, const std::string& path, bool instanced, bool skinned) = 0;
	protected:
		class VertexLayout& ExtractLayoutFromMaterial(class Material& mat);
		std::string name;
		std::vector<Step> steps;
	};

#define ZT_DEFINE_TECHNIQUE(x) 	template <bool Instanced = false, bool Skinned = false> \
	class FlatBase : public tryn::gfx::Technique<x, #x, Instanced, Skinned>

	template <template <bool Inst, bool Skn> class T, utl::StaticString Name, bool Instanced, bool Skinned>
	class Technique : public TechniqueBase
	{
	public:
		Technique() = default;
		Technique(const std::string& name)
			:
		TechniqueBase(name) {}

		std::shared_ptr<TechniqueBase> ConstructDerived(class Material& material, aiMaterial& aiMaterial, const IGraphics& gfx, const std::string& path, const bool instanced = false, const bool skinned = false) override
		{
			if (instanced && skinned)
			{
				return std::make_shared<T<false, false>>(material, aiMaterial, gfx, path);
			}							
			else if (instanced)			
			{							
				return std::make_shared<T<false, false>>(material, aiMaterial, gfx, path);
			}							
			else if (skinned)			
			{							
				return std::make_shared<T<false, false>>(material, aiMaterial, gfx, path);
			}							
			else						
			{							
				return std::make_shared<T<false, false>>(material, aiMaterial, gfx, path);
			}
		}
	protected:
		using Type = Technique<T, Name, Instanced, Skinned>;
		static constexpr auto UUID = ZT_STRING_HASH(Name.v);
		using TechType = T<false, false>;
		static inline bool registered = TechniquePool::RegisterTechnique(UUID);
	};

	inline std::shared_ptr<class TechniqueBase> TechniquePool::ConstructTechnique(utl::UUID_t techniqueUUID,
		Material& material, aiMaterial& aiMaterial, const IGraphics& gfx, const std::string& path, const bool instanced,
		const bool skeleton)
	{
		auto it = Get().techniqueMap.find(techniqueUUID);
		trynass(it != Get().techniqueMap.end()).msg(utl::ToWide(std::format("Did not find technique with UUID: {}", techniqueUUID))).lvl(log::Level::Error).ex();
		return it->second->ConstructDerived(material, aiMaterial, gfx, path, instanced, skeleton);
	}
}
