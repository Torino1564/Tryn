#pragma once
#include <memory>
#include <optional>

namespace tryn::mem
{
	/*template <typename T>
	class SharedRef
	{
	public:
		template<typename ... Args, std::invocable Callback>
		SharedRef(Callback callback, std::shared_ptr<T> ptr)
		{
			ptr_ = std::move(ptr);
			this->callback = std::move(callback);
		}
		SharedRef(std::shared_ptr<T> ptr)
		{
			ptr_ = ptr;
		}
		~SharedRef()
		{
			if (ptr_.use_count() == 1 && callback)
			{
				(callback.value())();
			}
		}
		T& operator->()
		{
			return *ptr_;
		}
		std::shared_ptr<T>& Get()
		{
			return ptr_;
		}
	private:
		std::optional<std::function<void()> callback;
		std::shared_ptr<T> ptr_;
	};*/
}