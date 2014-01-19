#define _GNU_SOURCE
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <dlfcn.h>
#include <time.h>
#include <fcgi_stdio.h>
#include "backtrace.h"

static unw_context_t resume_ctx;
static void (*segfault_callback)() = 0;

void addr2line(void *p, char *c) { 
	Dl_info dli = { 0 };
	(void)dladdr(p, &dli);
	ptrdiff_t offset = p - (dli.dli_saddr ?: dli.dli_fbase);
	if (dli.dli_fname) {
		sprintf(c, "%s(%s+0x%lx)[%p]",
			dli.dli_fname, dli.dli_sname ?: "", offset, p);
	} else { 
		sprintf(c, "[%p]", p);
	}
}

void show_backtrace(FILE *f) { 
	unw_cursor_t cursor;
	unw_context_t uc;
	unw_word_t ip, sp;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);

	char buffer[1024];
	unw_step(&cursor);
	for(;;) {
		int end = (unw_step(&cursor) > 0);
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		
		if (!end || !ip) 
			break; 

		addr2line((void*)ip, buffer);
		fprintf(f, "%s\n", buffer);
	}
}

void on_segfault(int signal, siginfo_t *si, void *arg) { 
	if (segfault_callback) {
		segfault_callback();
	}

	unw_cursor_t resume_cursor;
	unw_init_local(&resume_cursor, &resume_ctx);
	unw_resume(&resume_cursor);
}

int install_segfault_handler(void (*cb)()) { 
    segfault_callback = cb;

    struct sigaction sa = { 0 };
    sa.sa_sigaction = &on_segfault;
    sa.sa_flags = SA_SIGINFO;

    int rc;
	if (rc = sigaction(SIGSEGV, &sa, NULL))
		return rc;

	unw_getcontext(&resume_ctx);
	return 0;
}
