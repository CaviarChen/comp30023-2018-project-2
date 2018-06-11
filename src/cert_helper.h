/* TLS Certificate Checker (Cert Helper)
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#ifndef CERT_HELPER_H
#define CERT_HELPER_H

#include <stdbool.h>

void cert_init();
void cert_free();

bool cert_verify_cert(const char* filename, const char* domain);

#endif
