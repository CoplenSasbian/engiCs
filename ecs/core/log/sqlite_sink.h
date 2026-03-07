
#pragma once
#include <spdlog/sinks/base_sink.h>
#include <sqlite_modern_cpp.h>
#include <memory_resource>
 namespace nx
{

	class SqliteLogSink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:

		SqliteLogSink();
		~SqliteLogSink() override;

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override {};
	private:
		std::pmr::string m_tableName;
		std::pmr::string m_insertSql;
		sqlite::database m_database;
	};
} // namespace nx