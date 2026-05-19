#include <sqlite3.h>
#include <stdio.h>

int main(void) {
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("test_zet.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    char *sql = "DROP TABLE IF EXISTS Friends;"
                "BEGIN TRANSACTION;" 
                "CREATE TABLE Friends(Id INTEGER PRIMARY KEY, Name TEXT);" 
                "INSERT INTO Friends(Name) VALUES ('Tom');" 
                "INSERT INTO Friend(Name) VALUES ('Robert');"
                "COMMIT;";
                           
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("Как и ожидалось, транзакция вызвала ошибку: %s\n", err_msg);
        printf("Все изменения отменены автоматически (all-or-nothing).\n");
        sqlite3_free(err_msg);        
    }
        
    sqlite3_close(db);
    return 0;
}
