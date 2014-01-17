#ifndef UTILS_H_
#define UTILS_H_

#define DBHOST "localhost"
#define DBNAME "webapp"
#define DBUSER "cdc"
#define DBPASS "cdc"

char* read_file(char *filename);
int authenticate(char *username, char *password);
int is_authenticated();
char *get_session_username();
char *get_first_name(char *username);
char *get_last_name(char *username);
int add_user(char *username, char *password, char *first_name, char *last_name, char *ssn, char is_admin);

#endif
