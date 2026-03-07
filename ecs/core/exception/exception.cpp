#include "exception.h"

#include <sstream>
#include <Windows.h>
#include <stacktrace>


nx::NxError::NxError(std::string_view error, std::stacktrace trace)
	:message()
{
	std::ostringstream ss{  };
	ss << error
		<< "\n"
		<< trace
		<< "\n";

	message = ss.str();
}

char const* nx::NxError::what() const
{
	return message.c_str();
}

nx::NxSystemError::NxSystemError(std::string_view error)
	:NxError(_FormatStstemMessage(error))
{


}

std::pmr::string nx::NxSystemError::_FormatStstemMessage(std::string_view error)
{
	char buffer[256];
	DWORD errorCode = GetLastError();
	if (errorCode == 0) {
		return "unknowed error";
	}

	 FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		0,
		buffer,
		sizeof(buffer),
		NULL
	);

	std::stringstream ss;
	ss << error << ":"
		<< buffer;

	std::pmr::string s{};
	s = ss.str();
	return s;
}

