/* TLS Certificate Checker (Cert Helper)
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/asn1.h>

#include "debug_setting.h"

#define FALSE 0
#define TRUE 1

int cert_verify_dates(X509 *cert, BIO *outbio);

void cert_init() {
    //initialise openSSL
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
}

int cert_verify_cert(const char* filename, const char* domain) {

    //create BIO object to read certificate
    BIO *cert_bio = BIO_new(BIO_s_file());

    //Read certificate into BIO
    X509 *cert = NULL;
    if (!(BIO_read_filename(cert_bio, filename))) {
        fprintf(stderr, "Error in reading cert BIO filename");
        exit(EXIT_FAILURE);
    }
    if (!(cert = PEM_read_bio_X509(cert_bio, NULL, 0, NULL))) {
        BIO_free_all(cert_bio);
        fprintf(stderr, "Error in loading certificate");
        exit(EXIT_FAILURE);
    }

    // BIO object for output
    BIO *outbio = NULL;
    #if DEBUG
    outbio  = BIO_new(BIO_s_file());
    outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);
    #endif

    int flag = FALSE;

    if(cert_verify_dates(cert, outbio)) {

        flag = TRUE;
    }



    X509_free(cert);
    BIO_free_all(cert_bio);

    #if DEBUG
    BIO_free_all(outbio);
    #endif

    return flag;
}



int cert_verify_dates(X509 *cert, BIO *outbio) {

    //debug info
    #if DEBUG
    printf("Not before: ");
    ASN1_TIME_print(outbio, X509_get_notBefore(cert));
    printf("\n");
    printf("Not after: ");
    ASN1_TIME_print(outbio, X509_get_notAfter(cert));
    printf("\n");
    #endif


    const ASN1_TIME *time = NULL;
    int day, sec;

    // not before

    time = X509_get_notBefore(cert);

    ASN1_TIME_diff(&day, &sec, NULL, time);

    if (day > 0 || sec > 0) return FALSE;

    // not after
    time = X509_get_notAfter(cert);

    ASN1_TIME_diff(&day, &sec, NULL, time);

    if (day < 0 || sec < 0) return FALSE;

    return TRUE;
}
