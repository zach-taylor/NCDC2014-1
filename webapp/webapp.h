#ifndef WEBAPP_H_
#define WEBAPP_H_

#include "response.h"

void write_template(response *res, char *template);
void write_page_template_header(response *res);
void write_page_template_footer(response *res);

#endif
