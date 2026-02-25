#include "MiniDB.h"
/*La funziona alloca un nuovo database, inizializzandone i parametri */
Database createDatabase(const char* name){
    Database db; 
    strncpy(db.name, name, sizeof(db.name) - 1);
    db.name[sizeof(db.name) - 1] = '\0'; //copia fino al penultimo carattere la stringa e all'ultimo carattere inserisce il terminatore di stringa
    db.tables = NULL;
    db.table_number = 0;
    return db;
}

/*inizializza una nuova tabella e i suoi parametri*/
Database createTable(Database db, const char* name){
    db.table_number++; 
    db.tables = realloc(db.tables, db.table_number * sizeof(Table)); 
    if(db.tables == NULL){
        fprintf(stderr, "Errore di allocazione della memoria\n");
        exit(EXIT_FAILURE);
    }
    Table new_table; 
    strncpy(new_table.name, name, sizeof(new_table.name)-1);
    new_table.name[sizeof(new_table.name) - 1] = '\0';
    new_table.pages = NULL; 
    new_table.page_number = 0;
    db.tables[db.table_number -1] = new_table;
    return db;
}

/*Inserisce un nuovo record in una tabella, controlla se c'è da creare una nuova pagina (nessuna pagina inizializzata o ultima pagina piena )
e successivamente inserisce in numero_record + 1 il nuovo record e poi incrementa il conteggio di record*/
void insertRow(Table* table, Row row){
    if(table->page_number ==0 || table->pages[table->page_number - 1].row_count == PAGE_SIZE / sizeof(Row)){
        table->page_number ++; // se non abbiamo pagine o se l'ultima è piena abbiamo bisogno di allocare una nuova pagina
        table->pages = realloc(table->pages, table->page_number * sizeof(Page)); // riallochiamo la memoria per le pagine
        Page new_page; 
        new_page.row_count = 0;
        table->pages[table->page_number - 1] = new_page; // aggiungiamo la nuova pagina alla tabella
    }
    Page* current_page = &table->pages[table->page_number -1]; // prendo l'ultima pagina
    current_page->rows[current_page->row_count] = row; //inserisco la riga nella pagina
    current_page->row_count++; //incremento il numero di righe nella pagina
}

void printTable(Table table){
    printf("table name: %s", table.name);
    printf("page number: %d", table.page_number);
    for(int i=0; i<table.page_number; i++){
        printf("page: %d \nrow count: %d \n",i+1, table.pages[i].row_count);
        for(int j=0; j<table.pages[i].row_count; j++){
            Row row = table.pages[i].rows[j];
            printf("| %d | %s | %s | %s | %d | \n", row.id, row.name, row.surname, row.email, row.age);
        }
    }
}

void dropDB(Database *db){
    for(int i=0; i<db->table_number; i++){
        dropTable(&db->tables[i]);
    }
    free(db->tables);
    db->table_number=0; 
    db->tables = NULL; 
    printf("Database eliminato\n");
}

void dropTable(Table* table){
    free(table->pages);
    table->page_number = 0;
    table->pages = NULL;
    printf("Tabella eliminata\n");
}

void deleteRow(Table *table, int id){
    for(int p = 0; p < table->page_number; p++){
        Page* page = &table->pages[p];
        for(int j = 0; j < page->row_count; j++){
            if(page->rows[j].id == id){
                for(int k = j; k < page->row_count - 1; k++){
                    page->rows[k] = page->rows[k+1];
                }
                page->row_count--;
                printf("Riga eliminata\n");
                return;
            }
        }
    }
    printf("ID non valido\n");
}

void describeDB(Database *db){
    for(int i=0; i<db->table_number; i++){
        Table *t = &db->tables[i];
        printf("Table number: %d \n", i+1);
        printf("Table name: %s \n", t->name);
        printf("Page number: %d \n", t->page_number);
        int total_rows = 0;
        for(int p = 0; p < t->page_number; p++){
            total_rows += t->pages[p].row_count;
        }
        printf("Row count: %d \n", total_rows);
        printf("Columns: id, name, surname, email, age \n");
    }
}

bool isTableEmpty(Table *table){
    for(int i=0; i<table->page_number; i++){
        if(table->pages[i].row_count > 0){
            return false;
        }
    }
    return true;
}

bool isPageFull(Page *page){
    return page->row_count = PAGE_SIZE / sizeof(Row);
}

void updateRow(Table* table, int id, Row *new_row){
    for(int p = 0; p < table->page_number; p++){
        Page *page = &table->pages[p];
        for(int j=0; j<page->row_count; j++){
            if(page->rows[j].id == id){
                page->rows[j] = *new_row;
                printf("Riga aggiornata\n");
                return; //fermiamo la ricerca
            }
        }
    }
}

schema* create_schema(const char* schema_name){
    schema* s = (schema*)malloc(sizeof(schema));
    strncpy(s->name, schema_name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
    s->columns = NULL;
    s->column_number = 0;
    return s;
}

schema* add_column(schema *s, const char *name, fieldType type){
    s->column_number++;
    s->columns = realloc(s->columns, s->column_number * sizeof(column));
    if(s->columns == NULL){
        fprintf(stderr, "Impossibile creare la colonna\n");
        exit(EXIT_FAILURE);
    }
    column *new_column = &s->columns + s->column_number - 1;
    strncpy(new_column->name, name, sizeof(new_column->name) - 1);
    new_column->name[sizeof(new_column->name) - 1] = '\0';
    new_column->field=type;
    return s;
}

void print_schema(schema s){
    printf('schema name: %s\n', s.name);
    printf("column number: %d\n", s.column_number);
    for(int i=0; i<s.column_number; i++){
        printf("column name: %s\n", s.columns[i].name);
        printf("field type: %d\n", s.columns[i].field);
    }
}

void delete_column(schema *s, const char *name){
    for(int i=0; i<s->column_number; i++){
        if(strcmp(s->columns[i].name, name) == 0){
            for(int j=i; j<s->column_number - 1; j++){
                s->columns[j] = s->columns[j+1];
            }
            s->column_number--;
            s->columns = realloc(s->columns, s->column_number * sizeof(column));
            if(s->columns == NULL && s->column_number > 0){
                fprintf(stderr, "Impossibile eliminare la colonna\n");
                exit(EXIT_FAILURE);
            }
            printf("Colonna eliminata\n");
            return;
        }
    }
}


int main(){
    //TEST 1: Database e Tabelle
    printf("===== TEST 1: Database e Tabelle =====\n\n");
    Database db = createDatabase("my_database");
    db = createTable(db, "users");
    Row row1 = {1, "Alice", "Rossi", "AliRossi@gmail.com", 22};
    Row row2 = {2, "Bob", "Bianchi", "BianchiBob@outlook.it", 31};
    insertRow(&db.tables[0], row1);
    insertRow(&db.tables[0], row2);
    printTable(db.tables[0]);
    describeDB(&db);
    deleteRow(&db.tables[0], 1);
    printTable(db.tables[0]);
    Row* new_row = (Row*)malloc(sizeof(Row));
    new_row->id = 2; 
    new_row->age = 32;
    strncpy(new_row->name, "Bob", sizeof(new_row->name)-1);
    new_row->name[sizeof(new_row->name) - 1] = '\0';
    strncpy(new_row->surname, "Bianchi", sizeof(new_row->surname)-1);
    new_row->surname[sizeof(new_row->surname) - 1] = '\0';
    strncpy(new_row->email, "BobBianchi@gmail.com", sizeof(new_row->email)-1);
    new_row->email[sizeof(new_row->email) - 1] = '\0';
    updateRow(&db.tables[0], 2, new_row);
    printTable(db.tables[0]);
    
    //TEST 2: Schema e Colonne
    printf("\n===== TEST 2: Schema e Colonne =====\n\n");
    schema* s = create_schema("user_schema");
    s = add_column(s, "id", INT);
    s = add_column(s, "name", STRING);
    s = add_column(s, "email", STRING);
    print_schema(*s);
    printf("\nEliminando colonna 'email'...\n");
    delete_column(s, "email");
    print_schema(*s);
    free(s->columns);
    free(s);
    free(new_row);
    return 0;
}