module;
#include <optional>
#include <system_error>
export module nx.core.types;
import nx.core.error_code;

export namespace nx {
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

		virtual void PreInitialize() {}
		virtual Error Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual ~IComponent() = default;
	};
}