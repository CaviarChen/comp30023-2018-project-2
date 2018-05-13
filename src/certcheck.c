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

    cert_verify_cert("./cert-file2.pem", "example.com");

    return 0;
}
