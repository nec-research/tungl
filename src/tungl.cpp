#include <string_view>
#include <string>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <cstdint>
#include <iomanip>

// required to always have ASSERT enabled
#ifdef NDEBUG 
#undef NDEBUG
#endif
#include <cassert>

#include <tungl/tungl.h>
#include <tungl/config.h>
#include <tungl/colors.h>

//------------------------------------------------------------------------------
inline uint64_t tungl_time(void) {
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//------------------------------------------------------------------------------
static uint64_t			tungl_start	= tungl_time();
static tungl_level_t	tungl_level	= TUNGL_LEVEL_WARN;

//------------------------------------------------------------------------------
TUNGL_API int	tungl_is_active(const tungl_level_t level)	{	return level <= tungl_level;	}
TUNGL_API void	tungl_set_level(const tungl_level_t level)	{	tungl_level = level;			}

//------------------------------------------------------------------------------
TUNGL_API void tungl_set_level_str(const char* str) {
	const std::string_view level(str);

	if		(level == "TRACE")	tungl_set_level(TUNGL_LEVEL_TRACE);
	else if	(level == "WARN")	tungl_set_level(TUNGL_LEVEL_WARN);
	else if	(level == "INFO")	tungl_set_level(TUNGL_LEVEL_INFO);
	else if	(level == "DEBUG")	tungl_set_level(TUNGL_LEVEL_DEBUG);
	else if	(level == "ERROR")	tungl_set_level(TUNGL_LEVEL_ERROR);
}

//------------------------------------------------------------------------------
inline std::string tungl_format(const tungl_level_t level, std::string_view module, std::string_view file, const int line) {
	size_t space		= TUNGL_PROLOG_LEN;
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
			module_dots	= true;
			mod_size	= TUNGL_MAX_MODULE_LEN - TUNGL_DOTS_LEN;
			module		= {module.begin(), mod_size};
		}

		space -= mod_size + 3; // "[" << module << "] "
	} else {
		space -= 1; // " "
	}

	// File --------------------------------------------------------------------
	auto file_begin	= file.rfind('/');
	if(file_begin == std::string_view::npos)	file_begin = 0;
	else										file_begin++;

	auto file_end	= file.rfind('.'); assert(file_end != std::string_view::npos);
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

	float time = (tungl_time() - tungl_start)/1000.0f;
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

	for(int i = 0; i < space; i++)
		g << ' ';

	return g.str();
}

//------------------------------------------------------------------------------
inline void tungl_log_impl(const tungl_level_t level, const std::string_view module, const std::string_view file, const int line, const std::string msg, va_list args) {
	if(!tungl_is_active(level))
		return;

	auto format = tungl_format(level, module, file, line);

	std::ostringstream g;

	std::istringstream in(msg);
	std::string lin;

	std::string_view color_begin("");

	switch(level) {
		case TUNGL_LEVEL_DEBUG:	color_begin = C_DEBUG;	break;
		case TUNGL_LEVEL_ERROR:	color_begin = C_ERROR;	break;
		case TUNGL_LEVEL_WARN:	color_begin = C_WARN;	break;
	}

	std::string_view color_end(color_begin.size() ? L_RESET : "");

	while(in.good()) {
		lin.clear();
		std::getline(in, lin);
		if(lin.size())
			g << format << color_begin << lin << color_end << std::endl;
	}

	auto newMsg = g.str();
	vprintf(newMsg.c_str(), args);
}

//------------------------------------------------------------------------------
TUNGL_API void tungl_log(const tungl_level_t level, const char* module, const char* file, const int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	tungl_log_impl(level, module, file, line, msg, args);
	va_end(args);
}

//------------------------------------------------------------------------------
TUNGL_API TUNGL_NORETURN void tungl_throw(const char* module, const char* file, const int line, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	tungl_log_impl(TUNGL_LEVEL_ERROR, module, file, line, msg, args);
	va_end(args);
	fflush(0);
	assert(false);
	abort();
}

#if 0
//------------------------------------------------------------------------------
void glog(const std::string_view& file, const int line, const std::string_view& module, const Level level, gen::Func func, const std::string_view& color) {
	if(active(level)) {
		Log g(file, line, module, level);
		auto clevel = level.toString();
		g >> color << "# " L_RESET L_EMPH "SOL " << clevel << L_RESET << color << " " << std::setfill('#') << std::setw(75 - clevel.size()) << L_RESET << std::setfill(' ');
		g.ln();
		g >> L_EMPH << func << L_RESET;
		g.ln();
		g >> color << "##############################################################################" << L_RESET;
		g.ln();
		g.reset();
		g.eol();
	}
}
#endif

//------------------------------------------------------------------------------
