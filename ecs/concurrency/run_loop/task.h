#pragma once
#include <memory_resource>
namespace nx
{
	
	template<typename Func>
	concept TaskCallable = requires(Func&& func) {
		{ std::forward<Func>(func)() } -> std::same_as<void>;
	};
	
	class Task
	{
	public:
		Task() = default;
		virtual ~Task() = default;
		virtual void operator()() = 0;
		virtual void Destroy() noexcept =0;
	};



	template<TaskCallable Func>
	class TaskWrapper : public Task {

	public:
		TaskWrapper(Func&& func,std::pmr::memory_resource* res = std::pmr::get_default_resource())
			: m_func(std::forward<Func>(func)), resource(res) {}
		void operator()() override {
			m_func();
		}
		~TaskWrapper() override = default;

		void Destroy() noexcept override {
			std::pmr::polymorphic_allocator<TaskWrapper<Func>> alloc(resource);
			alloc.delete_object(this);
		}


	private:
		std::pmr::memory_resource* resource;
		Func m_func;
	};


	template<TaskCallable Func>
	Task* MakeTask(Func&& func, std::pmr::memory_resource* res = std::pmr::get_default_resource()) noexcept
	{
		std::pmr::polymorphic_allocator<TaskWrapper<Func>> alloc(res);
		return alloc.new_object<TaskWrapper<Func>>(std::forward<Func>(func), res);
	}
	

}