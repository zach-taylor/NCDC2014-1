#include <syslog.h>

void logs(char *message) {
	setlogmask (LOG_UPTO (LOG_DEBUG));

	openlog ("webapp", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	syslog (LOG_INFO, "%s", message);

	closelog ();
}
