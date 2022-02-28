// Copyright 2017-2022 NEC Laboratories Europe

// Tungl: Iclandic for "Moon"
#pragma once
#include <tungl/version.h>

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

typedef enum {
	TUNGL_LEVEL_ERROR	= 1,
	TUNGL_LEVEL_INFO	= 2,
	TUNGL_LEVEL_WARN	= 3,
	TUNGL_LEVEL_DEBUG	= 4,
	TUNGL_LEVEL_TRACE	= 5
} tungl_level_t;

inline void tungl_nop(void) {}

TUNGL_API	void				tungl_log			(const tungl_level_t level, const char* module, const char* file, const int line, const char* msg, ...);
TUNGL_API	TUNGL_NORETURN void	tungl_throw			(const char* module, const char* file, const int line, const char* msg, ...);
TUNGL_API	void				tungl_set_level_str	(const char* str);
TUNGL_API	void				tungl_set_level		(const tungl_level_t level);
TUNGL_API	int					tungl_is_active		(const tungl_level_t level);
TUNGL_API	tungl_level_t		tungl_get_level		(void);
