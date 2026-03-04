module;

export module game.app;
import nx.platform.win32;
import nx.platform.win32.event_loop;
import nx.core.memory;
import game.render_window;
export namespace game {
	using namespace nx;

		constexpr   PlatformMagic Platform = PlatformWin32;


	class App {
	public:




		App(int argc, char** argv);
		void Quit();
		~App();
		void Run();

		IEventloop* GetEventLoop() {
			return &m_event_loop;
		}
	private:
		nx::EventLoop<PlatformMagic> m_event_loop;
	};
}