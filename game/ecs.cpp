#include <mimalloc.h>
#include "app/app.h"
#include "core//config/console_config.h"
#include "core/log/log.h"


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
