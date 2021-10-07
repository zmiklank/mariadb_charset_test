#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define USER "dbtester"
#define PASS "dbtester"
#define DATABASE "dbtest"
#define TABLE "tabletest"

static void show_mysql_error(MYSQL *mysql)
{
	printf("Error(%d) [%s] \"%s\"", mysql_errno(mysql), mysql_sqlstate(mysql),
		mysql_error(mysql));
	exit(1);
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

void print_table(MYSQL* mysql)
{
	if (!mysql_query(mysql,"SELECT * FROM "TABLE))
		mysql_error(mysql);
	MYSQL_RES *result = mysql_store_result(mysql);
	printf("table "TABLE" is filled as follows:\n");
	print_result(result);
}

int insert_to_table(MYSQL* mysql, char* id, char* name, char* surname)
{
	char command[100];
	sprintf(command, "INSERT INTO "TABLE" VALUES ('%s', '%s', '%s')", id, name, surname);
	if (mysql_query(mysql,command))
		show_mysql_error(mysql);
}

void test_write_db()
{
	MYSQL* mysql;
	mysql = mysql_init(NULL);
	if (mysql_real_connect(mysql, NULL, USER, PASS, DATABASE, 0, NULL, 0) == NULL)
		show_mysql_error(mysql);
	insert_to_table(mysql, "1", "Čeněk Řehák", "z Žabovřesek");
	insert_to_table(mysql, "2", "Cenek Rehak", "z Zabovresek");
	mysql_close(mysql);
}

void test_read_db()
{
	MYSQL* mysql;
	mysql = mysql_init(NULL);
	if (mysql_real_connect(mysql, NULL, USER, PASS, DATABASE, 0, NULL, 0) == NULL)
		show_mysql_error(mysql);
	print_table(mysql);
	mysql_close(mysql);
}
int main(int argc, char *argv[])
{
	printf("----------------------------------------------------------------\n");
	test_write_db();
	test_read_db();
	printf("----------------------------------------------------------------\n");
}
