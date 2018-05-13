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
int cert_verify_domain(X509 *cert, BIO *outbio, const char* domain);

int check_domain(const char* domain, const char* target_domain);
char* astr_to_str(ASN1_STRING* a_str);

void cert_init() {
    //initialise openSSL
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
}

int cert_verify_cert(const char* filename, const char* domain) {

    #if DEBUG
    printf("\n ------ %s -------\n", filename);
    #endif

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

    // if(cert_verify_dates(cert, outbio)) {
    //     if(cert_verify_domain(cert, outbio, domain)) {
    //         flag = TRUE;
    //     }
    // }

    cert_verify_dates(cert, outbio);
    cert_verify_domain(cert, outbio, domain);



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

int cert_verify_domain(X509 *cert, BIO *outbio, const char* domain) {

    X509_NAME *cert_name = X509_get_subject_name(cert);

    char cert_cn[256] = {};
    X509_NAME_get_text_by_NID(cert_name, NID_commonName, cert_cn, 256);

    #if DEBUG
    printf("CommonName: %s\n", cert_cn);
    #endif

    if(check_domain(domain, cert_cn)) return TRUE;

    // SAN
    GENERAL_NAMES *g_names;
    g_names = X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);

    if (g_names!=NULL) {
        int count = sk_GENERAL_NAME_num(g_names);
        for(int i=0; i<count; i++) {
            GENERAL_NAME *g_name = sk_GENERAL_NAME_value(g_names, i);
            if(g_name->type != GEN_DNS) continue;

            ASN1_IA5STRING * a_str = g_name->d.dNSName;
            char* buf = astr_to_str(a_str);

            #if DEBUG
            printf("SAN: %s\n", buf);
            #endif

            if(check_domain(domain, buf)) {
                free(buf);
                return TRUE;
            }

            free(buf);

        }

    }

    return FALSE;
}

char* astr_to_str(ASN1_STRING* a_str) {
    char *buf = (char *)malloc((a_str->length + 1) * sizeof(char));

    memcpy(buf, a_str->data, a_str->length);
    buf[a_str->length] = '\0';

    return buf;
}

int check_domain(const char* domain, const char* target_domain) {
    return FALSE;
}
