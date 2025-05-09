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

		static std::shared_ptr<TechniqueBase> ConstructTechnique(utl::UUID_t techniqueUUID, const std::vector<const std::shared_ptr<class Material>>& materials, const IGraphics& gfx, bool instanced = false, bool skeleton = false);

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
		static const std::string& Name(utl::UUID_t techniqueUuid);

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
		void Draw(const IGraphics& gfx, const Drawable* parent) const;
		void Submit(const IGraphics& gfx, Drawable* parent);
		void Submit(const IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instancedParent);
		void Accept(class TechniqueProbe& probe);
		virtual std::shared_ptr<TechniqueBase> ConstructDerived(const std::vector<const std::shared_ptr<Material>>& materials, const IGraphics& gfx, bool instanced, bool skinned) const = 0;
		class VertexLayout GetVertexLayout() const;
		virtual utl::UUID_t UUID() const = 0;
		auto&& GetStep(this auto&& self, const uint16_t index)
		{
			return std::forward<decltype(self)>(self).steps[index];
		}
		auto&& GetStep(this auto&& self, const std::string& renderQueueID)
		{
			auto it = std::ranges::find_if(std::forward<decltype(self)>(self).steps, [&](const Step& step) { return step.RenderQueueID() == renderQueueID; });
			return *it;
		}
	protected:
		std::vector<IBindable*> pExtraBinds;
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

		std::shared_ptr<TechniqueBase> ConstructDerived(const std::vector<const std::shared_ptr<Material>>& materials, const IGraphics& gfx, const bool instanced = false, const bool skinned = false) const override;
		utl::UUID_t UUID() const override;
		static constexpr auto GetUUID();

	protected:
		friend class TechniquePool;
		using Type = T;
		static constexpr auto uuid = ZT_TYPE_UUID(T);
		static inline bool registered = TechniquePool::RegisterTechnique<T>(uuid);
	};

	template <class T>
	std::shared_ptr<TechniqueBase> Technique<T>::ConstructDerived(const std::vector<const std::shared_ptr<Material>>& materials,
		const IGraphics& gfx, const bool instanced, const bool skinned) const
	{
		return std::make_shared<T>(materials, gfx, instanced, skinned);
	}

	template <class T>
	utl::UUID_t Technique<T>::UUID() const
	{
		return GetUUID();
	}

	template <class T>
	constexpr auto Technique<T>::GetUUID()
	{
		return uuid;
	}
}
