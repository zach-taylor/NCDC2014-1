#define _GNU_SOURCE
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>
#include <time.h>
#include "backtrace.h"

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
	char fname[64];
	sprintf(fname, "backtrace.%ld.log", time(0));
    FILE *bt = fopen(fname, "w+") ?: stderr;
    fprintf(stderr, "Caught segfault at addr %p\n", si->si_addr);

    show_backtrace(bt);

    exit(-SIGSEGV);
}

int install_segfault_handler() { 
    struct sigaction sa = { 0 };
    sa.sa_sigaction = &on_segfault;
    sa.sa_flags = SA_SIGINFO;

    return sigaction(SIGSEGV, &sa, NULL);
}
