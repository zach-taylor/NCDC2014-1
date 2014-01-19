#ifndef BACKTRACE_H
#define BACKTRACE_H

#include <fcgi_stdio.h>

int install_segfault_handler(void (*)());
void show_backtrace(FILE *);

#endif
