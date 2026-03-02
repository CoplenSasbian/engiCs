module;
#include <optional>
module game.context;
import game.error_code;
void game::Context::PreInitialize() {
}

nx::Error game::Context::Initialize()
{
	return NoError;
}

void game::Context::Shutdown()
{
	m_tp.reset();
}

void game::Context::PollUiTask()
{
}

nx::Threadpool* game::Context::GetThreadPool() const {
	return m_tp.get();
}


