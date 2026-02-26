module;
#include <string>
#include <memory_resource>
#include <source_location>

export module nx.core.log;
import nx.core.types;
export namespace nx
{
	void InitializeLogSystem();
	void ShutdownLogSystem();

	enum Level
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};



	struct Logger : nx::NoneCopyable {
		Logger(const std::string& loggerName = __FILE__);
		Logger(Logger&&) noexcept = default;
		void Log(Level, std::string_view, std::source_location = std::source_location::current());

#define DEFINE_LOG_LEVEL_FUNC(level) \
		void level(std::string_view fmt,std::source_location loc = std::source_location::current()) {\
			Log(Level::level,fmt,loc);\
		}

		DEFINE_LOG_LEVEL_FUNC(Trace);

		DEFINE_LOG_LEVEL_FUNC(Debug);

		DEFINE_LOG_LEVEL_FUNC(Info);

		DEFINE_LOG_LEVEL_FUNC(Warn);

		DEFINE_LOG_LEVEL_FUNC(Error);

		DEFINE_LOG_LEVEL_FUNC(Critical);

#undef DEFINE_LOG_LEVEL_FUNC



	private:
		void* ptr;
	};
}
