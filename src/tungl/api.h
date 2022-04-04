// Copyright 2022 NEC Laboratories Europe

// Tungl: Iclandic for "Moon"
#pragma once
#include <tungl/version.h> // gets generates from CMake into a different folder
#include "types.h"
#include "config.h"
#include <stdarg.h>

#ifdef __cplusplus
#define TUNGL_API extern "C"
#else
#define TUNGL_API
#endif

#if __cplusplus >= 201103L
#define TUNGL_NORETURN [[noreturn]]
#else
#define TUNGL_NORETURN
#endif

inline void tungl_nop(void) {}

TUNGL_API	const char*		tungl_color						(const tungl_level_t level);
TUNGL_API	int				tungl_is_active					(const tungl_level_t level);
TUNGL_API	int				tungl_is_color					(void);
TUNGL_API	tungl_level_t	tungl_get_level					(void);
TUNGL_API	tungl_level_t	tungl_get_level_str				(const char* str);
TUNGL_API	void			tungl_log						(const tungl_level_t level, const char* module, const char* file, const int line, const char* msg, ...);
TUNGL_API	void			tungl_set_color					(const int enabled);
TUNGL_API	void			tungl_set_color_str				(const char* str);
TUNGL_API	void			tungl_set_level					(const tungl_level_t level);
TUNGL_API	void			tungl_set_level_str				(const char* str);
TUNGL_API	void			tungl_throw		TUNGL_NORETURN	(const char* module, const char* file, const int line, const char* msg, ...);
TUNGL_API	void			tungl_vlog						(const tungl_level_t level, const char* module, const char* file, const int line, const char* msg, va_list args);
TUNGL_API	void			tungl_vthrow	TUNGL_NORETURN	(const char* module, const char* file, const int line, const char* msg, va_list args);

#if TUNGL_WITH_FILE
TUNGL_API	const char*		tungl_get_file					(void);
TUNGL_API	void			tungl_set_file					(const char* file);
#endif
