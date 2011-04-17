/***************************************************************************
                          convert.h  -  header for convert.c
                             -------------------
    begin                : Tue Aug 28 2001
    copyright            : (C) 2001 by Jan Friesse
    email                : janf@email.cz
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONVERT_H
#define CONVERT_H

#define KAMC 0
#define W95C 1
#define LA2C 2
#define W31C 3
#define UNXC 4
#define KO8C 5
#define CORC 6
#define IBMC 7

#define EOLDOS 10
#define EOLUNIX 20
#define EOLMAC 30

#define ECANTOPENSOURCEFILE -1
#define ECANTCREATEDESTFILE -2
#define EALLOK               1

#define CONV_BUF_LEN 256
extern char EOL_NAME_STR[3][10];
extern char CODING_NAME_STR[8][20];

typedef void code_call_back_proc(char *sname,char *dname,int scode,int dcode,int percent);

/**Procedure to convert file with coding fromc to coding toc. Source file is
file1name and destination
   is file2name*/
extern int convert(int fromc,int toc,char *file1name,char *file2name,code_call_back_proc call_back);
/**Get coding of file*/
extern int getcoding(char *file1name);

#endif
