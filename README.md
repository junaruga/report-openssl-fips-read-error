# report-openssl-fips-read-error

This repository is to manage the files related to [this issue](https://github.com/openssl/openssl/issues/20657) on OpenSSL project.

The reproducing program was downloaded from the <https://gist.github.com/levitte/7a27cebdb9537ff0a59641c9a5bed53d>.

## Reproducing steps

Create `key.pem` if you want to create newly.

```
$ openssl genrsa -out key.pem 4096
```

Compile. The following example shows the compile with debugging options.

```
$ gcc \
  -I /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/include \
  -L /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib \
  -Wl,-rpath,/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib \
  -O0 -g3 -ggdb3 -gdwarf-5 \
  -o reproducer reproducer.c -lcrypto
```

Check the linked libraries.

```
$ ldd ./reproducer
	linux-vdso.so.1 (0x00007fff6fdb1000)
	libcrypto.so.3 => /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/libcrypto.so.3 (0x00007fcda4000000)
	libc.so.6 => /lib64/libc.so.6 (0x00007fcda3e22000)
	/lib64/ld-linux-x86-64.so.2 (0x00007fcda466b000)
```

### Non-FIPS case

This is ok.

```
$ OPENSSL_CONF_INCLUDE=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/ssl \
  OPENSSL_MODULES=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/ossl-modules \
  ./reproducer key.pem
[DEBUG] Loaded providers:
  default
[DEBUG] FIPS mode enabled: 0
[DEBUG] data_size: 3272
[DEBUG] Calling OSSL_DECODER_from_bio.
[DEBUG] Calling OSSL_DECODER_CTX_set_selection with PEM and EVP_PKEY_KEYPAIR.
[DEBUG] OSSL_DECODER_from_bio PEM success.
[DEBUG] Got a pkey! 0x1e08cf0
[DEBUG] It's held by the provider default
[DEBUG] ossl_membio2str buf->data:
-----BEGIN PUBLIC KEY-----
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA1zjzeiqsSJW0QK7MCi6+
UYWu5uLHrPucubpS1Y2hhfUDQAkeJeXwb/iv6arSptXRbQK5GcVHvn21t5KuraQe
INhnBc1+O95WRPKnFjR3yUAXpoYJ8QuuIUTKAO+bh10qVapZyhDDj5/Gr3YsKgUz
Vyr4LHckAYmLkJhQjcC1jqrXUykLbrxJh/vSJtW+3yOOYQnOb1BFUsdcA0SHjfOC
Gn6n/D02Jgy6czhdnI7vVRpQ0YsY5AmDW7PI3JlCOFCNy2c1l3+BgyU7c8PFscZJ
gRijZVWMujhEOaW15PEt8Dg3MG2gcb5zxNR57rj+sPbFAQtcZarDp0/1nxymEwtO
1bAwZfVuz9s2a52denBV39zo+YAZ3dKWXxF0Dn2tJQfzpcmIJmuY4/sp5tCkVkoe
okchoDf7BZjX+p60JwvWUmLvAk5zgbLS60jpHjT4hy6qlVnGGkItQGv8nmaU4Iw6
zAB6Pr+YabF7qAz0NnjDQn42DDLKfCthQ93B8KKo2vfTMQpiMd2DaJBXORaBIv1W
C1vbZwj4BIroZzYCMbENOBotXB2DR17YrTRe9TgAToHlGfEpnBFmO6K0OQcAnAAh
KxeWZ8vQ5KiY/2w5wjpKQpNZFZYw3dTtcmOrRP7pmaVObkZ6FaD4WHOn6lKscn2L
lZ/HcBXdBPtGJ/W/LhsqZs0CAwEAAQ==
-----END PUBLIC KEY-----
```

### FIPS case

This is not ok.

```
$ OPENSSL_CONF=$(pwd)/openssl_fips.cnf \
  OPENSSL_CONF_INCLUDE=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/ssl \
  OPENSSL_MODULES=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/ossl-modules \
  ./reproducer key.pem
[DEBUG] Loaded providers:
  fips
  base
[DEBUG] FIPS mode enabled: 1
[DEBUG] data_size: 3272
[DEBUG] Calling OSSL_DECODER_from_bio.
[DEBUG] Calling OSSL_DECODER_CTX_set_selection with PEM and EVP_PKEY_KEYPAIR.
[DEBUG] BIO_eof break.
[DEBUG] Failed to get the pkey.
[DEBUG] Could not parse a PKey
```

Debug with gdb. It seems setting the `LD_LIBRARY_PATH` is still necessary to see the source properly by the gdb command "l".

```
$ OPENSSL_CONF=$(pwd)/openssl_fips.cnf \
  OPENSSL_CONF_INCLUDE=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/ssl \
  OPENSSL_MODULES=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/ossl-modules \
  gdb --args ./reproducer key.pem
(gdb) set environment LD_LIBRARY_PATH /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib
```

## 20657.c (original reproducing program)

Run the commands below.

```
$ gcc \
  -I /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/include \
  -L /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib \
  -o 20657 20657.c -lcrypto

$ OPENSSL_CONF=$(pwd)/openssl_fips.cnf \
  OPENSSL_CONF_INCLUDE=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/ssl \
  OPENSSL_MODULES=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/ossl-modules \
  LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib \
  ./20657 key.pem
Loaded providers:
  fips
  base
4097042C6A7F0000:error:00800000:unknown library:main:unknown library:20657.c:153:Could not read PKey
```

## License

The license belongs to the author of the linked page above, @levitte - Richard Levitte.
