#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "utils.h"

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

int authenticate(char *username, char *password) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL){
		return 0;
	}

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, 0) == NULL){
		mysql_close(con);
		return 0;
	}

	// INSERT INTO Users VALUES ('test','test','Test1','Test2');
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
			} // else user does not exist
		}
		mysql_free_result(users);
	}

	mysql_close(con);
	return result;
}

int add_user(char *username, char *password, char *first_name, char *last_name) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL){
		return 0;
	}

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASS, DBNAME, 0, NULL, 0) == NULL){
		mysql_close(con);
		return 0;
	}

	char query[1024];
	sprintf(query, "INSERT INTO Users VALUES ('%s','%s','%s','%s');", username, password, first_name, last_name);

	if (mysql_query(con, query)) {
		mysql_close(con);
		return 0;
	}

	mysql_close(con);
	return 1;
}
