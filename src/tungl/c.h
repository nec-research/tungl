// Copyright 2022 NEC Laboratories Europe

#pragma once

#include "types.h"
#include "colors.h"

#ifndef L_MODULE
#define L_MODULE 0
#endif

#ifndef L_LOG
#define L_LOG(LEVEL, ...)	(tungl_is_active(LEVEL) ? tungl_log(LEVEL, __VA_ARGS__) : tungl_nop())
#endif

#ifndef L_THROW
#define L_THROW(...)		tungl_throw(__VA_ARGS__)
#endif

#define L_STR(...) L_STR_(__VA_ARGS__)
#define L_STR_(...) #__VA_ARGS__

#define L_INFO(...)			L_LOG(TUNGL_LEVEL_INFO,  L_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define L_ERROR(...)		L_LOG(TUNGL_LEVEL_ERROR, L_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define L_WARN(...)			L_LOG(TUNGL_LEVEL_WARN,  L_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define L_DEBUG(...)		L_LOG(TUNGL_LEVEL_DEBUG, L_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define L_TRACE(...)		L_LOG(TUNGL_LEVEL_TRACE, L_MODULE, __FILE__, __LINE__, __VA_ARGS__)

#define THROWAT(...)		L_THROW(__VA_ARGS__)
#define THROW(...) 			THROWAT(L_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define THROWIF(COND, ...)	(__builtin_expect((COND) != 0, 1)) ? THROW(__VA_ARGS__) : tungl_nop()
#define ASSERT(...) 		THROWIF(!(__VA_ARGS__), "Assertion '" L_STR(__VA_ARGS__) "' failed!")
#define FAIL()				THROW("Implementation Error")
#define FAILIF(...)			THROWIF((__VA_ARGS__), "Implementation Error")
#define TODO(...)			THROW("ERROR 404: Ups... you reached the end of this application")

#include "api.h"
