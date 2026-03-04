module;
#include <format>
#include "log/log.h"

module game.app;
import nx.core.log;
import nx.core.config.console;
import nx.core.memory;
LOGGER(app);




game::App::App(int argc, char **argv) {

}

void game::App::Quit()
{
    m_event_loop.PostQuit();
}

game::App::~App()
{

}


void game::App::Run()
{
    m_event_loop.Run();
}


