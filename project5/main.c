#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// 1. Вставка с ТРАНЗАКЦИЕЙ
void insert_transaction(sqlite3 *db) {
    char last_name[50], first_name[50];
    int class_num;
    char *err_msg = 0;

    printf("Фамилия: "); scanf("%49s", last_name);
    printf("Имя: "); scanf("%49s", first_name);
    printf("Класс: "); scanf("%d", &class_num);

    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &err_msg);

    const char *sql = "INSERT INTO schoolboy (фамилия, имя, класс) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, first_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, class_num);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("Ошибка! Откат транзакции.\n");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, &err_msg);
    } else {
        printf("Успешно добавлено (Transaction)!\n");
        sqlite3_exec(db, "COMMIT;", 0, 0, &err_msg);
    }
    sqlite3_finalize(stmt);
}

// 2. Вставка AUTOCOMMIT (без явного BEGIN/COMMIT)
void insert_autocommit(sqlite3 *db) {
    char last_name[50], first_name[50];
    int class_num;

    printf("Фамилия: "); scanf("%49s", last_name);
    printf("Имя: "); scanf("%49s", first_name);
    printf("Класс: "); scanf("%d", &class_num);

    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO schoolboy (фамилия, имя, класс) VALUES ('%s', '%s', %d);", last_name, first_name, class_num);
    
    char *err_msg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        printf("Ошибка вставки: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Успешно добавлено (Autocommit)!\n");
    }
}

// 3. Выборка по классу (параметризованный запрос)
void select_by_class(sqlite3 *db) {
    int target_class;
    printf("Введите номер класса: ");
    scanf("%d", &target_class);

    const char *sql = "SELECT id, фамилия, имя FROM schoolboy WHERE класс = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, target_class);

    printf("\n--- Результат ---\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("ID: %d | %s %s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1), sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);
}

// 4. Удаление ученика (DELETE)
void delete_schoolboy(sqlite3 *db) {
    int id;
    printf("Введите ID ученика для удаления: ");
    scanf("%d", &id);

    const char *sql = "DELETE FROM schoolboy WHERE id = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Ученик удален!\n");
    } else {
        printf("Ошибка удаления.\n");
    }
    sqlite3_finalize(stmt);
}

// 5. Выгрузка фото из БД (BLOB)
void extract_photo(sqlite3 *db) {
    int student_id;
    printf("Введите ID школьника для извлечения фото: ");
    scanf("%d", &student_id);

    const char *sql = "SELECT photo FROM schoolboy WHERE id = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, student_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int bytes = sqlite3_column_bytes(stmt, 0);
        if (bytes > 0) {
            const void *blob_data = sqlite3_column_blob(stmt, 0);
            FILE *fp = fopen("exported_photo.jpg", "wb");
            if (fp) {
                fwrite(blob_data, 1, bytes, fp);
                fclose(fp);
                printf("Фото %d байт сохранено как exported_photo.jpg\n", bytes);
            }
        } else {
            printf("У ученика нет фото (NULL).\n");
        }
    } else {
        printf("Ученик не найден.\n");
    }
    sqlite3_finalize(stmt);
}

int main() {
    sqlite3 *db;
    // Открываем базу на директорию выше (где мы ее создали на Шаге 2)
    if (sqlite3_open("../school.db", &db)) {
        printf("Ошибка открытия БД!\n");
        return 1;
    }

    int choice;
    do {
        printf("\n=== БД 'Школьник' (Вар 12) ===\n");
        printf("1. Добавить (Transaction)\n");
        printf("2. Добавить (Autocommit)\n");
        printf("3. Найти по классу\n");
        printf("4. Удалить по ID\n");
        printf("5. Выгрузить фото\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1: insert_transaction(db); break;
            case 2: insert_autocommit(db); break;
            case 3: select_by_class(db); break;
            case 4: delete_schoolboy(db); break;
            case 5: extract_photo(db); break;
        }
    } while (choice != 0);

    sqlite3_close(db);
    return 0;
}
