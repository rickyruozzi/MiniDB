#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PAGE_SIZE 4096

typedef struct Row {
    int id;
    char name[32];
    char email[64];
    int age;
} Row;

typedef struct Page {
    Row rows[PAGE_SIZE / sizeof(Row)];
    int row_count;
} Page;

typedef struct Table {
    char name[32];
    Page* pages; 
    int page_number;
} Table;

typedef struct Database{
    char name[32];
    Table* tables; 
    int table_number;
} Database;

Database createDatabase(const char* name);
Database createTable(Database db, const char* name);
void insertRow(Table* table, Row row);
void printTable(Table table);
