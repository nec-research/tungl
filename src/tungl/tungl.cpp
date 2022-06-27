#include <string_view>
#include <string>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <regex>

// required to always have ASSERT enabled
#ifdef NDEBUG 
#undef NDEBUG
#endif
#include <cassert>

#include <tungl/api.h>
#include <tungl/config.h>
#include <tungl/colors.h>

//------------------------------------------------------------------------------
inline uint64_t tungl_time(void) {
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//------------------------------------------------------------------------------
static	uint64_t		s_tungl_start		= 0;
static	tungl_level_t	s_tungl_level		= TUNGL_LEVEL_WARN;
static	bool			s_tungl_color		= true;

#if TUNGL_WITH_FILE
static	FILE*			s_tungl_file_handle	= 0;
static	const char*		s_tungl_file		= 0;
#endif

//------------------------------------------------------------------------------
__attribute__((constructor))
static void tungl_init(void) {
	if(auto env = std::getenv("TUNGL_LOG"))		tungl_set_level_str(env);
	if(auto env = std::getenv("TUNGL_COLOR"))	tungl_set_color_str(env);
	if(auto env = std::getenv("TUNGL_TIME")) {
		s_tungl_start = std::atol(env);
	} else {
		s_tungl_start = tungl_time();
		std::ostringstream g;
		g << s_tungl_start;
		setenv("TUNGL_TIME", g.str().c_str(), 1);
	}

#if TUNGL_WITH_FILE
	if(auto env = std::getenv("TUNGL_FILE"))	tungl_set_file(env);
#endif
}

//------------------------------------------------------------------------------
#if TUNGL_WITH_FILE
__attribute__((destructor))
static void tungl_fini(void) {
	if(s_tungl_file_handle) {
		fflush(s_tungl_file_handle);
		fclose(s_tungl_file_handle);
		s_tungl_file_handle = 0;
	}
}

TUNGL_API const char* tungl_get_file(void) {
	return s_tungl_file;
}

TUNGL_API void tungl_set_file(const char* file)	{
	s_tungl_file = file;

	if(s_tungl_file_handle) {
		fclose(s_tungl_file_handle);
		s_tungl_file_handle = 0;
	}

	if(s_tungl_file) {
		s_tungl_file_handle = fopen(s_tungl_file, "a");
	}
}
#endif

//------------------------------------------------------------------------------
TUNGL_API int			tungl_is_color		(void)						{	return s_tungl_color;										}
TUNGL_API int			tungl_is_active		(const tungl_level_t level)	{	return level <= s_tungl_level;								}
TUNGL_API tungl_level_t	tungl_get_level		(void)						{	return s_tungl_level;										}
TUNGL_API void			tungl_set_color		(const int enabled)			{	s_tungl_color = enabled;									}
TUNGL_API void			tungl_set_level		(const tungl_level_t level)	{	s_tungl_level = level;										}
TUNGL_API void			tungl_set_level_str	(const char* str)			{	tungl_set_level(tungl_get_level_str(str));					}
TUNGL_API void			tungl_set_color_str	(const char* str)			{	tungl_set_color(str ? std::string_view(str) != "OFF" : 1);	}

//------------------------------------------------------------------------------
TUNGL_API tungl_level_t tungl_get_level_str(const char* str) {
	if(!str)
		return TUNGL_LEVEL_ERROR;

	const std::string_view level(str);
	if		(level == "TRACE")	return TUNGL_LEVEL_TRACE;
	else if	(level == "WARN")	return TUNGL_LEVEL_WARN;
	else if	(level == "INFO")	return TUNGL_LEVEL_INFO;
	else if	(level == "DEBUG")	return TUNGL_LEVEL_DEBUG;
	return TUNGL_LEVEL_ERROR;
}

//------------------------------------------------------------------------------
TUNGL_API const char* tungl_color(const tungl_level_t level) {
	switch(level) {
		case TUNGL_LEVEL_DEBUG:	return C_DEBUG;
		case TUNGL_LEVEL_ERROR:	return C_ERROR;
		case TUNGL_LEVEL_INFO:	return C_INFO;
		case TUNGL_LEVEL_TRACE:	return C_TRACE;
		case TUNGL_LEVEL_WARN:	return C_WARN;
	}

	return "";
};

//------------------------------------------------------------------------------
inline std::string tungl_format(const tungl_level_t level, std::string_view module, std::string_view file, const int line) {
	int64_t space		= TUNGL_PROLOG_LEN;
	bool module_dots	= false;
	bool file_dots		= false;

	// Line --------------------------------------------------------------------
	if(line) {
		int line_len = 0; 
		for(int i = line; i != 0; i /= 10, line_len++);
		space -= line_len + 3; // " (" << line << ")"
	}

	// Module ------------------------------------------------------------------
	auto mod_size = module.size();
	if(mod_size) {
		if(mod_size > TUNGL_MAX_MODULE_LEN) {
			module_dots	 = true;
			mod_size	 = TUNGL_MAX_MODULE_LEN - TUNGL_DOTS_LEN;
			space		-= TUNGL_DOTS_LEN;
			module		 = {module.begin(), mod_size};
		}

		space -= mod_size + 3; // "[" << module << "] "
	} else {
		space -= 1; // " "
	}

	// File --------------------------------------------------------------------
	auto file_begin	= file.rfind('/');
	if(file_begin == std::string_view::npos)	file_begin = 0;
	else										file_begin++;

	auto file_end	= file.rfind('.');
	if(file_end == std::string_view::npos)
		file_end = file.size();
	file			= {file.begin() + file_begin, file_end - file_begin};

	if(file.size() > space) {
		file_dots		= true;
		file			= {file.begin(), (std::string_view::size_type)(space - TUNGL_DOTS_LEN)};
		space			= 0;
	} else {
		space -= file.size();
	}

	assert(space >= 0);
	
	// Format String -----------------------------------------------------------
	std::ostringstream g;
	g << '[';
	switch(level) {
		case TUNGL_LEVEL_DEBUG:	g << C_DEBUG	"DEBUG"	L_RESET;	break;
		case TUNGL_LEVEL_ERROR:	g << C_ERROR	"ERROR"	L_RESET;	break;
		case TUNGL_LEVEL_INFO:	g << C_INFO		"INFO "	L_RESET;	break;
		case TUNGL_LEVEL_TRACE:	g << C_TRACE	"TRACE"	L_RESET;	break;
		case TUNGL_LEVEL_WARN:	g << C_WARN		"WARN "	L_RESET;	break;
	}
	g << ']';

	float time = (tungl_time() - s_tungl_start)/1000.0f;
	g << '[' << std::setprecision(2) << std::fixed << std::right << std::setw(6) << time << ']';
	g << L_EMPH;

	if(module.size()) {
		g << '[';
		if(module_dots)
			g << "...";
		g << module << "] ";
	} else {
		g << ' ';
	}

	if(file_dots)
		g << "...";
	g << file;
	
	if(line)
		g << " (" << line << ")";
	g << L_RESET;

	for(int64_t i = 0; i < space; i++)
		g << ' ';

	return g.str();
}

//------------------------------------------------------------------------------
inline void tungl_log_impl(const tungl_level_t level, const char* module_, const char* file_, const int line, const char* msg_, va_list args) {
	static std::regex remove_color("\033[[0-9;]+m");

	if(!tungl_is_active(level))
		return;

	if(!msg_)
		return;

	// C++ doesn't standardize to use nullptr constructions of string_view
	const std::string_view module	(module_ ? module_ : "");
	const std::string_view file		(file_   ? file_   : "");
	const std::string_view msg		(msg_);

	auto format = tungl_format(level, module, file, line);

	std::string_view color_begin("");

	switch(level) {
		case TUNGL_LEVEL_DEBUG:	color_begin = C_DEBUG;	break;
		case TUNGL_LEVEL_ERROR:	color_begin = C_ERROR;	break;
		case TUNGL_LEVEL_WARN:	color_begin = C_WARN;	break;
	}

	std::string_view color_end(color_begin.size() ? L_RESET : "");

	std::ostringstream g;

	for(size_t start = 0; start < msg.size();) {
		auto end = msg.find('\n', start);
		if(end == std::string_view::npos)
			end = msg.size();

		std::string_view line(msg.data() + start, end - start);

		g << format << color_begin << line << color_end;
		if(line[line.size() - 1] != '\r')
			g << std::endl;

		start = end + 1;
	}

	auto newMsg = g.str();

	if(!s_tungl_color)
		newMsg = std::regex_replace(newMsg, remove_color, "");

#if TUNGL_WITH_FILE
	if(s_tungl_file_handle) {
		va_list fargs;
		va_copy(fargs, args);

		vprintf(newMsg.c_str(), args);

		if(s_tungl_color)
			newMsg = std::regex_replace(newMsg, remove_color, "");

		vfprintf(s_tungl_file_handle, newMsg.c_str(), args);

		va_end(fargs);
	} else {
		vprintf(newMsg.c_str(), args);
	}
#else
	vprintf(newMsg.c_str(), args);
#endif
}

//------------------------------------------------------------------------------
TUNGL_API void tungl_vlog(const tungl_level_t level, const char* module, const char* file, const int line, const char* msg, va_list args) {
	tungl_log_impl(level, module, file, line, msg, args);
}

//------------------------------------------------------------------------------
TUNGL_API void tungl_vthrow [[noreturn]] (const char* module, const char* file, const int line, const char* msg, va_list args) {
	tungl_log_impl(TUNGL_LEVEL_ERROR, module, file, line, msg, args);
	fflush(0);
	assert(false);
	abort();
}

//------------------------------------------------------------------------------
TUNGL_API void tungl_log(const tungl_level_t level, const char* module, const char* file, const int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	tungl_log_impl(level, module, file, line, msg, args);
	va_end(args);
}

//------------------------------------------------------------------------------
TUNGL_API void tungl_throw [[noreturn]] (const char* module, const char* file, const int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	tungl_vthrow(module, file, line, msg, args);
	va_end(args);
}

//------------------------------------------------------------------------------
