/* Simple HTTP1.0 Server (CSV File Helper)
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#include <stdio.h>
#include <stdlib.h>

#define DELIMITER ","

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

// write a row in a given csv file
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
