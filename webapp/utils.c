#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <cgi.h>
#include <fcgi_stdio.h>
#include "utils.h"
#include "webapp.h"

char* read_file(char* filename)
{
    FILE* file = fopen(filename,"r");
    if(file == NULL)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    rewind(file);

    char* content = calloc(size + 1, 1);

    fread(content,1,size,file);

    return content;
}

int is_authenticated(){
	s_cgi *cgi;
	s_cookie *cookie;
	cgi = cgiInit();
	cookie = cgiGetCookie(cgi, "Authenticated");
	if(cookie != NULL){
		if(strcmp(cookie->value,"yes") == 0){
			return 1;
		}
	}
	return 0;
}

char *get_session_username(){
	s_cgi *cgi;
	s_cookie *cookie;
	cgi = cgiInit();
	cookie = cgiGetCookie(cgi, "Username");
	if(cookie != NULL){
		return strdup(cookie->value);
	}
	return NULL;
}

int authenticate(char *username, char *password) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL){
		return 0;
	}

        // use the real functions
        // https://www.youtube.com/watch?v=_jKylhJtPmI
	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL){
		mysql_close(con);
		return 0;
	}

	// prepared statement to select username
	char query[1024];
	sprintf(query, "SELECT Password FROM Users WHERE Username='%s';", username);

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	int result = 0;
	MYSQL_RES *users = mysql_store_result(con);
	if (users != NULL) {
		int num_users = mysql_num_fields(users);
		if(num_users > 0){
			MYSQL_ROW row = mysql_fetch_row(users);
			if(row != NULL){
				if(strcmp(password,row[0]) == 0){
					result = 1; // correct password
				} // else incorrect password
			} // shouldn't happen I don't think
		} // else user does not exist
		mysql_free_result(users);
	}

	mysql_close(con);
	return result;
}

char *get_field_for_username(char *username, char *field){
	MYSQL *con = mysql_init(NULL);

	if (con == NULL){
		return 0;
	}

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL){
		mysql_close(con);
		return 0;
	}

	char query[1024];
	sprintf(query, "SELECT %s FROM Users WHERE Username='%s';", field, username);

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	MYSQL_RES *users = mysql_store_result(con);
	if (users != NULL) {
		int num_users = mysql_num_fields(users);
		if(num_users > 0){
			MYSQL_ROW row = mysql_fetch_row(users);
			if(row != NULL){
				mysql_close(con);
				return row[0];
			} // shouldn't happen...I don't think
		} // else user does not exist
		mysql_free_result(users);
	}

	mysql_close(con);
	return NULL;
}

char *get_first_name(char *username){
	return get_field_for_username(username, "FirstName");
}

char *get_last_name(char *username){
	return get_field_for_username(username, "LastName");
}

// TODO: We should probably hash these passwords or something...
// Source: https://www.youtube.com/watch?v=8ZtInClXe1Q
int add_user(char *username, char *password, char *first_name, char *last_name, char *ssn, char is_admin) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL){
		return 0;
	}

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL){
		mysql_close(con);
		return 0;
	}

	// using a prepared statement for security
	char query[1024];
	sprintf(query, "INSERT INTO Users (Username, Password, FirstName, LastName, SSN, IsAdmin) VALUES ('%s','%s','%s','%s', '%s', '%c');", username, password, first_name, last_name, ssn, is_admin);

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	mysql_close(con);
	return 1;
}

void dump_tables(response *res) {
	MYSQL *con;
	if (!(con = mysql_init(NULL))) {
		return;
	}
	
	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
		mysql_close(con);
		return;
	}

	char query[] = "SELECT * FROM Users ORDER BY LastName, FirstName";

	if (mysql_query(con, query)) {
		mysql_close(con);
		return;
	}

	MYSQL_RES *result = mysql_store_result(con);
	unsigned int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
		unsigned long *lengths = mysql_fetch_lengths(result);
		unsigned int i;
		if(res != NULL) response_write(res, "<tr>");
		for (i = 0; i < num_fields; ++i ) {
			printf("%.*s,", lengths[i], row[i] ?: "NULL");
			char field[512];
			char *value = row[i] ? row[i] : "NULL";
			if(i==0){
				sprintf(field, "<td><a href=\"/webapp/timesheet?user=%s\">%s</a></td>", value, value);
			} else {
				sprintf(field, "<td>%s</td>", value);
			}
			
			if(res != NULL) response_write(res, field);
		}
		if(res != NULL) response_write(res, "</tr>");
	}

	mysql_close(con);
}

