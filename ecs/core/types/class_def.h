#pragma once
#include "core/error_code.h"

namespace nx {
	struct ImMoveable {
		ImMoveable() = default;
		ImMoveable(const ImMoveable&) = delete;
		ImMoveable(ImMoveable&&) = delete;
		ImMoveable& operator=(const ImMoveable&) = delete;
		ImMoveable& operator=(ImMoveable&&) = delete;
	};

	struct NoneCopyable {
		NoneCopyable() = default;
		NoneCopyable(const NoneCopyable&) = delete;
		NoneCopyable(NoneCopyable&&) = default;
		NoneCopyable& operator=(const NoneCopyable&) = delete;
		NoneCopyable& operator=(NoneCopyable&&) = default;
	};

	class NonTransferable : private ImMoveable, private NoneCopyable {
	public:
		NonTransferable() = default;
		~NonTransferable() = default;
	};


	class IComponent {
	public:

		virtual void PreInitialize() noexcept{}
		virtual Error Initialize() noexcept= 0;
		virtual void Shutdown() noexcept= 0;

		virtual ~IComponent() = default;
	};
}