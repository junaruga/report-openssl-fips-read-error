CC = gcc
SRCS = reproducer.c
OBJS = $(SRCS:.c=.o)
OPENSSL_DIR = /home/jaruga/.local/openssl-3.2.0.dev-fips-debug-06a0d40322
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

.PHONY: clean
clean :
	rm -f *.o $(EXE)
