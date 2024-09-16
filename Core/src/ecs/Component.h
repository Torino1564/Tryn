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

	template <template <typename T> class Fun>
	struct Placeholder
	{
		using tt = Fun;

		template <typename T>
		using ttp = tt<T>;
	};

	template <typename T>
	struct Test
	{
		template <typename OtherT>
		static void Call(void*)
		{
			
		}
	};

	struct WrappedTask
	{
		template <template <typename T> typename FunctionTemplate>
		static WrappedTask Make()
		{
			WrappedTask task;
			auto fun = [](void* data)
				{
					using ft = FunctionTemplate;
					static auto& inside = []<typename T0>(void* var)
						{
							auto trueVar = static_cast<T0*>(var);
							FunctionTemplate<T0>(data);
						};
					
				};
		}

		template <typename T>
		void (*CallWith())(T*)
		{
			void(*retval)(T*) = nullptr;

			auto second = []<typename T0>()
			{
				
			};

		}
	};

	class IComponentWrapper
	{
	public:
		virtual ~IComponentWrapper() = default;
		virtual const char* Name() const = 0;
		virtual size_t Size() const = 0;
		virtual void Construct(void* pData) const = 0;
		virtual void Destroy(void* pData) const = 0;
		virtual uint32_t Index() const = 0;
		virtual void ForEachField(void(*)(std::string_view, void*)) = 0;
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

		void ForEachField(void(*callback)(const std::string_view, void*)) override
		{
			namespace fr = field_reflection;
			
			fr::for_each_field(ComponentType(), [&](std::string_view field, auto& value)
				{
					callback(field, value);
				});
		}
	};
}
