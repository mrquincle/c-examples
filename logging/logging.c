/**
 * Test logging.
 *
 * Goal: check how we can add debug levels in such a way that we have minimal code size bloat.
 *
 * Compile with something like:
 *
 *    gcc -DSERIAL_VERBOSITY=LOG_DEBUG logging.c -o log
 *
 * See Makefile
 */

// As macros we can use the following in precompiler if-clauses.
#define LOG_DEBUG    5
#define LOG_INFO     4
#define LOG_WARNING  3
#define LOG_ERROR    2
#define LOG_FATAL    1
#define LOG_NONE     0

// On an embedded platform, it's something similar. We either exclude all kind of string manipulation
// libraries or complete serial drivers.
#if SERIAL_VERBOSITY > LOG_NONE
#include <stdio.h>
#endif

// We want to temporarily set e.g. LOG_DEBUG_SMART_SWITCH to LOG_INFO if we are only interested in
// this module. We want to do this outside of this file as preprocessor macro. Here we decide to
// define defaults.
#ifndef LOG_DEBUG_SMART_SWITCH
#define LOG_DEBUG_SMART_SWITCH    LOG_DEBUG
#endif
#ifndef LOG_INFO_SMART_SWITCH
#define LOG_INFO_SMART_SWITCH     LOG_INFO
#endif
#ifndef LOG_WARNING_SMART_SWITCH
#define LOG_WARNING_SMART_SWITCH  LOG_WARNING
#endif
#ifndef LOG_ERROR_SMART_SWITCH
#define LOG_ERROR_SMART_SWITCH    LOG_ERROR
#endif
#ifndef LOG_FATAL_SMART_SWITCH
#define LOG_FATAL_SMART_SWITCH    LOG_FATAL
#endif

const char max_debug_levels = 9;

/**
 * This is just the printf statement itself. The contents of this macro will be pasted in by the
 * preprocessor so we have the right __FILE__ and __LINE__.
 */
#define _log_internal(fmt, ...) \
	do { \
		printf("[%-30.30s: %-4d] ", __FILE__, __LINE__); \
		printf(fmt, ##__VA_ARGS__); \
		printf("\n"); \
	} while(0)

/**
 * Undef completely when we don't have logging at all.
 */
#if SERIAL_VERBOSITY <= LOG_NONE
#undef _log_internal
#define _log_internal(fmt, ...) do { } while(0)
#endif

/**
 * If the verbosity level is not a precompiler macro the compiler won't optimize it out. This is a
 * trick to force only LOG_INFO etc in the _log macros. If it's for example LOG_INFO, the 4 will
 * become 4.0. If it is "i", it will become "i.0" which won't compile and that's what we want.
 */
#define ENFORCE_INTEGER_LITERAL(literal) literal ## .0

/**
 * Here we have defined an if-statement within the macro. This indeed shows the proper __FILE__ and
 * __LINE__, however, if "level" is "not easy enough", the compiler won't optimize it.
 *
 * The do {} while(0) is just basic trick so we don't accidentally jump out of if-else statements by
 * dangling semicolons. It also enforces a semicolon after the macro, making it more function-like.
 *
 * The most awesomest would be a method in which we can use conditionals in this define for the
 * precompiler.
 */
#define _log(level, fmt, ...) \
	do { \
		if (ENFORCE_INTEGER_LITERAL(level) && (level <= SERIAL_VERBOSITY)) { \
			_log_internal(fmt, ##__VA_ARGS__); \
		} \
	} while(0)

#define LOGi(fmt, ...) do { _log(LOG_INFO, fmt, ##__VA_ARGS__); } while(0)
#define LOGf(fmt, ...) do { _log(LOG_FATAL, fmt, ##__VA_ARGS__); } while(0)

#if SERIAL_VERBOSITY < LOG_INFO
	#undef LOGi
	#define LOGi(fmt, ...) do { } while(0)
#endif

#define FORBIDDEN_STATEMENTS
#undef FORBIDDEN_STATEMENTS

int main() {
	/*
	 * This is how we can send a log message at a certain level, if you run only the preprocessor:
	 *    gcc -DSERIAL_VERBOSITY=LOG_FATAL -E logging.c -o logging_pre.c
	 * then you will see that this resolves to:
	 *
	 *   if (4 <= 1) { printf("[%-30.30s: %-4d] ", "logging.c", 48); ... };
	 *
	 * This is something the compiler can optimize in such way that the printf statement is not called.
	 * It already knows that 4 is smaller than 1.
	 */
	_log(LOG_INFO, "Log test!");
	
	_log(LOG_DEBUG_SMART_SWITCH, "Log test of module!");

#ifdef FORBIDDEN_STATEMENTS
	const char level = LOG_INFO;
	_log(level, "Log test!");
#endif

	/*
	 * Extreme case of ripping everything out.
	 */
	LOGi("Check precompiler output!");

	/*
	 * Iterate over a couple of levels (they will be shown up to SERIAL_VERBOSITY)
	 *
	 * Here, the preprocessor will generate:
	 *
	 *   if (i <= 1) { printf("[%-30.30s: %-4d] ", "logging.c", 58); ... };
	 *
	 * And this statement the compiler (on my system) will be checked at runtime. You can check with
	 * nm on the binary build and you will see something like `printf@@GLIBC_2.2.5` or alike.
	 *
	 * Likewise (inspecting the log binary with the `strings` utility):
	 *
	 * $> strings log | grep Hello
	 *   Hello debug level %i
	 * $> strings log | grep Log
	 *
	 * No strings in the binary with the above call, but in the for-loop we see the strings.
	 *
	 * The most awesomest would be a way in which calling it like this is forbidden.
	 */
#ifdef FORBIDDEN_STATEMENTS
	for (char i = 3; i < max_debug_levels; ++i) {
		_log(i, "Hello debug level %i", i);
	}
#endif
	return 0;
}
