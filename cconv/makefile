#make file

CC = gcc
DEBUG = FALSE

#DEBUG = TRUE          #enable this for debug

ifeq ($(DEBUG),TRUE)
  #debug
  LD_PARAMS =
  CC_PARAMS =-ggdb
else
  #final
  LD_PARAMS =-o6 -s
  CC_PARAMS =-Wall -ansi -pedantic
endif

all : cconv

rebuild : clean cconv

install : cconv cconv.1.gz
	cp ./cconv /usr/bin/cconv
	cp ./cconv.1.gz /usr/share/man/man1/cconv.1.gz
	cp ./tkcconv    /usr/bin/tkcconv
	chmod +x /usr/bin/cconv
	chmod +x /usr/bin/tkcconv
	
uninstall:
	rm /usr/bin/cconv
	rm /usr/share/man/man1/cconv.1.gz
	rm /usr/bin/tkcconv
	
clean :
	rm *.o cconv

cconv : main.o convert.o
	$(CC) $(LD_PARAMS) -o $@ main.o convert.o

main.o : main.c
	$(CC) $(CC_PARAMS) -c main.c

convert.o : convert.c convert.h
	$(CC) $(CC_PARAMS) -c convert.c

