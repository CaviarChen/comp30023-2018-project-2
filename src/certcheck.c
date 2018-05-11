/* TLS Certificate Checker
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#include <stdio.h>
#include <stdlib.h>

#include "csv_helper.h"

int main(int argc, char const *argv[]) {


    char* data1[] = {"aaaa"};
    char* data2[] = {"aaaa", "bbbb"};
    char* data3[] = {"aaaa", "bbbb", "cccc"};

    // test writing csv
    void* csv = csv_open("test.csv", "w");
    csv_write_row(csv, data1, 1);
    csv_write_row(csv, data2, 2);
    csv_write_row(csv, data3, 3);
    csv_close(csv);


    // test reading csv
    csv = csv_open("test.csv", "r");
    while(1) {
        printf("----\n");
        char* data[6];
        int n = csv_get_row(csv, data, 6);
        if (n<1) break;
        for(int i=0; i<n; i++) {
            printf("%s\n", data[i]);
        }
        csv_free_row(data);
    }
    csv_close(csv);

    return 0;
}
