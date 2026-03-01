module;
#include <optional>
module game.context;

void game::Context::PreInitialize() {
}

std::optional<nx::NxError> game::Context::Initialize()
{
	return std::nullopt;
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


