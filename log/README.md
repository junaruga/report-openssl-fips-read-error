The log files were created by the command below.

## FIPS mode

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

## Non-FIPS mode

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-debug/lib/ \
  ltrace -ttt -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& non_fips_ltrace_ttt.log
```

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-debug/lib/ \
  ltrace -ttt -S -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& non_fips_ltrace_ttt_S.log
```

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-debug/lib/ \
  strace -f \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& non_fips_strace_f.log
```
