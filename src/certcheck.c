/* TLS Certificate Checker
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#include <stdio.h>
#include <stdlib.h>

#include "csv_helper.h"
#include "cert_helper.h"

int main(int argc, char const *argv[]) {

    cert_init();



    void *csv = csv_open("sample_input.csv", "r");
    while(1) {
        char* row[2];
        if (csv_get_row(csv, row, 2)!=2) break;

        cert_verify_cert(row[0], row[1]);

        csv_free_row(row);
    }

    csv_close(csv);


    return 0;
}
