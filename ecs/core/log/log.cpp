#include "log.h"
#include <print>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "sqlite_sink.h"


static const std::vector<spdlog::sink_ptr>& GetSinks()
{
	static std::vector<spdlog::sink_ptr> sinks;
	static std::once_flag flag;
	std::call_once(flag, []() {
		sinks.push_back(std::make_shared<nx::SqliteLogSink>());
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	});
	return sinks;
}

static auto MakeLoggerIfNotExists(const std::string& loggerName)
{
	if (!spdlog::thread_pool()) {
		spdlog::init_thread_pool(8192, 1);
	}
	auto async_logger = spdlog::get(loggerName.data());
	if (async_logger == nullptr)
	{
		async_logger = std::make_shared<spdlog::async_logger>(loggerName.data(),
        GetSinks().begin(), GetSinks().end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
		spdlog::register_logger(async_logger);
		async_logger->set_level(spdlog::level::trace);
		async_logger->flush_on(spdlog::level::err);

		async_logger->set_pattern("[%H:%M:%S.%e][%l][%n]%v");
	}
	return async_logger;

}





void nx::InitializeLogSystem()
{
	spdlog::set_default_logger( MakeLoggerIfNotExists("default"));
}

void nx::ShutdownLogSystem()
{
	spdlog::shutdown();
}


nx::Logger::Logger(const std::string& loggerName)
{
	try {
	 ptr = MakeLoggerIfNotExists(loggerName).get();
	}catch(std::exception& e) {
		std::println(stderr, "{}",e.what());
	}
}

void nx::Logger::Log(Level l, std::string_view messgae,std::source_location loc)
{
	spdlog::level::level_enum spdlogLevel;
	switch (l)
	{
	case Level::Trace:
		spdlogLevel = spdlog::level::trace;
		break;
	case Level::Debug:
		spdlogLevel = spdlog::level::debug;
		break;
	case Level::Info:
		spdlogLevel = spdlog::level::info;
		break;
	case Level::Warn:
		spdlogLevel = spdlog::level::warn;
		break;
	case Level::Error:
		spdlogLevel = spdlog::level::err;
		break;
	case Level::Critical:
		spdlogLevel = spdlog::level::critical;
		break;
	default:
		spdlogLevel = spdlog::level::info;
		break;
	}
	
	spdlog::logger* logger = static_cast<spdlog::logger*>(this->ptr);
	
	logger->log(spdlog::source_loc{ loc.file_name(),(int)loc.line(),loc.function_name()}, spdlogLevel, messgae);


}
