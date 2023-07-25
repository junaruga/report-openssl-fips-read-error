#include <stdlib.h>
#include <string.h>

#include <openssl/decoder.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/provider.h>

static int print_provider(OSSL_PROVIDER *prov, void *unused)
{
    printf("  %s\n", OSSL_PROVIDER_get0_name(prov));
    return 1;
}

EVP_PKEY *
ossl_pkey_read_generic(BIO *bio, char *pass)
{
    OSSL_DECODER_CTX *dctx;
    EVP_PKEY *pkey = NULL;
    int pos = 0, pos2;

    dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "DER", NULL, NULL, 0, NULL, NULL);
    if (!dctx) {
        fprintf(stderr, "[DEBUG] OSSL_DECODER_CTX_new_for_pkey DER failed.\n");
        goto out;
    }
    if (OSSL_DECODER_CTX_set_pem_password_cb(dctx, PEM_def_callback, pass) != 1) {
        fprintf(stderr, "[DEBUG] OSSL_DECODER_CTX_set_pem_password_cb DER failed.\n");
        goto out;
    }

    /* First check DER */
    fprintf(stderr, "[DEBUG] Calling OSSL_DECODER_from_bio.\n");
    if (OSSL_DECODER_from_bio(dctx, bio) == 1) {
        fprintf(stderr, "[DEBUG] OSSL_DECODER_from_bio DER failed.\n");
        goto out;
    }
    BIO_reset(bio);

    /* Then check PEM; multiple OSSL_DECODER_from_bio() calls may be needed */
    if (OSSL_DECODER_CTX_set_input_type(dctx, "PEM") != 1) {
        fprintf(stderr, "[DEBUG] OSSL_DECODER_CTX_set_input_type PEM failed.\n");
        goto out;
    }

    fprintf(stderr, "[DEBUG] Calling OSSL_DECODER_CTX_set_selection with PEM and EVP_PKEY_KEYPAIR.\n");
    OSSL_DECODER_CTX_set_selection(dctx, EVP_PKEY_KEYPAIR);

    while (1) {
        if (OSSL_DECODER_from_bio(dctx, bio) == 1) {
            fprintf(stderr, "[DEBUG] OSSL_DECODER_from_bio PEM success.\n");
            goto out;
        }
        if (BIO_eof(bio)) {
            fprintf(stderr, "[DEBUG] BIO_eof break.\n");
            break;
        }
        pos2 = BIO_tell(bio);
        if (pos2 < 0 || pos2 <= pos) {
            fprintf(stderr, "[DEBUG] BIO_tell break.\n");
            break;
        }
        ERR_clear_error();
        pos = pos2;
    }

    if (!pkey) {
        fprintf(stderr, "[DEBUG] Failed to get the pkey.\n");
    }
out:
    OSSL_DECODER_CTX_free(dctx);
    return pkey;
}

int main(int argc, char *argv[])
{
    int status = EXIT_SUCCESS;
    int fips_enabled = 0;
    /* Input file */
    FILE *f = NULL;
    static char data[1024 * 1024];
    size_t data_size = 0;
    EVP_PKEY *pkey = NULL;
    BIO *bio = NULL;
    BUF_MEM *buf = NULL;

    /* Print debugging info for FIPS */
    printf("[DEBUG] Loaded providers:\n");
    OSSL_PROVIDER_do_all(NULL, &print_provider, NULL);
    fips_enabled = EVP_default_properties_is_fips_enabled(NULL);
    printf("[DEBUG] FIPS mode enabled: %d\n", fips_enabled);

    /* Check if the input file from the first argument is valid. */
    if ((f = fopen(argv[1], "r")) == NULL
        || (data_size = fread(data, 1, sizeof(data), f)) == 0) {
        fprintf(stderr, "[DEBUG] Could not read PKey: %s\n", argv[1]);
        status = EXIT_FAILURE;
        goto end;
    }
    printf("[DEBUG] data_size: %d\n", data_size);

    fclose(f);
    f = NULL;

    data[data_size] = '\0';

    /* Get Pkey from data. */
    bio = BIO_new_mem_buf(data, strlen(data));
    if (!bio) {
        fprintf(stderr, "[DEBUG] BIO_new_mem_buf() failed\n");
        status = EXIT_FAILURE;
        goto end;
    }
    pkey = ossl_pkey_read_generic(bio, "");
    BIO_free(bio);
    bio = NULL;
    if (!pkey) {
        fprintf(stderr, "[DEBUG] Could not parse a PKey\n");
        status = EXIT_FAILURE;
        goto end;
    }

    printf("[DEBUG] Got a pkey! %p\n", (void *)pkey);
    printf("[DEBUG] It's held by the provider %s\n",
           OSSL_PROVIDER_get0_name(EVP_PKEY_get0_provider(pkey)));

    /* Export Pkey. */
    bio = BIO_new(BIO_s_mem());
    if (!bio) {
        fprintf(stderr, "BIO_new\n");
        status = EXIT_FAILURE;
        goto end;
    }
    if (!PEM_write_bio_PUBKEY(bio, pkey)) {
        fprintf(stderr, "PEM_write_bio_PUBKEY\n");
        status = EXIT_FAILURE;
        goto end;
    }
    BIO_get_mem_ptr(bio, &buf);
    printf("[DEBUG] ossl_membio2str buf->data:\n%s\n", buf->data);
end:
    if (f) {
        fclose(f);
    }
    if (bio) {
        BIO_free(bio);
    }
    if (pkey) {
        EVP_PKEY_free(pkey);
    }
    return status;
}
