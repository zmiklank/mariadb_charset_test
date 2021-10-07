#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define USER "dbtester"
#define PASS "dbtester"
#define DATABASE "dbtest"
#define TABLE "tabletest"

// the test is not to be run within the automated tmt suite

static void show_mysql_error(MYSQL *mysql)
{
	printf("Error(%d) [%s] \"%s\"", mysql_errno(mysql), mysql_sqlstate(mysql),
		mysql_error(mysql));
}

void print_result(MYSQL_RES *result)
{
	int num_fields = mysql_num_fields(result);

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))){
		for(int i = 0; i < num_fields; i++){
			printf("%s ", row[i] ? row[i] : "NULL");
		}
		printf("\n");
	}
}

void print_charset_settings(MYSQL * mysql)
{
	MY_CHARSET_INFO charset;
	mysql_get_character_set_info(mysql, &charset);
	const char* charset_conn =  mysql_character_set_name(mysql);
	printf("connection character set is: %s, num: %d\n", charset_conn, charset.number);
	mysql_query(mysql,
	"SELECT default_character_set_name FROM information_schema.SCHEMATA WHERE schema_name = '"DATABASE"';");
	MYSQL_RES *result = mysql_store_result(mysql);
	printf("charset of db "DATABASE" is ");
	print_result(result);
}
void print_table(MYSQL* mysql)
{
	if (!mysql_query(mysql,"SELECT * FROM "TABLE))
		mysql_error(mysql);
	MYSQL_RES *result = mysql_store_result(mysql);
	printf("table "TABLE" is filled as follows:\n");
	print_result(result);
}

void change_database_charset_and_collation(MYSQL *mysql, char* charset, char* collation)
{
	char command[100];
	if (collation == NULL) {
		sprintf(command,
			"ALTER DATABASE "DATABASE" CHARACTER SET %s", charset);
	}else{
		sprintf(command,
			"ALTER DATABASE "DATABASE" CHARACTER SET %s COLLATE %s", charset, collation);
	}
    if (mysql_query(mysql, command))
        show_mysql_error(mysql);
}

// can be called after mysql_init()
void change_connection_charset(MYSQL* mysql, char* charset)
{
	if (mysql_optionsv(mysql, MYSQL_SET_CHARSET_NAME, (void *)charset))
		show_mysql_error(mysql);
}

void create_table(MYSQL* mysql)
{
	if (mysql_query(mysql, "DROP TABLE IF EXISTS "TABLE))
		show_mysql_error(mysql);

	char command[100];
	sprintf(command, "CREATE TABLE "TABLE"(id CHAR(30), forename CHAR(30), surname CHAR(30))");
	if (mysql_query(mysql, command))
		show_mysql_error(mysql);
}

int insert_to_table(MYSQL* mysql, char* id, char* name, char* surname)
{
	char command[100];
	sprintf(command, "INSERT INTO "TABLE" VALUES ('%s', '%s', '%s')", id, name, surname);
	if (mysql_query(mysql,command))
		show_mysql_error(mysql);
}

void test_write_db(char* connection_charset, char* db_charset)
{
	MYSQL* mysql;
	mysql = mysql_init(NULL);
	printf("\nwrite combination: connection charset: %s db charset: %s\n\n", connection_charset, db_charset);
	change_connection_charset(mysql, connection_charset);
	if (mysql_real_connect(mysql, NULL, USER, PASS, DATABASE, 0, NULL, 0) == NULL)
		show_mysql_error(mysql);
	change_database_charset_and_collation(mysql, db_charset, NULL);
	create_table(mysql);
	//print_charset_settings(mysql);
	char testname_1[20];
	sprintf(testname_1,"%s-%s_1", connection_charset, db_charset);
	char testname_2[20];
	sprintf(testname_2,"%s-%s_2", connection_charset, db_charset);
	insert_to_table(mysql, testname_1, "Čeněk Řehák", "z Žabovřesek");
	insert_to_table(mysql, testname_2, "Cenek Rehak", "z Zabovresek");
	mysql_close(mysql);
}

void test_read_db(char* connection_charset)
{
	MYSQL* mysql;
	mysql = mysql_init(NULL);
	printf("\nread  connection charset: %s\n\n", connection_charset);
	change_connection_charset(mysql, connection_charset);
	if (mysql_real_connect(mysql, NULL, USER, PASS, DATABASE, 0, NULL, 0) == NULL)
		show_mysql_error(mysql);
	// print_charset_settings(mysql);
	print_table(mysql);
	mysql_close(mysql);
}
int main(int argc, char *argv[])
{
	printf("----------------------------------------------------------------\n");
	test_write_db("latin1","utf8mb4");
	test_read_db("utf8mb4");
	test_read_db("latin1");
	printf("----------------------------------------------------------------\n");
	test_write_db("utf8mb4","latin1");
	test_read_db("latin1");
	test_read_db("utf8mb4");
	printf("----------------------------------------------------------------\n");
	printf("----------------------------------------------------------------\n");
	test_write_db("latin1","latin1");
	test_read_db("utf8mb4");
	test_read_db("latin1");
	printf("----------------------------------------------------------------\n");
	printf("----------------------------------------------------------------\n");
	test_write_db("utf8mb4","utf8mb4");
	test_read_db("latin1");
	test_read_db("utf8mb4");
	printf("----------------------------------------------------------------\n");
}
