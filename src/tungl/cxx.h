// Copyright 2022 NEC Laboratories Europe

#pragma once

#include "api.h"

#include <sstream>
#include <functional>

inline std::string tungl_log(std::function<void(std::ostream& ss)> func) {
	std::ostringstream ss;
	func(ss);
	return ss.str();
}

inline void tungl_log(const tungl_level_t level, const char* module, const char* file, const int line, std::function<void(std::ostream& ss)> func) {
	if(tungl_is_active(level))
		tungl_log(level, module, file, line, tungl_log(func).c_str());
}

inline void tungl_throw [[noreturn]] (const char* module, const char* file, const int line, std::function<void(std::ostream& ss)> func) {
	tungl_throw(module, file, line, tungl_log(func).c_str());
}

#define L_LOG(LEVEL, MODULE, FILE, LINE, ...)	tungl_log  (LEVEL, MODULE, FILE, LINE, [&](std::ostream& ss) -> void { ss << __VA_ARGS__; })
#define L_THROW(MODULE, FILE, LINE, ...)		tungl_throw(       MODULE, FILE, LINE, [&](std::ostream& ss) -> void { ss << __VA_ARGS__; })

#include "c.h"
