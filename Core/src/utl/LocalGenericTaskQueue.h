#pragma once
#include <functional>
#include <deque>
#include <memory>
#include <future>

namespace tryn::utl
{
	class LocalGenericTaskQueue
	{
	public:
		using Task = std::move_only_function<void()>;
		template<std::invocable InputTask, typename... Args>
		void Push(std::shared_ptr<InputTask> inputTask, Args&&...args)
		{
			tasks_.push_back([args_ = std::make_tuple(std::forward<Args>(args)...), callback = inputTask]() {
				std::apply(*callback, std::move(args_));
				});
		}
		template<std::invocable InputTask, typename... Args>
		void Push(std::unique_ptr<InputTask>&& inputTask, Args&&...args)
		{
			tasks_.push_back([args_ = std::make_tuple(std::forward<Args>(args)...), callback = std::move(inputTask)]() {
				std::apply(*callback, std::move(args_));
				});
		}
		template<std::invocable InputTask, typename... Args>
		void Push(InputTask&& inputTask, Args&&...args)
		{
			tasks_.push_back([args_ = std::make_tuple(std::forward<Args>(args)...), callback = std::forward<InputTask>(inputTask)]() {
				std::apply(callback, std::move(args_));
				});
		}
		auto PopExecute()
		{
			Task task = std::move(tasks_.front());
			task();
			tasks_.pop_front();
		}
		bool Empty()
		{
			return tasks_.empty();
		}
		std::size_t Size()
		{
			return tasks_.size();
		}
	private:
		std::deque<Task> tasks_;
	};
}