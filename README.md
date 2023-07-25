# report-openssl-fips-read-error

This repository is to manage the files related to [this issue](https://github.com/openssl/openssl/issues/20657) on OpenSSL project.

The reproducing program was downloaded from the <https://gist.github.com/levitte/7a27cebdb9537ff0a59641c9a5bed53d>.

## Reproducing steps

Create `key.pem` if you want to create newly.

```
$ openssl genrsa -out key.pem 4096
```

Compile

```
$ gcc \
  -I /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/include \
  -L /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib \
  -o reproducer reproducer.c -lcrypto
```

### Non-FIPS case

This is ok.

```
$ OPENSSL_CONF_INCLUDE=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/ssl \
  OPENSSL_MODULES=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/ossl-modules \
  LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib \
  ./reproducer key.pem
[DEBUG] Loaded providers:
  default
[DEBUG] FIPS mode enabled: 0
[DEBUG] data_size: 3272
[DEBUG] OSSL_DECODER_from_bio PEM failed.
[DEBUG] Got a pkey! 0x10ddcf0
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
  LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib \
  ./reproducer key.pem
[DEBUG] Loaded providers:
  fips
  base
[DEBUG] FIPS mode enabled: 1
[DEBUG] data_size: 3272
4067B53FF37F0000:error:1E08010C:DECODER routines:OSSL_DECODER_from_bio:unsupported:crypto/encode_decode/decoder_lib.c:102:No supported data to decode. Input type: DER
[DEBUG] Failed to get the pkey.
[DEBUG] Could not parse a PKey
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
