/* TLS Certificate Checker (CSV File Helper)
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELIMITER ","
#define ROW_MAX_LEN 1024

void remove_newline(char *str) {
    int pos = strlen(str) - 1;
    if (str[pos] == '\n') str[pos] = '\0';
}


// open a csv file
 void* csv_open(const char* filename, const char* mode) {
     FILE *fp;
     fp = fopen(filename, mode);
     return fp;
 }

// close a csv file
 void csv_close(void* csv) {
     fclose(csv);
 }

// write a row in the given csv file
void csv_write_row(void* csv, char** row, int num_fields){
    FILE *fp = csv;

    // move to a new line except first one
    if (ftell(fp)!=0) {
        fputs("\n", fp);
    }

    for(int i=0; i<num_fields; i++) {
        fputs(row[i], fp);

        if (i<num_fields-1) {
            fputs(DELIMITER, fp);
        }
    }
}

// read a row from the given csv file
int csv_get_row(void* csv, char** row, int max_fields) {
    FILE *fp = csv;
    if(feof(fp)) return 0;

    char* raw_line = malloc(ROW_MAX_LEN);

    if(fgets(raw_line, ROW_MAX_LEN, fp) == NULL) {
        free(raw_line);
        return -1;
    }
    remove_newline(raw_line);

    int n = 0;

    // separating row
    char *token = strtok(raw_line, DELIMITER);
    while(token && n<max_fields) {
        row[n++] = token;
        token = strtok(NULL, DELIMITER);
    }

    return n;
}

// free the memory allocated by csv_get_row
void csv_free_row(char** row) {
    free(row[0]);
}
