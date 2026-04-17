#include "deque.h"

nx::detail::WSQImpl::WSQImpl(WSQImpl&& other) noexcept
    : resource(std::exchange(other.resource, nullptr)),
      buffer(std::exchange(other.buffer, nullptr)),
      capacity(std::exchange(other.capacity, 0)),
      top(other.top.load(std::memory_order_relaxed)),
	bottom(other.bottom.load(std::memory_order_relaxed))
{

}

nx::detail::WSQImpl& nx::detail::WSQImpl::operator=(WSQImpl&& other) noexcept
{
    if (this != &other) {
        Deallocate();
        resource = std::exchange(other.resource, nullptr);
        buffer = std::exchange(other.buffer, nullptr);
        capacity = std::exchange(other.capacity, 0);
        top.store(other.top.load(std::memory_order_relaxed), std::memory_order_relaxed);
        bottom.store(other.bottom.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }
	return *this;
}

void nx::detail::WSQImpl::Initialize(std::size_t capacity, std::pmr::memory_resource* resource) noexcept
{
	this->resource = resource;
	this->capacity = capacity;
	this->buffer = static_cast<void**>(resource->allocate(capacity * sizeof(void*)));
}

void nx::detail::WSQImpl::Deallocate() noexcept
{
	if (buffer) {
		resource->deallocate(buffer, capacity * sizeof(void*));
		buffer = nullptr;
		capacity = 0;
	}
}

bool nx::detail::WSQImpl::Push(void* ptr) noexcept
{
	std::size_t b = bottom.load(std::memory_order_relaxed);
	std::size_t t = top.load(std::memory_order_acquire);

	if (b - t >= capacity) {
		return false;   
	}

	buffer[b % capacity] = ptr;
	bottom.store(b + 1, std::memory_order_release);
	return true;
}

void* nx::detail::WSQImpl::Pop() noexcept
{
    std::size_t b = bottom.load(std::memory_order_relaxed);
    if (b == 0) {
        return nullptr;
    }

    b = b - 1;
    bottom.store(b, std::memory_order_release);

    std::size_t t = top.load(std::memory_order_acquire);
    if (b < t) {
        bottom.store(t, std::memory_order_release);
        return nullptr;
    }

    void* task = buffer[b % capacity];

    if (b > t) {
        return task;
    }

	// last element, need to compete with stealers
    if (top.compare_exchange_strong(t, t + 1,
        std::memory_order_release,
        std::memory_order_relaxed)) {
        bottom.store(t + 1, std::memory_order_release);
        return task;
    }
    else {
        bottom.store(t + 1, std::memory_order_release);
        return nullptr;
    }
}

void* nx::detail::WSQImpl::Steal() noexcept
{
    std::size_t t = top.load(std::memory_order_acquire);
    std::size_t b = bottom.load(std::memory_order_acquire);

    if (t >= b) {
        return nullptr;   
    }

    void* task = buffer[t % capacity];

 
    if (top.compare_exchange_weak(t, t + 1,
        std::memory_order_release,
        std::memory_order_relaxed)) {
        return task;
    }

    return nullptr;  
}

bool nx::detail::WSQImpl::Empty() const noexcept
{
        std::size_t t = top.load(std::memory_order_acquire);
		std::size_t b = bottom.load(std::memory_order_acquire);
		return t >= b;
}





