#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOGGING 1

#if LOGGING

#define SLOG(stream, ...) { \
	fprintf(stream, "%s (%d): ", __FILE__, __LINE__); \
	fprintf(stream, __VA_ARGS__);  \
	fprintf(stream, "\n"); \
}

#define LOG(...) { SLOG(stderr, __VA_ARGS__); }

#define NOTIFY(...) { \
	fprintf(stderr, ANSI_COLOR_BLUE); \
	fprintf(stderr, __VA_ARGS__ );  \
	fprintf(stderr, ANSI_COLOR_RESET); \
	fprintf(stderr,  "\n"); \
}


#else // logging turned off

#define SLOG(...) {}
#define LOG(...) {}
#define NOTIFY(...) {}
#endif

#endif
