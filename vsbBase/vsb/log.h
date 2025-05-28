//(C) 2025 Alexander Samarin

#pragma once

#include "spdlog/spdlog.h"

namespace vsb::log
{
	enum class Level
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};


	//init and wrap up
	void Init(bool enableConsole, std::string_view logFile = {});
	void SetLevel(Level level);
	void Uninit();


	// ---- Debug-Only Macros (Compile Out in Release) ----
	#if !defined(NDEBUG)
	#define VSBLOG_TRACE(fmt, ...)   spdlog::trace("[{}:{}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
	#define VSBLOG_DEBUG(fmt, ...)   spdlog::debug("[{}:{}] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
	#define VSBLOG_VERBOSE(fmt, ...) spdlog::debug("[{}:{}][{}] " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
	#else
	#define VSBLOG_TRACE(...)
	#define VSBLOG_DEBUG(...)
	#define VSBLOG_VERBOSE(...)
	#endif


	// ---- Utility Macros (Always Available) ----
	#define VSBLOG_INFO(fmt, ...)     spdlog::info(fmt, ##__VA_ARGS__)
	#define VSBLOG_WARN(fmt, ...)     spdlog::warn(fmt, ##__VA_ARGS__)
	#define VSBLOG_ERROR(fmt, ...)    spdlog::error(fmt, ##__VA_ARGS__)
	#define VSBLOG_CRITICAL(fmt, ...) spdlog::critical(fmt, ##__VA_ARGS__)

	#define VSBLOG_IF(condition, fmt, ...) if (condition) { spdlog::info(fmt, ##__VA_ARGS__); }


}