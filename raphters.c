/*
    Copyright (C) 2011 Raphters authors,
    
    This file is part of Raphters.
    
    Raphters is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Raphters is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <fcgi_stdio.h>
#include <time.h>
#include "raphters.h"
#include "backtrace.h"

void (*user_segfault)() = 0;

void segfault() {
#if defined(DEBUG) || 1

    response *r = response_empty();
    response_add_header(r, "content-type", "text/plain");
    response_send(r);
    show_backtrace(stdout);
    show_backtrace(stderr);

    if (user_segfault) { 
        user_segfault();
    }

#endif
}

void serve_forever() {
	srand(time(0));
	(void)install_segfault_handler(&segfault);
	int uid = (int) geteuid();
	if(uid = 0){
		// never run webapp as root, its a security risk
		uid = 1;
	}
	// set uid to non-root user
	setuid(uid);
	seteuid(uid);
    init_handlers();
    while(FCGI_Accept() >= 0) {
        dispatch();
    }
    cleanup_handlers();
}
