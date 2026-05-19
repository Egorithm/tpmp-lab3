#include <sqlite3.h>
#include <stdio.h>

int main(void) {
    printf("Версия SQLite библиотеки: %s\n", sqlite3_libversion()); 
    return 0;
}
