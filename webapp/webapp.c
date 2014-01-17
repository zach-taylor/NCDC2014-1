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

#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <time.h>
#include "fcgi_stdio.h"
#include "raphters.h"
#include "webapp.h"
#include "utils.h"

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

void write_logout_link(response *res, char *username){
	if(username != NULL){
		char *first_name = get_first_name(username);
		char *last_name = get_last_name(username);
		if(first_name != NULL && last_name != NULL){
			response_write(res, "<p>Welcome: ");
			response_write(res, first_name);
			response_write(res, " ");
			response_write(res, last_name);
			response_write(res, " [<a href=\"/webapp/logout\">Logout</a>]</p>");
		}
	}
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

	if(authenticate(username,password)){
		char username_cookie[1024];
		sprintf(username_cookie, "Username=%s; path=/; max-age=604800;", username);
		response_add_header(res, "Set-Cookie", username_cookie);
		response_add_header(res, "Set-Cookie", "Authenticated=yes; path=/; max-age=604800;");
		response_add_header(res, "Location", "/webapp/timesheet");
	} else {
		response_add_header(res, "content-type", "text/html");
		write_page_template_header(res);
		response_write(res, "Username or password is incorrect.");
		write_page_template_footer(res);
	}

} END_HANDLER

// logout action
START_HANDLER (logout_action_handler, GET, "/logout", res, 0, matches) {
	// expire session
	response_add_header(res, "Set-Cookie", "Authenticated=no; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT;");
	response_add_header(res, "Set-Cookie", "Username=; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT;");
	response_add_header(res, "Location", "/webapp/login");
} END_HANDLER

// add user page
START_HANDLER (create_user_page_handler, GET, "/user/new", res, 0, matches) {
	response_add_header(res, "content-type", "text/html");
	write_page_template_header(res);
	const char *create_form = read_file("./templates/new_user.html.template");
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

	char* ssn = get_param(post_data, "ssn");
	if(ssn == NULL){
		ssn = "";
	}

	char set_admin = 'N';
	char* administrator = get_param(post_data, "administrator");
	if(administrator != NULL){
		if(strcmp(administrator, "yes") == 0){
			set_admin = 'Y';
		}
	}

	response_add_header(res, "content-type", "text/html");
	write_page_template_header(res);

	if(add_user(username, password, first_name, last_name, ssn, set_admin)){
		response_write(res, "Success! Created ");
		response_write(res, username);
	} else {
		response_write(res, "Could not create user.");
	}

	write_page_template_footer(res);

} END_HANDLER

// timesheet page
START_HANDLER (timesheet_page_handler, GET, "/timesheet", res, 0, matches) {
	char *username = get_session_username();
	if(username != NULL && is_authenticated()){
		response_add_header(res, "content-type", "text/html");
		write_page_template_header(res);
		write_logout_link(res, username);

		time_t curtime = time(NULL); // get the current time.
		struct tm *local_time = localtime(&curtime); // convert to local time representation

		response_write(res, "<br />");
		response_write(res, "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" class=\"table table-striped table-bordered\" id=\"timesheet\">");
		response_write(res, "<tr><th>Date</th><th>Hours</th><th>Actions</th></tr>");

		response_write(res, "<tr><td>");
		char buffer[256];
		strftime (buffer, 256, "%A, %B %d", local_time);
		response_write(res, buffer);
		response_write(res, "</td><td></td><td></td></tr>");

		response_write(res, "</table>");
		write_page_template_footer(res);
	} else {
		response_add_header(res, "Location", "/webapp/login"); // redirect to login page
	}
} END_HANDLER

// default route
START_HANDLER (default_handler, GET, "", res, 0, matches) {
	response_add_header(res, "Location", "/webapp/login"); // redirect to login page
} END_HANDLER

int main() {
	HANDLE(timesheet_page_handler);
	HANDLE(login_page_handler);
	HANDLE(login_action_handler);
	HANDLE(logout_action_handler);
	HANDLE(create_user_page_handler);
	HANDLE(create_user_action_handler);
	HANDLE(default_handler);
    serve_forever();
    return 0;
}
