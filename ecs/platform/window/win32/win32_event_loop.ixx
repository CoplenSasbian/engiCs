module;
#include <optional>
export module nx.platform.window.win32.eventloop;

import nx.platform.window.eventloop;

export namespace nx {

	class Win32EventLoop : public IEventLoop {
	public:
		Win32EventLoop();
		~Win32EventLoop() override = default;

		Error Initialize() override;

		void Shutdown() override;
		void PoolEvents() override;
	};

}