#pragma once
#include <atomic>
#include <memory_resource>
namespace nx {

	namespace detail
	{
		struct WSQImpl {
			WSQImpl() = default;
			WSQImpl(const WSQImpl&) = delete;
			WSQImpl& operator=(const WSQImpl&) = delete;

			WSQImpl(WSQImpl&&) noexcept;
			WSQImpl& operator=(WSQImpl&&) noexcept;

			void Initialize(std::size_t capacity, std::pmr::memory_resource* resource)noexcept;
		

			void Deallocate()noexcept;
		

			bool Push(void* ptr)noexcept;

			void* Pop()noexcept;

			void* Steal() noexcept;

			bool Empty() const noexcept;
		

			std::pmr::memory_resource* resource;
			void** buffer = nullptr;
			std::size_t capacity = 0;
			std::atomic<std::size_t> top = 0;
			std::atomic<std::size_t> bottom = 0;
		};
	}

	template<typename T>
	concept Pointer = std::is_pointer_v<T>;

	template<Pointer T>
	class WorkStealDeque
	{
	public:
		WorkStealDeque (std::size_t capacity, std::pmr::memory_resource* resource)
		{
			m_impl.Initialize(capacity, resource);
		}

		WorkStealDeque(const WorkStealDeque&) = delete;
		WorkStealDeque& operator=(const WorkStealDeque&) = delete;

		WorkStealDeque(WorkStealDeque&& other) noexcept = default;
		WorkStealDeque& operator=(WorkStealDeque && other) noexcept = default;



		~WorkStealDeque()
		{
			m_impl.Deallocate();
		}

		bool Push(T ptr) {
			return m_impl.Push(static_cast<void*>(ptr));
		}

		T Pop() {
			return static_cast<T>(m_impl.Pop());
		}

		T Steal() {
			return static_cast<T>(m_impl.Steal());
		}

		bool Empty() const noexcept
		{
			return m_impl.Empty();
		}
	private:
		detail::WSQImpl m_impl;
	};
}