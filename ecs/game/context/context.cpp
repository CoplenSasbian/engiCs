module game.context;

void game::Context::PreInitialize() {
	// m_tp = MakeWinrtPool();
}

void game::Context::Initialize()
{
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


