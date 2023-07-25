#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/provider.h>

/* BEGIN COPY */
/* The following is extracted from https://github.com/junaruga/openssl/raw/41bc792df2cf54660264bd6fc6368044f2877e99/ext/openssl/ossl_pkey.c and modified to get rid of Ruby specific stuff */
# include <openssl/decoder.h>

EVP_PKEY *
ossl_pkey_read_generic(BIO *bio, char *pass)
{
    OSSL_DECODER_CTX *dctx;
    EVP_PKEY *pkey = NULL;
    int pos = 0, pos2;

    dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "DER", NULL, NULL, 0, NULL, NULL);
    if (!dctx)
        goto out;
    if (OSSL_DECODER_CTX_set_pem_password_cb(dctx, PEM_def_callback, pass) != 1)
        goto out;

    /* First check DER */
    fprintf(stderr, "[DEBUG] Calling OSSL_DECODER_from_bio 1.\n");
    if (OSSL_DECODER_from_bio(dctx, bio) == 1)
        goto out;
    ERR_print_errors_fp(stderr);
    BIO_reset(bio);

    /* Then check PEM; multiple OSSL_DECODER_from_bio() calls may be needed */
    if (OSSL_DECODER_CTX_set_input_type(dctx, "PEM") != 1)
        goto out;
    /*
     * First check for private key formats. This is to keep compatibility with
     * ruby/openssl < 3.0 which decoded the following as a private key.
     *
     *     $ openssl ecparam -name prime256v1 -genkey -outform PEM
     *     -----BEGIN EC PARAMETERS-----
     *     BggqhkjOPQMBBw==
     *     -----END EC PARAMETERS-----
     *     -----BEGIN EC PRIVATE KEY-----
     *     MHcCAQEEIAG8ugBbA5MHkqnZ9ujQF93OyUfL9tk8sxqM5Wv5tKg5oAoGCCqGSM49
     *     AwEHoUQDQgAEVcjhJfkwqh5C7kGuhAf8XaAjVuG5ADwb5ayg/cJijCgs+GcXeedj
     *     86avKpGH84DXUlB23C/kPt+6fXYlitUmXQ==
     *     -----END EC PRIVATE KEY-----
     *
     * While the first PEM block is a proper encoding of ECParameters, thus
     * OSSL_DECODER_from_bio() would pick it up, ruby/openssl used to return
     * the latter instead. Existing applications expect this behavior.
     *
     * Note that normally, the input is supposed to contain a single decodable
     * PEM block only, so this special handling should not create a new problem.
     */
    OSSL_DECODER_CTX_set_selection(dctx, EVP_PKEY_KEYPAIR);
    while (1) {
        fprintf(stderr, "[DEBUG] Calling OSSL_DECODER_from_bio 2.\n");
        if (OSSL_DECODER_from_bio(dctx, bio) == 1)
            goto out;
        ERR_print_errors_fp(stderr);
        if (BIO_eof(bio))
            break;
        pos2 = BIO_tell(bio);
        if (pos2 < 0 || pos2 <= pos)
            break;
        ERR_clear_error();       /* Maybe print? */
        pos = pos2;
    }

    BIO_reset(bio);
    OSSL_DECODER_CTX_set_selection(dctx, 0);
    while (1) {
        fprintf(stderr, "[DEBUG] Calling OSSL_DECODER_from_bio 3.\n");
        if (OSSL_DECODER_from_bio(dctx, bio) == 1)
            goto out;
        ERR_print_errors_fp(stderr);
        if (BIO_eof(bio))
            break;
        pos2 = BIO_tell(bio);
        if (pos2 < 0 || pos2 <= pos)
            break;
        ERR_clear_error();       /* Maybe print? */
        pos = pos2;
    }

  out:
    OSSL_DECODER_CTX_free(dctx);
    return pkey;
}

/*
 *  call-seq:
 *     OpenSSL::PKey.read(string [, pwd ]) -> PKey
 *     OpenSSL::PKey.read(io [, pwd ]) -> PKey
 *
 * Reads a DER or PEM encoded string from _string_ or _io_ and returns an
 * instance of the appropriate PKey class.
 *
 * === Parameters
 * * _string_ is a DER- or PEM-encoded string containing an arbitrary private
 *   or public key.
 * * _io_ is an instance of IO containing a DER- or PEM-encoded
 *   arbitrary private or public key.
 * * _pwd_ is an optional password in case _string_ or _io_ is an encrypted
 *   PEM resource.
 */
static EVP_PKEY *
ossl_pkey_new_from_data(char *data, char *pass)
{
    EVP_PKEY *pkey;
    BIO *bio;

    bio = BIO_new_mem_buf(data, strlen(data));
    if (!bio) {
        ERR_raise_data(ERR_LIB_NONE, ERR_R_BIO_LIB,
                       "BIO_new_mem_buf() failed");
        return NULL;
    }
    fprintf(stderr, "[DEBUG] Calling ossl_pkey_read_generic from ossl_pkey_new_from_data.\n");
    pkey = ossl_pkey_read_generic(bio, pass);
    BIO_free(bio);
    if (!pkey)
        ERR_raise_data(ERR_LIB_NONE, 0, "Could not parse PKey");
    return pkey;
}

/* END COPY */

static int print_provider(OSSL_PROVIDER *prov, void *unused)
{
    printf("  %s\n", OSSL_PROVIDER_get0_name(prov));
    return 1;
}

int main(int argc, char *argv[])
{
    static char data[1024 * 1024];
    EVP_PKEY *pkey;

    printf("Loaded providers:\n");
    OSSL_PROVIDER_do_all(NULL, &print_provider, NULL);

    /* Slurp */
    FILE *f;
    size_t data_size;

    if ((f = fopen(argv[1], "r")) == NULL
        || (data_size = fread(data, 1, sizeof(data), f)) == 0) {
        if (f)
            fclose(f);
        ERR_raise_data(ERR_LIB_NONE, 0, "Could not read PKey");
    } else {
        fclose(f);
        data[data_size] = '\0';
        pkey = ossl_pkey_new_from_data(data, "");
        if (pkey) {
            printf("Got a pkey! %p\n", (void *)pkey);
            printf("It's held by the provider %s\n",
                   OSSL_PROVIDER_get0_name(EVP_PKEY_get0_provider(pkey)));
        }
        EVP_PKEY_free(pkey);
    }

    ERR_print_errors_fp(stderr);
}
