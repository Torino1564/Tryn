#pragma once
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ser/Serializer.h>
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/gfx/ImguiManager.h>
#include <Core/src/utl/StringHasher.h>

namespace tryn::ecs
{
	struct EntityID;
	class ComponentArray;

	class BufferView {};

	template <typename T>
	class TBufferView : public BufferView
	{
	public:
		std::span<T> view;
	};

	class ComponentWrapper
	{
	public:
		template <typename T>
		static ComponentWrapper Make(const std::string_view name, const uint16_t id);
		std::string_view Name() const;
		utl::UUID_t Uuid() const;
		uint16_t ByteSize() const;
		ComponentArray MakeArray(const uint16_t newSize = 0) const;
		void Delete(void* pData) const;
		void New(void* pData) const;
		void Move(void* pSource, void* pDestination) const;
		void ImGuiPrint(void* pData) const;
		virtual ~ComponentWrapper() = default;
		void UpdateView(ComponentArray& array) const;
	protected:
		ComponentWrapper() = default;
		std::string name;
		utl::UUID_t uuid = 0;
		std::uint16_t componentSize = 0;
		// callbacks

		void (*delete_)(void*) = nullptr;
		void (*new_)(void*) = nullptr;
		void (*move_)(void*, void*) = nullptr;
		void (*imguiPrint_)(void*) = nullptr;
		void (*updateView_)(ComponentArray&) = nullptr;
	};

	class ComponentArray
	{
	public:
		friend class ComponentWrapper;

		uint32_t ByteSize() const;
		uint32_t ElementCount() const;
		void Resize(uint32_t elementCount);
		std::byte* data();
		void Delete(EntityID id);
		std::byte* operator[](std::uint16_t);
		const ComponentWrapper& Wrapper() const;
	private:
		explicit ComponentArray(const ComponentWrapper& parent);
		std::vector<std::byte> buffer;
		std::shared_ptr<BufferView> view;
		const ComponentWrapper& parentWrapper;

	};

	class SingletonWrapper final : public ComponentWrapper
	{
	public:
		void* GetData();
		template <typename T>
		static SingletonWrapper Make(const std::string_view name, const uint16_t id, T&& singleton)
		{
			SingletonWrapper temp(std::move(ComponentWrapper::Make<T>(name, id)));
			temp.pData.resize(sizeof(T));
			T* tPointer = static_cast<T*>(temp.GetData());
			*tPointer = std::move(singleton);

			temp.pDestructor = [](SingletonWrapper& thisRef)
				{
					T* tPointer_ = static_cast<T*>(thisRef.GetData());
					tPointer_->~T();
				};

			return temp;
		}

		~SingletonWrapper() override;
	private:
		explicit SingletonWrapper(ComponentWrapper&&);
		std::vector<std::byte> pData;
		void (*pDestructor)(SingletonWrapper& thisRef) = nullptr;
	};


	template <typename T>
	ComponentWrapper ComponentWrapper::Make(const std::string_view name, const uint16_t id)
	{
		auto retval = ComponentWrapper();
		retval.name = name;
		retval.uuid = ZT_TYPE_UUID(T);
		retval.componentSize = sizeof(T);

		retval.delete_ = [](void* pData_)
			{
				auto pData = static_cast<T*>(pData_);
				pData->~T();
			};

		retval.new_ = [](void* pData_)
			{
				auto pData = static_cast<T*>(pData_);
				new(pData) T();
			};

		retval.move_ = [](void* pSource_, void* pDestination_)
			{
				auto pSource = static_cast<T*>(pSource_);
				auto pDestination = static_cast<T*>(pDestination_);

				new(pDestination) T(std::move(*pSource));
			};

		retval.imguiPrint_ = [](void* pData_)
			{
				auto pData = static_cast<T*>(pData_);
				ImGui::Text(ZT_TYPE_OF(T).data());
			};

		retval.updateView_ = [](ComponentArray& array)
			{
				array.view.reset();
				TBufferView<T> newView;
				T* ptr = reinterpret_cast<T*>(array.buffer.data());
				newView.view = std::span<T>(ptr, array.ElementCount());
				array.view = std::make_shared<TBufferView<T>>(std::move(newView));
			};

		return retval;
	}
}
