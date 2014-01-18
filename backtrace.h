#ifndef BACKTRACE_H
#define BACKTRACE_H

#include <stdio.h>

int install_segfault_handler(void);
void show_backtrace(FILE *);

#endif
