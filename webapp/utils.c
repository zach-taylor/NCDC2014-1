#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <cgi.h>
#include <fcgi_stdio.h>
#include "utils.h"
#include "webapp.h"
#include "constants.h"
#include "logger.h"

s_cgi *cgi;

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

char *randstring(size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {
	    int n;
            for (n = 0;n < length;n++) {
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

int is_authenticated(){
	s_cookie *cookie;
	if(cgi==NULL)
		cgi = cgiInit();
	cookie = cgiGetCookie(cgi, "sid");


	MYSQL *con = mysql_init(NULL);
	logs("level=INFO, action=is_authenticated, status=started");
	
	if (con == NULL){
		logs("level=FATAL, action=is_authenticated, status=failed, message=\"database connection is null\"");
		return 0;
	}

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL){
    mysql_close(con);
		logs("level=FATAL, action=is_authenticated, status=failed, message=\"database connection failed\"");
    return 0;
  }

	char query[1024];
	if(cookie != NULL){
		sprintf(query, "SELECT * FROM Sessions WHERE SessionID='%s' AND IsActive=1;", cgiEscape(cookie->value));
	} else {
		logs("level=ERROR, action=is_authenticated, status=failed, message=\"cookie reference is null\"");
		return 0;
	}
	
	if(mysql_real_query(con, query, strlen(query))) {
		mysql_close(con);
		logs("level=FATAL, action=is_authenticated, status=failed, message=\"database query failed\"");
		return 0;
	}

	int result = 0;
        
  MYSQL_RES *sessions = mysql_store_result(con);

	if(sessions != NULL) {
		int num_sessions = mysql_num_rows(sessions);
		if(num_sessions > 0) {
      result = 1;
			logs("level=INFO, action=is_authenticated, status=succeeded");
		} else {
			logs("level=INFO, action=is_authenticated, status=failed");
    }
		mysql_free_result(sessions);
  }
  mysql_close(con);
  return result;
	
	/*
	if(cookie != NULL){
		if(strcmp(cookie->value,"yes") == 0){
			return 1;
		}
	}
	return 0;
	*/
}

char *get_session_username(){
	s_cookie *cookie;
	if(cgi==NULL)
		cgi = cgiInit();
	cookie = cgiGetCookie(cgi, "sid");
	if(cookie != NULL){
		//return strdup(cookie->value);
		return get_field_for_session(cookie->value, "Username");
	}
	return NULL;
}

// just checks if the username and password are correct
int authenticate(char *username, char *password) {
	MYSQL *con = mysql_init(NULL);
	
	// log auth attempt
	char logstring[1024];
	sprintf(logstring, "level=INFO, action=authenticate, user=\"%s\"", username);
	logs(logstring);
	
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
	//sprintf(query, "SELECT Password FROM Users WHERE Username='%s';", username);
	
	sprintf(query, "SELECT Username FROM Users WHERE Username='%s' AND Password=SHA(CONCAT('%s','%s')) AND IsActive=1;", username, password, SALT);
	
	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	int result = 0;
	
	MYSQL_RES *users = mysql_store_result(con);
	/*
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
	*/

	if(users != NULL) {
		int num_users = mysql_num_fields(users);
		if(num_users > 0) {
			return 1;
		}
		mysql_free_result(users);
	}
	mysql_close(con);
	return result;
}

char *get_field_for_session(char *sessionid, char *field){
	MYSQL *con = mysql_init(NULL);

	if (con == NULL){
		return 0;
	}

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL){
		mysql_close(con);
		return 0;
	}

	char query[1024];
	sprintf(query, "SELECT %s FROM Sessions WHERE SessionID='%s';", field, sessionid);

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	MYSQL_RES *sessions = mysql_store_result(con);
	if (sessions != NULL) {
		int num_sessions = mysql_num_fields(sessions);
		if(num_sessions > 0){
			MYSQL_ROW row = mysql_fetch_row(sessions);
			if(row != NULL){
				mysql_close(con);
				return row[0];
			} // shouldn't happen...I don't think
		} // else user does not exist
		mysql_free_result(sessions);
	}

	mysql_close(con);
	return NULL;
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

int is_admin(char *username){
	char *isadmin = get_field_for_username(username, "IsAdmin");
	char logstring[1024];
	sprintf(logstring, "level=INFO, action=is_admin, username=\"%s\", value=%s", username, isadmin);
}

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
	sprintf(query, "INSERT INTO Users (Username, Password, FirstName, LastName, SSN, IsAdmin, IsActive) VALUES ('%s', SHA(CONCAT('%s','%s')),'%s', '%s', '%s', '%c', '1');", username, password, SALT, first_name, last_name, ssn, is_admin);

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	mysql_close(con);
	return 1;
}

// Source: https://www.youtube.com/watch?v=8ZtInClXe1Q
int add_session(char *username, char *sessionid) {
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
	sprintf(query, "INSERT INTO Sessions (Username, SessionID, LastUse, IsActive) VALUES ('%s','%s', NOW(), '1');", username, sessionid);

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	mysql_close(con);
	return 1;
}

int disable_session(){
	s_cookie *cookie;
	if(cgi==NULL)
		cgi = cgiInit();
	cookie = cgiGetCookie(cgi, "sid");
	if(cookie != NULL){
		logs("cookie not null");
		char query[1024];
		sprintf(query, "UPDATE Sessions SET IsActive=0 WHERE SessionID='%s';", cookie->value);
		MYSQL *con = mysql_init(NULL);
		if (con == NULL) {
			return 0;
		}
	
		if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
			mysql_close(con);
			return 0;
		}


		if (mysql_query(con, query)) {
			mysql_close(con);
			return 0;
		}

		mysql_close(con);
	}
	logs("cookie null");
	return 1;
}

// day should be in format yyyy-mm-dd
int add_entry(char *username, char *day, char *minutes_worked){
	char query[1024];
	sprintf(query, "INSERT INTO Entries (Username, Day, MinutesWorked, ApprovedBy) VALUES ('%s', '%s', '%s', 'Not Approved');", username, day, minutes_worked);
	
	MYSQL *con;
	if (!(con = mysql_init(NULL))) {
		return;
	}
	
	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
		mysql_close(con);
		return;
	}

	if (mysql_query(con, query)) {
		mysql_close(con);
		return;
	}

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	mysql_close(con);
	return 1;
}

int approve_entry(char *username, char *day){
	char query[1024];
	sprintf(query, "UPDATE Entries SET ApprovedBy='Approved' WHERE Username='%s' AND Day='%s';", username, day);
	
	MYSQL *con;
	if (!(con = mysql_init(NULL))) {
		return;
	}
	
	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
		mysql_close(con);
		return;
	}

	if (mysql_query(con, query)) {
		mysql_close(con);
		return;
	}

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	mysql_close(con);
	return 1;
}

void render_entries_json(response *res, char *username, char *start_date, char *end_date) {
	response_write(res, "{ \"entries\": [");
	char *prepend = "";	
	if(username != NULL && start_date != NULL && end_date != NULL){
		MYSQL *con;
		if (!(con = mysql_init(NULL))) {
			return;
		}

		if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
			mysql_close(con);
			return;
		}

		char query[512]; 
		sprintf(query, "SELECT * FROM Entries WHERE Username = '%s' AND Day BETWEEN '%s' AND '%s';", username, start_date, end_date);

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
			if(num_fields == 4){
				response_write(res, prepend);
				prepend = ",";
				char result[1024];
				char *day_value = row[1] ? row[1] : "NULL";
				char *minutes_worked_value = row[2] ? row[2] : "NULL";
				char *approved_by_value = row[3] ? row[3] : "NULL";
				sprintf(result, "{ \"day\":\"%s\", \"minutes\":\"%s\",\"approver\":\"%s\" }", day_value, minutes_worked_value, approved_by_value);
				response_write(res, result);
			}
		}

		mysql_close(con);
		
	}
	response_write(res, "]}");
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

