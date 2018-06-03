
DIR_ROOT=../..
DIR_SRC = ../../source

DIR_INCLUDE = -I../../../include
DIR_LDFLAGS = -L../../../libs/ -Wl,-rpath ../../../libs/ -ltoyserver

all: serverTest clientTest cplib

serverTest : $(DIR_SRC)/serverTest.c
	rm -f $@
	gcc -g $< -o $@  $(DIR_INCLUDE)  $(DIR_LDFLAGS)
	rm -rf serverTest.*

clientTest : $(DIR_SRC)/clientTest.c
	rm -f $@
	gcc -g $< -o $@ $(DIR_INCLUDE)  $(DIR_LDFLAGS)
	rm -rf clientTest.*

cplib:
	cp ../../../libs/libtoyserver.so .

clean:
	rm -f serverTest clientTest  libtoyserver.so
