#pragma once
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

