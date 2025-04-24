#pragma once
#include <string>
#include "TechniqueProbe.h"
#include "Step.h"
#include <Core/src/utl/StringHasher.h>

#include <unordered_map>

#include "Core/src/utl/String.h"

struct aiMaterial;

namespace tryn::gfx
{
	class IGraphics;

	template <typename T>
	class Technique;

	template <typename T>
	concept TechniqueClass = std::derived_from<T, Technique<T>>;

	class TechniquePool
	{
	public:

		static std::shared_ptr<TechniqueBase> ConstructTechnique(utl::UUID_t techniqueUUID, const class Material& material, const IGraphics& gfx, const std::string& path, bool instanced = false, bool skeleton = false);

		template <typename T>
		static bool RegisterTechnique(utl::UUID_t uuid)
		{
			const auto it = Get().techniqueMap.find(uuid);
			if (it == Get().techniqueMap.end())
			{
				auto [iterator, result] = Get().techniqueMap.insert({uuid, std::make_unique<T>("Handle")});
				return true;
			}
			trynchk_fail.msg(utl::ToWide(std::format("The technique [{}] with UUID: [{}] has already been registered in the TechniquePool.", ZT_TYPE_OF(T), uuid)));
			std::unreachable();
		}

	private:
		static TechniquePool& Get();
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
		void Draw(const IGraphics& gfx, Drawable* parent) const;
		void Submit(const IGraphics& gfx, Drawable* parent);
		void Submit(const IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instancedParent);
		void Accept(class TechniqueProbe& probe);
		virtual std::shared_ptr<TechniqueBase> ConstructDerived(const Material& material, const IGraphics& gfx, const std::string& path, bool instanced, bool skinned) = 0;
		class VertexLayout GetVertexLayout() const;
	protected:
		std::unique_ptr<VertexLayout> pVertexLayout;
		std::string name;
		std::vector<Step> steps;
	};

	template <class T>
	class Technique : public TechniqueBase
	{
	public:
		Technique() = default;
		explicit Technique(const std::string& name)
			:
		TechniqueBase(name) {}

		std::shared_ptr<TechniqueBase> ConstructDerived(const Material& material, const IGraphics& gfx, const std::string& path, const bool instanced = false, const bool skinned = false) override;
		static constexpr auto GetUUID();

	protected:
		friend class TechniquePool;
		using Type = T;
		static constexpr auto uuid = ZT_TYPE_UUID(T);
		static inline bool registered = TechniquePool::RegisterTechnique<T>(uuid);
	};

	template <class T>
	std::shared_ptr<TechniqueBase> Technique<T>::ConstructDerived(const Material& material,
		const IGraphics& gfx, const std::string& path, const bool instanced, const bool skinned)
	{
		return std::make_shared<T>(material, gfx, path, instanced, skinned);
	}

	template <class T>
	constexpr auto Technique<T>::GetUUID()
	{
		return uuid;
	}
}
