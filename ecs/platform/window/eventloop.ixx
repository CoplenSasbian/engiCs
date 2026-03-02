module;
#include <memory>
#include <functional>
#include <optional>

export module nx.platform.window.eventloop;
import nx.core.types;
import nx.platform.error_code;

export namespace nx {
	class IEventLoop :public IComponent {
	public:
		virtual Error Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void PoolEvents() = 0;

		virtual ~IEventLoop() = default;

		std::function<void()> OnIdle;
	};

	std::shared_ptr<IEventLoop> MakeWin32EventLoop();

	

}