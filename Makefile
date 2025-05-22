CC = gcc
SRCS = reproducer.c
OBJS = $(SRCS:.c=.o)
# Not OK. The openssl version I reported the issue originally.
# OPENSSL_DIR = /home/jaruga/.local/openssl-3.2.0-dev-fips-debug-06a0d40322
# OK. The previous commit before fixing OSSL_DECODER_CTX_set_selection not applying the value.
OPENSSL_DIR = /home/jaruga/.local/openssl-3.5.0-dev-fips-debug-054f6c0fc1
# OK. The commit fixing OSSL_DECODER_CTX_set_selection not applying the value.
# OPENSSL_DIR = /home/jaruga/.local/openssl-3.5.0-dev-fips-debug-c2f4d7aae1
OPENSSL_INC_DIR = $(OPENSSL_DIR)/include
OPENSSL_LIB_DIR = $(OPENSSL_DIR)/lib
CFLAGS = -I $(OPENSSL_INC_DIR) -L $(OPENSSL_LIB_DIR) -O0 -g3 -ggdb3 -gdwarf-5
LDFLAGS = -Wl,-rpath,$(OPENSSL_LIB_DIR)
EXE = reproducer
LIBS = -lcrypto

.c.o :
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

.PHONY: all
all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

.PHONY: run-non-fips
run-non-fips :
	OPENSSL_CONF_INCLUDE=$(OPENSSL_DIR)/ssl \
	OPENSSL_MODULES=$(OPENSSL_LIB_DIR)/ossl-modules \
	./reproducer key.pem

.PHONY: run-fips
run-fips :
	sed -e "s|OPENSSL_DIR|$(OPENSSL_DIR)|" openssl_fips.cnf.tmpl > \
		openssl_fips.cnf
	OPENSSL_CONF=$(shell pwd)/openssl_fips.cnf \
	OPENSSL_CONF_INCLUDE=$(OPENSSL_DIR)/ssl \
	OPENSSL_MODULES=$(OPENSSL_LIB_DIR)/ossl-modules \
	./reproducer key.pem

.PHONY: clean
clean :
	rm -f *.o $(EXE)
