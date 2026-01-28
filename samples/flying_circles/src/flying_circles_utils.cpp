//
// Created by Alexander on 12.01.2026.
//

#include "flying_circles_utils.h"

#include <climits>
#include <cstdlib>

#include "raylib.h"
#include "vsb/log.h"

float fc_utils::GetRandomFloat(float min, float max)
{
	const float scale = GetRandomFloat01();
	return min + scale * (max - min);
}


float fc_utils::GetRandomFloat01()
{
	return static_cast<float>(GetRandomValue(0, INT_MAX)) / static_cast<float>(INT_MAX);
}


void fc_utils::MyRaylibLogCallback(int logLevel, const char* text, va_list args)
{
	// Format the message
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), text, args);

	// Route to your logging system based on level
	switch (logLevel)
	{
		case LOG_TRACE:
		case LOG_DEBUG:
			VSBLOG_INFO(buffer);//;
			break;
		case LOG_INFO:
			VSBLOG_INFO(buffer);
			break;
		case LOG_WARNING:
			VSBLOG_WARN(buffer);
			break;
		case LOG_ERROR:
			VSBLOG_ERROR(buffer);
		case LOG_FATAL:
			VSBLOG_CRITICAL(buffer);
			break;
		default:
			VSBLOG_INFO(buffer);
			break;
	}
}
