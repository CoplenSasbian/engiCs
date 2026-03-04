#include <chrono>
#include <mimalloc.h>


import nx.core.config.console;
import nx.core.memory;
import nx.core.log;
import game.app;


using namespace std::chrono_literals;


int main(int argc, char** argv)
{
    mi_version();
    nx::InitResource();
    nx::SetupConsole();
    nx::InitializeLogSystem();
    game::App app{argc,argv};
    app.Run();
    return 0;
}
