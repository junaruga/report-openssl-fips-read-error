# report-openssl-fips-read-error

This repository is to manage the files related to [this issue](https://github.com/openssl/openssl/issues/20657) on OpenSSL project.

The log files were created by the command below.

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-fips-debug/lib/ \
  OPENSSL_CONF=/home/jaruga/.local/openssl-3.0.8-fips-debug/ssl/openssl_fips.cnf \
  ltrace -ttt -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& fips_ltrace_ttt.log
```

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-fips-debug/lib/ \
  OPENSSL_CONF=/home/jaruga/.local/openssl-3.0.8-fips-debug/ssl/openssl_fips.cnf \
  ltrace -ttt -S -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& fips_ltrace_ttt_S.log
```

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-fips-debug/lib/ \
  OPENSSL_CONF=/home/jaruga/.local/openssl-3.0.8-fips-debug/ssl/openssl_fips.cnf \
  strace -f \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& fips_strace_f.log
```
