//(C) 2025 Alexander Samarin

#include "log.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace vsb::log
{
	static std::shared_ptr<spdlog::logger> s_logger;

	static const std::string LoggerName = "Game";
	constexpr auto LogPattern = "[%H:%M:%S.%e] [%^%l%$] %v";

	void Init(const bool enableConsole, std::string_view logFile)
	{
		Uninit();

		std::vector<spdlog::sink_ptr> sinks;

		if (enableConsole)
		{
			const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			console_sink->set_pattern(LogPattern);
			sinks.push_back(console_sink);
		}

		if (!logFile.empty())
		{
			const auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::string(logFile));
			file_sink->set_pattern(LogPattern);
			sinks.push_back(file_sink);
		}

		s_logger = std::make_shared<spdlog::logger>(LoggerName, sinks.begin(), sinks.end());
		spdlog::set_default_logger(s_logger);
	}


	void InitForTests()
	{
		Uninit();

		// Create stderr sink for tests to avoid interfering with Catch2's JSON output
		const auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
		stderr_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
		
		std::vector<spdlog::sink_ptr> sinks = { stderr_sink };
		
		s_logger = std::make_shared<spdlog::logger>(LoggerName, sinks.begin(), sinks.end());
		spdlog::set_default_logger(s_logger);
	}


	void SetLevel(Level level)
	{
		spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
	}


	void Uninit()
	{
		if (s_logger)
		{
			s_logger->flush();
			spdlog::drop(LoggerName);  // Remove our specific logger
			spdlog::shutdown();    // Optional: Full teardown
			s_logger.reset();      // Clear shared_ptr
		}
	}
}