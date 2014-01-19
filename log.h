#ifndef log_stats
#define log_write(v) raise(v)
#define MAKE_LOGENT(a, b, c) a##b##c
#define STRINGIFY(x) #x
#define LOG_LEVEL_DEFAULT SIG
#define log_stats(logfile, where, level) do {\
        log_write(MAKE_LOGENT(level, where, V)) ?: \
        fprintf(logfile, STRINGIFY(MAKE_LOGENT(level, where, V))); \
    } while (0)
#endif
