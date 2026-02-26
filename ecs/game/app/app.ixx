module;
#include <memory>

export module game.app;

import nx.platform.window;
import game.render_window;
import game.context;
export namespace game {
	using namespace nx;
	class App {
		std::shared_ptr<IEventLoop> m_eventLoop;
		Context m_context;
		RenderWindow m_window;


	public:
		App(int argc, char** argv);
		~App();
		void Run();

	};
}