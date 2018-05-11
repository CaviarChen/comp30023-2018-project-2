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

    return 0;
}
