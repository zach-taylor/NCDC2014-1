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

//#include <cgi.h>
#include "raphters.h"
#include "utils.h"
#include "fcgi_stdio.h"
#include <mysql/mysql.h>

void write_template(response *res, char *template) {
	const char *content = read_file(template);
	response_write(res, content);
}

void write_page_template_header(response *res){
	write_template(res, "./templates/header.html.template");
}

void write_page_template_footer(response *res){
	write_template(res, "./templates/footer.html.template");
}

// login page
START_HANDLER (login_page_handler, GET, "/login", res, 0, matches) {
	response_add_header(res, "content-type", "text/html");
	write_template(res, "./templates/login.html.template");
} END_HANDLER

// login action
START_HANDLER (login_action_handler, POST, "/login", res, 0, matches) {
	char* post_data = get_post_string();

	char* username = get_param(post_data, "username");
	if(username == NULL){
		username = "";
	}

	char* password = get_param(post_data, "password");
	if(password == NULL){
		password = "";
	}

	response_add_header(res, "content-type", "text/html");
	write_page_template_header(res);

	if(authenticate(username,password)){
		response_write(res, "Success!");
	} else {
		response_write(res, "Username or password is incorrect.");
	}

	write_page_template_footer(res);

} END_HANDLER

// add user page
START_HANDLER (create_user_page_handler, GET, "/user/new", res, 0, matches) {
	response_add_header(res, "content-type", "text/html");
	write_page_template_header(res);
	const char *create_form = read_file("./templates/new.html.template");
	response_write(res, create_form);
	write_page_template_footer(res);
} END_HANDLER

// add user action
START_HANDLER (create_user_action_handler, POST, "/user/create", res, 0, matches) {
	char* post_data = get_post_string();

	char* username = get_param(post_data, "username");
	if(username == NULL){
		username = "";
	}

	char* password = get_param(post_data, "password");
	if(password == NULL){
		password = "";
	}

	char* first_name = get_param(post_data, "first_name");
	if(first_name == NULL){
		first_name = "";
	}

	char* last_name = get_param(post_data, "last_name");
	if(last_name == NULL){
		last_name = "";
	}

	response_add_header(res, "content-type", "text/html");
	write_page_template_header(res);

	if(add_user(username, password, first_name, last_name)){
		response_write(res, "Success!");
	} else {
		response_write(res, "Could not create user.");
	}

	write_page_template_footer(res);

} END_HANDLER

// default route
START_HANDLER (default_handler, GET, "", res, 0, matches) {
	// redirect to login page
	response_add_header(res, "Location", "/webapp/login");
} END_HANDLER

int main() {
    add_handler(login_page_handler);
    add_handler(login_action_handler);
    add_handler(create_user_page_handler);
    add_handler(create_user_action_handler);
    add_handler(default_handler);
    serve_forever();
    return 0;
}
