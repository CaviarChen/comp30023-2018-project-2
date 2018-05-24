/* TLS Certificate Checker
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#include <stdio.h>
#include <stdlib.h>

#include "csv_helper.h"
#include "cert_helper.h"

#define RETURN_TRUE "1"
#define RETURN_FLASE "0"

int main(int argc, char const *argv[]) {

    cert_init();


    void *csv = csv_open(argv[1], "r");
    void *csv_out = csv_open("output.csv", "w");

    while(1) {
        char* row[3] = {};
        if (csv_get_row(csv, row, 2)!=2) break;

        int code = cert_verify_cert(row[0], row[1]);

        row[2] = (code)? RETURN_TRUE: RETURN_FLASE;

        csv_write_row(csv_out, row, 3);

        csv_free_row(row);
    }

    csv_close(csv);
    csv_close(csv_out);

    cert_free();

    return 0;
}
