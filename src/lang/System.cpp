#include <lang/System.hpp>
#include <chrono>
#include <cstdlib> //std::getenv
#include <time.h>

using namespace std::chrono;

namespace {
#define CSI "\x1b["

#define SGR_RESET        CSI "m"

#define SGR_BLACK        CSI "0;30m"
#define SGR_RED          CSI "0;31m"
#define SGR_GREEN        CSI "0;32m"
#define SGR_YELLOW       CSI "0;33m"
#define SGR_BLUE         CSI "0;34m"
#define SGR_MAGENTA      CSI "0;35m"
#define SGR_CYAN         CSI "0;36m"
#define SGR_LIGHTGRAY    CSI "0;37m"
#define SGR_GRAY         CSI "1;30m"
#define SGR_LIGHTRED     CSI "1;31m"
#define SGR_LIGHTGREEN   CSI "1;32m"
#define SGR_LIGHTYELLOW  CSI "1;33m"
#define SGR_LIGHTBLUE    CSI "1;34m"
#define SGR_LIGHTMAGENTA CSI "1;35m"
#define SGR_LIGHTCYAN    CSI "1;36m"
#define SGR_WHITE        CSI "1;37m"

auto nano_start = high_resolution_clock::now();
char levelName[] = {'E', 'W', 'D', 'I', 'N', 0};
const char *levelColor[] = {SGR_RED, SGR_YELLOW, SGR_BLUE, SGR_CYAN, "", SGR_GREEN, SGR_MAGENTA };
}

namespace lang {

boolean Logger::release = false;

void Logger::format(const char *fn, unsigned ln, int level, const char *fmt, va_list& args) const {
	jlong jtm = System.currentTimeMillis();
	int r = 0;
	time_t t = (time_t)(jtm/1000);
	if (jtm >= 0) r = (int)(jtm%1000);
	else if (jtm < 0) r = (int)((-jtm)%1000);
	char buf[30];
	struct tm stm;
	gmtime_r(&t, &stm);
	if (release) {
		strftime (buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime_r(&t, &stm));
		System.out.printf("%s.%03llu %s[%c]: ", buf, r, levelColor[level], levelName[level]);
	}
	else {
		strftime (buf, sizeof(buf), "%H:%M:%S", localtime_r(&t, &stm));
		System.out.printf("%s.%03llu %s[%c] %s(%u): ", buf, r, levelColor[level], levelName[level], fn, ln);
	}
	//System.out.printf("%s.%03llu [%c]: ", buf, r, levelName[level]);
	System.out.print(String::format(fmt, args));
	System.out.println(SGR_RESET);
}

const Logger& Logger::error(const char *fn, unsigned ln, const char *fmt...) const {
	va_list args;
	va_start(args, fmt);
	format(fn, ln, 0, fmt, args);
	va_end(args);
	return *this;
}
const Logger& Logger::warn(const char *fn, unsigned ln, const char *fmt...) const {
	va_list args;
	va_start(args, fmt);
	format(fn, ln, 1, fmt, args);
	va_end(args);
	return *this;
}
const Logger& Logger::debug(const char *fn, unsigned ln, const char *fmt...) const {
	va_list args;
	va_start(args, fmt);
	format(fn, ln, 2, fmt, args);
	va_end(args);
	return *this;
}
const Logger& Logger::info(const char *fn, unsigned ln, const char *fmt...) const {
	va_list args;
	va_start(args, fmt);
	format(fn, ln, 3, fmt, args);
	va_end(args);
	return *this;
}
const Logger& Logger::notice(const char *fn, unsigned ln, const char *fmt...) const {
	va_list args;
	va_start(args, fmt);
	format(fn, ln, 4, fmt, args);
	va_end(args);
	return *this;
}

jlong The_System::currentTimeMillis() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
jlong The_System::nanoTime() {
	return duration_cast<nanoseconds>(high_resolution_clock::now()-nano_start).count();
}

const String The_System::getenv(const String& name) {
	const char *v = std::getenv(name.cstr());
	if (v == null) return String();
	return v;
}

void The_System::arraycopy(const Object& src, int srcPos, Object& dest, int destPos, int length) {
}

} //namespace lang

#ifdef INSTRUMENTATION
/*
 * Instrumentation
 * compile with gcc '-finstrument-functions' flag
 */
__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *this_fn, void *call_site) {
}
__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *this_fn, void *call_site) {
}
#endif
