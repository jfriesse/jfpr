#***************************************************************************
# Makefile
# -------------------
# begin                : 2006-01-07
# copyright            : (C) 2006 by Jan Friesse
# email                : janf@email.cz
# ***************************************************************************/
#
#/***************************************************************************
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# ***************************************************************************/
CC = gcc
#DEBUG = TRUE

#DEBUG = TRUE          #enable this for debug

ifeq ($(DEBUG),TRUE)
  #debug
  LD_PARAMS =
  CC_PARAMS =-ggdb -Wall -std=c99 -pedantic
else
  #final
  LD_PARAMS =-O2 -s
  CC_PARAMS =-Wall -std=c99 -pedantic
endif

O_LIST = main.o

all : sudoku-solve

sudoku-solve : $(O_LIST)
	$(CC) $(LD_PARAMS) $(O_LIST) -o sudoku-solve
    
main.o : main.c
	$(CC) $(CC_PARAMS) -c main.c

clean :
	rm $(O_LIST) sudoku-solve

rebuild : clean all

distr:
	tar -czf sudoku-solve.tar.gz main.c Makefile
