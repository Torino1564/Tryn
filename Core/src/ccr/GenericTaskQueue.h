#pragma once
#include <deque>
#include <future>
#include <functional>
#include <variant>
#include <type_traits>

namespace tryn::ccr
{
	class GenericTaskQueue
	{
	public:
		using Task = std::move_only_function<void()>;
		template <std::invocable InputTask>
		auto Push(std::unique_ptr<InputTask>&& functionPtr)
		{
			using T = std::invoke_result_t<decltype(*functionPtr)>;
			std::packaged_task<T(std::unique_ptr<InputTask>&&)> pkg{ [](std::unique_ptr<InputTask>&& taskPtr) { return (*taskPtr)(); } };
			auto future = pkg.get_future();
			PushTask_(std::move([pkg = std::move(pkg), ptr = std::move(functionPtr)]() mutable { pkg(std::move(ptr)); }));
			return future;
		}
		template <std::invocable InputTask>
		auto Push(InputTask&& functionPtr)
		{
			using T = std::invoke_result_t<decltype(functionPtr)>;
			std::packaged_task<T(InputTask&&)> pkg{ [](InputTask&& taskPtr) { return taskPtr(); } };
			auto future = pkg.get_future();
			PushTask_(std::move([pkg = std::move(pkg), ptr = std::move(functionPtr)]() mutable { pkg(std::move(ptr)); }));
			return future;
		}
		template <std::invocable InputTask>
		auto Push(std::shared_ptr<InputTask> functionPtr)
		{
			using T = std::invoke_result_t<decltype(*functionPtr)>;
			std::packaged_task<T(std::shared_ptr<InputTask>)> pkg{ [](std::shared_ptr<InputTask> taskPtr) { return (*taskPtr)(); } };
			auto future = pkg.get_future();
			PushTask_(std::move([pkg = std::move(pkg), ptr = std::move(functionPtr)]() mutable { pkg(std::move(ptr)); }));
			return future;
		}
		void PopExecute()
		{
			Task task;
			{
				std::lock_guard lck{ mtx_ };
				task = std::move(tasks_.front());
				tasks_.pop_front();
			}
			task();
		}
		bool Empty() const;
	private:
		// functions
		void PushTask_(Task&& taskptr);
		// data
		std::mutex mtx_;
		std::deque<Task> tasks_;
	};
}