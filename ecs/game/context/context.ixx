module;
#include <memory>

export module game.context;
import nx.concurrency.threadpool;
import nx.core.types;


export namespace game {
	using namespace nx;
	class Context :IComponent {
	public:
		void PreInitialize() override;
		void Initialize() override;

		void Shutdown() override;

		void PollUiTask();
		Threadpool* GetThreadPool() const;



	private:
		std::shared_ptr<Threadpool> m_tp;




	};

}