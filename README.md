# report-openssl-fips-read-error

This repository is to manage the files related to [this issue](https://github.com/openssl/openssl/issues/20657) on OpenSSL project.

## A test program

The reproducing program was downloaded from the <https://gist.github.com/levitte/7a27cebdb9537ff0a59641c9a5bed53d>.

### Reproducing steps

Create `key.pem` if you want to create newly.

```
$ openssl genrsa -out key.pem 4096
```

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
