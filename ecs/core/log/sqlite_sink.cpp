module;
#include <chrono>
#include <spdlog/spdlog.h>
#include <sqlite_modern_cpp.h>
#include <filesystem>
module nx.core.log.sqlite_sink;

static  std::string GetLogPath() {
	return  (std::filesystem::current_path() / "data"/"logs.db").string();
}


nx::SqliteLogSink::SqliteLogSink()
	:m_database(GetLogPath())
{
	std::time_t now_c = time(nullptr);
	std::tm now_tm;
	localtime_s(&now_tm, &now_c);
	char tableName[32];
	std::strftime(tableName, sizeof(tableName), "log_%Y_%m_%d", &now_tm);

	m_tableName = tableName;

	m_database << std::format(
		"CREATE TABLE IF NOT EXISTS {} ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"level TEXT NOT NULL,"
		"logger TEXT NOT NULL,"
		"message TEXT NOT NULL,"
		"function_name TEXT,"
		"source_loc TEXT,"
		"timestamp TEXT NOT NULL"
		");",
		tableName
	);

	m_insertSql = std::format(
		"INSERT INTO {} (timestamp, level, logger, message, function_name, source_loc) "
		"VALUES (?, ?, ? ,?, ?, ?);",
		tableName
	);
}

nx::SqliteLogSink::~SqliteLogSink(){

}

void nx::SqliteLogSink::sink_it_(const spdlog::details::log_msg& msg)
{
	auto time = std::chrono::system_clock::to_time_t(msg.time);
	std::tm time_tm;
	localtime_s(&time_tm, &time);
	char timeStr[64]{};
	std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &time_tm);


	std::string levelStr = spdlog::level::to_string_view(msg.level).data();




	constexpr auto sourceDir = std::string_view(PROJECT_DIR).size();

	auto relativePath = std::string_view(msg.source.filename).substr(sourceDir);

	auto sourceLoc = std::format("{}:{}", relativePath, msg.source.line);


	m_database << m_insertSql
		<< std::string_view(timeStr)
		<< levelStr
		<< std::string_view(msg.logger_name)
		<< std::string_view(msg.payload)
		<< std::string_view(msg.source.funcname)
		<< sourceLoc;
}
