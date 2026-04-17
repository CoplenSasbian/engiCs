#pragma once
#include "platform/win32/window/window.h"
#include "platform/win32/event_loop//message_loop.h"
#include "concurrency/threadpool/threadpool.h"
#include "renderer.h"
#include "platform/message_loop/event.h"
namespace game {

	constexpr   nx::PlatformMagic Platform = nx::PlatformWin32;


	class App {
	public:
		App(int argc, char** argv)noexcept;
		void Quit()noexcept;
		~App();
		void Run()noexcept;
		nx::IMessageLoop* GetEventLoop()noexcept;
	protected:
		void RegisterEvent()noexcept;
		nx::Error CreateThreadPool()noexcept;
		nx::Error CreateRenderWindow()noexcept;
		nx::Error CreateRenderer()noexcept;

		bool HandleMainWindowClose (const nx::CloseEvent& e)noexcept;
		void OnMousePress(const nx::MousePressEvent& e)noexcept;
	private:
		nx::MessageLoop<Platform> m_event_loop;
		nx::Window<Platform> m_window;
		nx::CommonPtr<nx::Threadpool> m_threadpool;
		renderer::Renderer m_renderer;
	};
}
