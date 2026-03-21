#pragma once
#include "platform/win32/window/window.h"
#include "platform/win32/event_loop//message_loop.h"
#include "concurrency/threadpool/threadpool.h"
#include "renderer.h"
namespace game {

	constexpr   nx::PlatformMagic Platform = nx::PlatformWin32;


	class App {
	public:
		App(int argc, char** argv);
		void Quit();
		~App();
		void Run();
		nx::IMessageLoop* GetEventLoop();

	private:
		nx::MessageLoop<Platform> m_event_loop;
		nx::Window<Platform> m_window;
		nx::CommonPtr<nx::Threadpool> m_threadpool;
		renderer::Renderer m_renderer;
	};
}
