The log files were created by the command below.

## OpenSSL tracing feature

Log by enabling `#define TEST_ENABLE_TRACE 1` in the `reproducer.c`.

### FIPS case

```
$ OPENSSL_CONF=$(pwd)/openssl_fips.cnf \
  OPENSSL_CONF_INCLUDE=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/ssl \
  OPENSSL_MODULES=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/ossl-modules \
  ./reproducer key.pem >& trace_fips.log
```


### Non-FIPS case

```
$ OPENSSL_CONF_INCLUDE=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/ssl \
  OPENSSL_MODULES=/home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322/lib/ossl-modules \
  ./reproducer key.pem >& trace_non_fips.log
```

## ltrace

### FIPS case

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-fips-debug/lib/ \
  OPENSSL_CONF=/home/jaruga/.local/openssl-3.0.8-fips-debug/ssl/openssl_fips.cnf \
  ltrace -ttt -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& ltrace_ttt_fips.log
```

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-fips-debug/lib/ \
  OPENSSL_CONF=/home/jaruga/.local/openssl-3.0.8-fips-debug/ssl/openssl_fips.cnf \
  ltrace -ttt -S -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& ltrace_ttt_S_fips.log
```

### Non-FIPS case

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-debug/lib/ \
  ltrace -ttt -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& ltrace_ttt_non_fips.log
```

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-debug/lib/ \
  ltrace -ttt -S -f -l openssl.so -l libssl.so.3 -l libcrypto.so.3 \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& ltrace_ttt_S_non_fips.log
```

## strace

### FIPS case

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-fips-debug/lib/ \
  OPENSSL_CONF=/home/jaruga/.local/openssl-3.0.8-fips-debug/ssl/openssl_fips.cnf \
  strace -f \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& strace_f_fips.log
```

### Non-FIPS case

```
$ LD_LIBRARY_PATH=/home/jaruga/.local/openssl-3.0.8-debug/lib/ \
  strace -f \
  ruby -I lib -e "require 'openssl'; OpenSSL::PKey.read(File.read('key.pem'))" >& strace_f_non_fips.log
```
