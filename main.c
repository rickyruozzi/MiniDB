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
            printf("| %d | %s | %s | %d | \n", row.id, row.name, row.email, row.age);
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
    for(int i=0; i<table->page_number; i++){
        free(table->pages[i].rows);
    }
    free(table->pages);
    table->page_number=0;
    table->pages = NULL;
    printf("Tabella eliminata\n");
}

void deleteRow(Table *table, int id){
    int num_page = id * sizeof(Row) / PAGE_SIZE; 
    if(num_page >= table->page_number){
        printf("ID non valido\n");
        return;
    }
    Page* page = &table->pages[num_page];
    for(int i=0; i<page->row_count; i++){
        for(int j=0; j < page->row_count; j++){
            if(page->rows[j].id == id){
                for(int k=j; k<page->row_count - 1; k++){
                    page->rows[k] = page->rows[k+1];
                }
                page->row_count--;
                printf("Riga eliminata\n");
                return;
            }
        }
    }
}

int main(){
    Database db = createDatabase("my_database");
    db = createTable(db, "users");
    Row row1 = {1, "Alice", "Rossi", "AliRossi@gmail.com", 22};
    Row row2 = {2, "Bob", "Bianchi", "BianchiBob@outlook.it", 31};
    insertRow(&db.tables[0], row1);
    insertRow(&db.tables[0], row2);
    printTable(db.tables[0]);
    return 0;
}