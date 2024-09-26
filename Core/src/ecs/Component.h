#pragma once
#include "Core/src/utl/StringHasher.h"
#include <Core/src/ecs/EcsClass.h>
#include <Core/third/field_reflection.hpp>

namespace tryn::ecs
{
	template <typename T>
	concept CallbackSignatureClass = requires {
		typename T::ArgTypes;
	}&& std::is_class_v<T>;

	class IComponentWrapper
	{
	public:
		virtual ~IComponentWrapper() = default;
		virtual const char* Name() const = 0;
		virtual size_t Size() const = 0;
		virtual void Construct(void* pData) const = 0;
		virtual void Destroy(void* pData) const = 0;
		virtual uint32_t Index() const = 0;
		virtual void Serialize(ser::StreamWriter& streamWriter, void* data) = 0;
		virtual void ImGuiPrint(void* data) = 0;
	};

	template <typename T>
	class HardComponentWrapperBase : public IComponentWrapper
	{
	public:

		constexpr static auto name = ZT_TYPE_OF(T);
		static constexpr auto UUID = ZT_STRING_HASH(name);
		static const uint32_t index;

		using ComponentType = T;

		const char* Name() const override
		{
			return name.data();
		}
		size_t Size() const override
		{
			return sizeof(T);
		}
		void Construct(void* pData) const override
		{
			auto pData_ = static_cast<T*>(pData);
			new (pData_) T();
		}
		void Destroy(void* pData) const override
		{
			auto pData_ = static_cast<T*>(pData);
			std::destroy_at(pData);
		}
		uint32_t Index() const
		{
			return index;
		}
		template <CallbackSignatureClass... Args>
		friend class ComponentView;

		void Serialize(ser::StreamWriter& streamWriter, void* data) override
		{
			auto pData = static_cast<T*>(data);

			streamWriter.Serialize(*pData);
		}

		void ImGuiPrint(void* data) override
		{
			namespace fr = field_reflection;
			auto pData = static_cast<T*>(data);
			auto tuple = fr::to_tuple(*pData);


		}

	private:
		template <unsigned N = 0, typename... Types>
		void ImGuiPrint_(std::tuple<Types...>& tuple)
		{
			if constexpr (N < std::tuple_size_v<decltype(tuple)>)
			{
				ImGui::Text(std::format("{}: {}", ZT_TYPE_OF(std::get<N>(tuple)), varNameFunc()).c_str());
			}
		}
	};
}
