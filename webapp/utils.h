#ifndef UTILS_H_
#define UTILS_H_

#define DBHOST "localhost"
#define DBNAME "webapp"
#define DBUSER "cdc"
#define DBPASS "cdc"

char* read_file(char *filename);
int authenticate(char *username, char *password);
int is_authenticated();
char *get_username();
int add_user(char *username, char *password, char *first_name, char *last_name);

#endif
