module;
#include<stdexcept>
#include<stacktrace>
#include<string>
#include<string_view>
export module nx.core.exception;

export namespace nx {

	struct NxError :std::exception {

		NxError(std::string_view error, std::stacktrace trace = std::stacktrace::current());
		NxError(const NxError&) = default;
		NxError(NxError&&) = default;

		char const* what() const override;
	private:
		std::pmr::string message;
	};

	struct NxSystemError :NxError {
		NxSystemError(std::string_view error);
	private:
		std::pmr::string _FormatStstemMessage(std::string_view error);
	};




}