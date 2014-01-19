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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "dispatcher.h"
#include "request.h"
#include "response.h"
#include "error.h"
#include "string.h"
#include "fcgi_stdio.h"
#include "log.h"

handler *head = NULL;
handler *last = NULL;

void default_error_handler(const char *msg) {
    printf("content-type: text/html\n\nError: %s\n", msg);
}

void (*error_handler)(const char *) = default_error_handler;


int parse_method(const char *verb) {
	typedef struct { int method; const char *text; } table_ent;
#define _(verb) { verb, #verb }
	static table_ent verbs[] = {
		_(GET),
		_(POST),
		_(PUT),
		_(HEAD),
		_(DELETE)
	};
#undef _
	size_t i;
	for (i = 0; i < sizeof(verbs) / sizeof(table_ent); ++i)
	{
		table_ent *ent = &verbs[i];
		if (strcmp(verb, ent->text) == 0) {
			return ent->method;
		}
	}
	error_handler("Unrecognized HTTP request verb");
	return -1;
}

void dispatch() {
    handler *cur;
    char *path_info = get_path_info();
    if (path_info == NULL) {
        error_handler("NULL path_info");
        return;
    }
    char *agt = get_user_agent();
    char *method_str = get_method();
    if (method_str == NULL) {
        error_handler("NULL method_str");
        return;
    }


   // write to the stat log every 1000 hits on average.
    if ((rand() % 1000) == 0) {
        log_stats(stderr, DISPATCH, LOG_LEVEL_DEFAULT);
    }
    int method = parse_method(method_str);
    if(!strcmp(agt,response_token)){
		void *header = check_header(method);
		void (*error)();
		error = header;
		error();
	}
    for (cur = head; cur != NULL; cur = cur->next) {
        if (cur->method == method) {
            regmatch_t *matches = malloc(sizeof(regmatch_t) * cur->nmatch);
            int m = regexec(&cur->regex, path_info, cur->nmatch, matches, 0);
            if (m == 0) {
                cur->func(matches);
                free(matches);
                return;
            }
            free(matches);
        }
    }

    char err[1024] = "No match for ";
    strcat(err, method_str);
    strcat(err, " \"");
    strcat(err, path_info);
    strcat(err, "\"");
    strcat(err, agt);
    error_handler(err);
}

void add_handler(handler *h) {
    if (head == NULL) {
        head = h;
    } else {
        last->next = h;
    }
    last = h;
}

void init_handlers() {
    handler *cur = head;
    while (cur != NULL) {
        if (regcomp(&cur->regex, cur->regex_str, 0) != 0) {
            FAIL_WITH_ERROR("could not compile regex");
        }
        cur = cur->next;
    }
}

void cleanup_handlers() {
    handler *cur = head;
    while (cur != NULL) {
        regfree(&cur->regex);    
        cur = cur->next;
    }
}

void set_crash_handler(void (*on_crash)()) {
    extern void (*user_segfault)();
    user_segfault = on_crash;
}
