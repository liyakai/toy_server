DIR_ROOT = ../..
CC = gcc
CFLAGS = -Wall -Werror -g -fPIC
DIR_INCLUDE = -I$(DIR_ROOT)/include
DIR_SRC = $(DIR_ROOT)/source
DIR_BIN = $(DIR_ROOT)/bin
DIR_LIB = $(DIR_ROOT)/libs
SRCS = $(wildcard $(DIR_SRC)/*.c)
$(warning $(SRCS))
OBJS = $(patsubst %.c, %.o,$(SRCS))

LD_LIB = $(DIR_ROOT)/../openssl/libssl.a $(DIR_ROOT)/../openssl/libcrypto.a

LIB_A = libtoyserver.a
LIB_SO = libtoyserver.so


#LIB_A_D = $(patsubst %.a, $(DIR_LIB)/%.a, $(LIB_A))
#LIB_SO_D = $(patsubst %.so, $(DIR_LIB)/%.so, $(LIB_SO))
	
all: $(LIB_A) $(LIB_SO)
	cp $(LIB_A) $(DIR_LIB)/$(LIB_A)
	cp $(LIB_SO) $(DIR_LIB)/$(LIB_SO)
	rm -f $(LIB_A)
	rm -f $(LIB_SO)

$(LIB_A): $(OBJS)
	rm -f $@
	ar crv  $@ $(OBJS) $(LD_LIB) 

$(LIB_SO): $(OBJS)
	rm -f $@
	${CC} -shared -o $@ $(OBJS) $(LD_LIB)
	rm -f $(OBJS)

$(DIR_SRC)/%.o : $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) $(DIR_INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJS)  $(DIR_LIB)/$(LIB_A) $(DIR_LIB)/$(LIB_SO)
