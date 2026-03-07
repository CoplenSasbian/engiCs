#pragma once
#include <string>
#include "core/types/class_def.h"
#include <source_location>

#ifdef _DEBUG
#define BEGIN_DEBUG_BLOCK() std::stringstream __debug_ss
#define BLOCK_OUTPUT(x) __debug_ss<<(x)
#define END_DEBUG_BLOCK(LEVEL) DEBUG_LOG(LEVEL,__debug_ss.str())
#define DEBUG_LOG(LEVEL,exp) _logger.Log(LEVEL,(exp))
#define DEBUG_TRACE(EXP)	DEBUG_LOG(nx::Level::Trace,(exp))
#define DEBUG_DEBUG(EXP)	DEBUG_LOG(nx::Level::Debug,(exp))
#define DEBUG_INFO(EXP)		DEBUG_LOG(nx::Level::Info,(exp))
#define DEBUG_WARM(EXP)		DEBUG_LOG(nx::Level::Warn,(exp))
#define DEBUG_ERROR(EXP)	DEBUG_LOG(nx::Level::Error,(exp))
#define DEBUG_CRICITAL(EXP) DEBUG_LOG(nx::Level::Critical,(exp))

#else
#define BEGIN_DEBUG_BLOCK()		//	/* ignored in release mode */
#define BLOCK_OUTPUT(x)			//	/* ignored in release mode */
#define END_DEBUG_BLOCK(LEVEL)		//	/* ignored in release mode */
#define DEBUG_LOG(LEVEL,exp)()	//	/* ignored in release mode */
#define DEBUG_TRACE(EXP)		//  /* ignored in release mode */
#define DEBUG_DEBUG(EXP)		//	/* ignored in release mode */
#define DEBUG_INFO(EXP)			//	/* ignored in release mode */
#define DEBUG_WARM(EXP)			//	/* ignored in release mode */
#define DEBUG_ERROR(EXP)		//	/* ignored in release mode */
#define DEBUG_CRICITAL(EXP)		//	/* ignored in release mode */
#endif

#define LOGGER(name) static nx::Logger _logger{#name}


namespace nx
{
    void InitializeLogSystem();
    void ShutdownLogSystem();

    enum class Level
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
