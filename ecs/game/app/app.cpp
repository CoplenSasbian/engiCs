module;
#include <format>
#include "log/log.h"

module game.app;
import nx.core.log;
import nx.core.config.console;
import nx.core.memory;
LOGGER(app);




game::App::App(int argc, char **argv) {
	try {
		SetupConsole();
		InitResource();
		InitializeLogSystem();

		m_context.PreInitialize();
		m_window.PreInitialize();
		m_eventLoop->PreInitialize();

		m_context.Initialize();
		m_window.Initialize();
		m_eventLoop->Initialize();



		m_eventLoop->OnIdle = [this]()
		{
			m_context.PollUiTask();
		};

		_logger.Info("App initlize");


	}catch (std::exception &e) {
		_logger.Error(e.what());
	}
}

game::App::~App()
{
	_logger.Info("App shutdown");
	m_context.Shutdown();
	nx::ShutdownResource();

}


void game::App::Run()
{
	m_eventLoop->PoolEvents();
}


