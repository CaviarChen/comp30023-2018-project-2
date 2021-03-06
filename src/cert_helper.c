/* TLS Certificate Checker (Cert Helper)
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/asn1.h>

#include "debug_setting.h"
#include "cert_helper.h"

#define PUBLICKEY_TYPE EVP_PKEY_RSA
#define PUBLICKEY_LEN 2048

#define BUFFER_LEN 1024

bool cert_verify_dates(X509 *cert, BIO *outbio);
bool cert_verify_domain(X509 *cert, BIO *outbio, const char* domain);
bool cert_verify_publickey(X509 *cert);
bool cert_verify_constraints(X509 *cert);
bool cert_verify_keyusage(X509 *cert);

bool check_domain(const char* domain, const char* target_domain);
char* astr_to_str(ASN1_STRING* a_str);


// initialise openSSL
void cert_init() {
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
}

// clean up openSSL
void cert_free() {
    ERR_free_strings();
    EVP_cleanup();
    ERR_remove_state(0);
}

// check if a cert is valid
bool cert_verify_cert(const char* filename, const char* domain) {

    #if DEBUG
    printf("\n ------ %s -------\n", filename);
    printf(" ------ %s -------\n", domain);
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

    int flag = false;

    if (cert_verify_dates(cert, outbio) &&
        cert_verify_domain(cert, outbio, domain) &&
        cert_verify_publickey(cert) &&
        cert_verify_constraints(cert) &&
        cert_verify_keyusage(cert)) {

        flag = true;
    }



    X509_free(cert);
    BIO_free_all(cert_bio);

    #if DEBUG
    BIO_free_all(outbio);

    printf("Overall: %s\n", flag?"True":"False");
    #endif

    return flag;
}

// check not before and not after
bool cert_verify_dates(X509 *cert, BIO *outbio) {

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

    if (day > 0 || sec > 0) return false;

    // not after
    time = X509_get_notAfter(cert);

    ASN1_TIME_diff(&day, &sec, NULL, time);

    if (day < 0 || sec < 0) return false;

    return true;
}

// check domain
bool cert_verify_domain(X509 *cert, BIO *outbio, const char* domain) {

    X509_NAME *cert_name = X509_get_subject_name(cert);

    // check CN
    char cert_cn[256] = {};
    X509_NAME_get_text_by_NID(cert_name, NID_commonName, cert_cn, 256);

    #if DEBUG
    printf("CommonName: %s\n", cert_cn);
    #endif

    if(check_domain(domain, cert_cn)) return true;

    // check SAN
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
                return true;
            }

            free(buf);
        }
    }

    return false;
}

// check public key len and type
bool cert_verify_publickey(X509 *cert) {
    EVP_PKEY *publickey = X509_get_pubkey(cert);
    // check type
    if((publickey)&&(publickey->type==PUBLICKEY_TYPE)) {

        #if DEBUG
        printf("key len: %d \n", BN_num_bits(publickey->pkey.rsa->n));
        #endif
        // check len
        if (BN_num_bits(publickey->pkey.rsa->n) >= PUBLICKEY_LEN) {
            return true;
        }

        return false;
    }

    #if DEBUG
    printf("Wrong key type \n");
    #endif

    return false;
}

// check constraints
bool cert_verify_constraints(X509 *cert) {
    BASIC_CONSTRAINTS *bc;
    bc = X509_get_ext_d2i(cert, NID_basic_constraints, NULL, NULL);

    // have basic_constraints and CA: False
    if((bc)&&(!bc->ca)) {

        #if DEBUG
        printf("CA: FALSE : pass\n");
        #endif

        return true;
    }

    #if DEBUG
    printf("CA: FALSE : fail\n");
    #endif

    return false;

}

//check key usage
bool cert_verify_keyusage(X509 *cert) {
    STACK_OF(ASN1_OBJECT) *objs = NULL;
    objs = X509_get_ext_d2i(cert, NID_ext_key_usage, NULL, NULL);

    if (objs) {
        int count = sk_ASN1_OBJECT_num(objs);
        // for each ext key usage
        for(int i=0; i<count; i++) {
            ASN1_OBJECT *obj = sk_ASN1_OBJECT_value(objs, i);

            if (OBJ_obj2nid(obj)==NID_server_auth) {
                // find server auth
                #if DEBUG
                printf("Key usage: pass\n");
                #endif

                return true;
            }
        }
    }

    #if DEBUG
    printf("Key usage: fail\n");
    #endif

    return false;
}

// convert a ASN1_STRING to C String
char* astr_to_str(ASN1_STRING* a_str) {
    char *buf = (char *)malloc((a_str->length + 1) * sizeof(char));

    memcpy(buf, a_str->data, a_str->length);
    buf[a_str->length] = '\0';

    return buf;
}

// check if a domain matches target domain
// support wildcard except partial wildcard
bool check_domain(const char* domain, const char* target_domain) {

    // too short to be a domain
    if (strlen(target_domain)<2) return false;

    if(target_domain[0]=='*') {
        // wildcard
        if (target_domain[1]!='.') {
            // wildcard must start with '*.'
            return false;
        }

        int ld = strlen(domain) - 1;
        int ltd = strlen(target_domain) - 1;

        // checking backwards
        while(ld>=0) {
            if (tolower(domain[ld--]) != tolower(target_domain[ltd--]))
                    return false;

            // reachs '*' in target_domain
            if (ltd==0) {
                // make sure there is no '.' in the part that matched by '*'
                for (int i=0; i<=ld; i++) {
                    if (domain[i]=='.') return false;
                }
                return true;
            };
        }
        return false;
    }

    // normal
    return (strcasecmp(domain, target_domain)==0);
}
