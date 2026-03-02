module;
#include <memory>
#include <optional>

export module game.context;
import game.error_code;
import nx.concurrency.threadpool;
import nx.core.types;


export namespace game {
	using namespace nx;
	class Context :IComponent {
	public:
		void PreInitialize() override;

		Error Initialize() override;

		void Shutdown() override;

		void PollUiTask();
		Threadpool* GetThreadPool() const;



	private:
		std::shared_ptr<Threadpool> m_tp;




	};

}