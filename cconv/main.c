/***************************************************************************
 *                         main.c  -  main file n                          *
 *                            -------------------                          *
 *   begin                : Ne pro  8 21:15:08 CET 2002                    *
 *   copyright            : (C) 2002 by Jan Friesse                        *
 *   email                : janf@email.cz                                  *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "convert.h"

/*#define DEBUG*/

#define TRUE  1
#define FALSE 0

#define PROG_NAME "cconv"
#define PROG_VER_MAJ    0
#define PROG_VER_MIN    2

#define ALL_OK                  0
#define ERROR                   1
#define ERROR_OPEN_SOURCE       2
#define ERROR_CREATE_DESTINATON 3
#define ERROR_BAD_NO_ARGS       4
#define ERROR_ARG_ERROR         5
#define ERROR_NO_INPUT_FILE     6

#define MAX_PATH_LEN 4096

void text_call_back(char *sname,char *dname,int scode,int dcode,int percent)
{
  int i;

  #ifdef DEBUG
    printf("%s(%d)->%s(%d) %d%\n",sname,scode,dname,dcode,percent);
  #endif
  
  printf("\r[");
  
  for (i=0;i<percent/5;i++) printf(".");
  for (i=percent/5;i<100/5;i++) printf(" ");
  printf("]   ");
  if (percent%4==0) printf("- ");
  if (percent%4==1) printf("\\ ");
  if (percent%4==2) printf("| ");
  if (percent%4==3) printf("/ ");
  return ;
}

int show_version(void)
{
  printf("%s v %d.%d (c)2002 Jan Friesse (janf@email.cz)\n",PROG_NAME,PROG_VER_MAJ,PROG_VER_MIN);

  return 0;
}

int show_help(void)
{
  show_version();

  printf("%s [OPTIONS]\n",PROG_NAME);
  printf("Options : \n");
  printf("%-20s%s","  --help, -h","Display this help\n");
  printf("%-20s%s","  --input,-i","Next parameter is name of file to convert\n");
  printf("%-20s%s","  --output,-o","Next parameter is destination file\n");
  printf("%-20s%s","  --sourcecode,-s","Coding of source file\n");
  printf("%-20s%s","  --destcode,-d","Coding of destination file\n");
  printf("%-20s%s","  --version,-v","Display version of this program\n");
  printf("%-20s%s","  --showstatus,-ss","Show status of coding\n");
  printf("%-20s%s","  --hidestatus,-hs","Hide status of coding (this is default)\n\n");
  printf("Coding can be number or string representation.\nSee man cconv for more informations\n\n");
  printf("For reporting bugs, please write to (janf@email.cz)\n");
 
  return ALL_OK;
}

int parse_coding(char *coding)
{
  int testnum;
  int res;
  char foostr[256];
  int code;
  /*I use two version of information, number and string representation*/
  
  /*First test numeric representation*/
  res=sscanf(coding,"%d%s",&testnum,foostr);

  if (res==1)
  {
    /*Yes it is numeric representation. Return it*/
    return testnum;
  }

  /*It isn't numeric representation. Decode string*/

  if (strlen(coding)!=4) return 0; /*Bad string length*/
  code=0;
  
  if (strcmp(coding+1,"kam")==0) code+=KAMC;
  if (strcmp(coding+1,"w95")==0) code+=W95C;
  if (strcmp(coding+1,"la2")==0) code+=LA2C;
  if (strcmp(coding+1,"w31")==0) code+=W31C;
  if (strcmp(coding+1,"unx")==0) code+=UNXC;
  if (strcmp(coding+1,"ko8")==0) code+=KO8C;
  if (strcmp(coding+1,"cor")==0) code+=CORC;
  if (strcmp(coding+1,"ibm")==0) code+=IBMC;

  if (code==0) return 0; /*No option is selected, return error*/

  if (coding[0]=='d') code+=EOLDOS;
  if (coding[0]=='m') code+=EOLMAC;
  if (coding[0]=='u') code+=EOLUNIX;
  
  if (code<10) return 0;/*No option is selected, return error*/

  return code;
}


int get_coding(char *coding)
{
  int res;
  int eolc,cod;
  
  res=parse_coding(coding);
  eolc=res/10;
  cod=res%10;

  if (res==0 || (eolc>3 || eolc<1) || (cod>7 || cod<0))
  {
    printf("%s : unrecognized coding : %s. Run cconv --help for options.\n",PROG_NAME,coding);
    return ERROR;
  }

  return res;
}

int write_coding(int coding)
{
  int eolc,cod;

  eolc=coding/10;
  eolc--; /*This is only for table*/
  cod=coding%10;

  printf("%d -> End of lines : %s, Czech coding : %s\n",coding,EOL_NAME_STR[eolc],CODING_NAME_STR[cod]); 

  return ALL_OK;
}

int analyze_argv(int argc,char *argv[],char *sname,char *dname,int *scode,int *dcode,int *showstatus)
{
  int i;
  int nextop;
  int parsed;
  int res;
  int autooper;
  
  /*Initialize default values...*/
  
  /*Default operation is 1*/
  nextop=1;
  autooper=TRUE;

  strcpy(sname,"");strcpy(dname,"");*scode=*dcode=*showstatus=0;

  for (i=1;i<argc;i++)
  {
    parsed=FALSE;
  
    if (strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h")==0) {show_help();parsed=TRUE;}
    if (strcmp(argv[i],"--input")==0 || strcmp(argv[i],"-i")==0) {nextop=1;autooper=FALSE;parsed=TRUE;}
    if (strcmp(argv[i],"--output")==0 || strcmp(argv[i],"-o")==0) {nextop=2;autooper=FALSE;parsed=TRUE;}
    if (strcmp(argv[i],"--sourcecode")==0 || strcmp(argv[i],"-s")==0) {nextop=3;autooper=FALSE;parsed=TRUE;}
    if (strcmp(argv[i],"--destcode")==0 || strcmp(argv[i],"-d")==0) {nextop=4;autooper=FALSE;parsed=TRUE;}
    if (strcmp(argv[i],"--version")==0 || strcmp(argv[i],"-v")==0) {show_version();parsed=TRUE;}
    if (strcmp(argv[i],"--hidestatus")==0 || strcmp(argv[i],"-hs")==0) {*showstatus=0;parsed=TRUE;}
    if (strcmp(argv[i],"--showstatus")==0 || strcmp(argv[i],"-ss")==0) {*showstatus=1;parsed=TRUE;}
  
    if (parsed==FALSE)
    {
      if (argv[i][0]!='-' && nextop!=0)
      {
        /*Try to get parameter as file*/
        if (nextop==1) strncpy(sname,argv[i],MAX_PATH_LEN-1);
        if (nextop==2) strncpy(dname,argv[i],MAX_PATH_LEN-1);
        if (nextop==3 || nextop==4) 
	{
          res=get_coding(argv[i]);
	  if (res==ERROR) return ERROR;
	  if (nextop==3) *scode=res; else *dcode=res;
	}
	if (autooper==TRUE) nextop++; else nextop=0;
      }
      else
      { 
        printf("cconv: unrecognized option %s\n",argv[i]);
        return ERROR;
      }
    } /*If Parsed==FALSE*/
  
  } /*For i=*/

  return ALL_OK;
}

int main(int argc, char *argv[])
{
  char sname[MAX_PATH_LEN],dname[MAX_PATH_LEN];
  int scode,dcode;
  int res;
  int showstatus;
  code_call_back_proc *call_back;

  if (argc<2) 
  {
    printf("cconv (%d) : bad number of arguments. Run cconv --help for options...\n",ERROR_BAD_NO_ARGS);
    return ERROR_BAD_NO_ARGS;
  }
  /*Parse arguments*/
  res=analyze_argv(argc,argv,sname,dname,&scode,&dcode,&showstatus);

  if (res!=ALL_OK) return ERROR_ARG_ERROR;

  /*Arguments are parsed. If I have filled only sname, get coding of file*/
           
  if (strcmp(dname,"")==0 && scode==0 && dcode==0)
  {
    if (strcmp(sname,"")==0) /*I have no sname*/
    {
      return ALL_OK;
    }
    res=getcoding(sname);
    if (res==ECANTOPENSOURCEFILE)
    {
      printf("%s (%d): can't open source file %s...\n",PROG_NAME,ERROR_OPEN_SOURCE,sname);
      return ERROR_OPEN_SOURCE;
    }
    printf("\"%s\" = ",sname);
    write_coding(res);
    return ALL_OK;
  }
  /*OK. Try to convert file*/ 
  /*But first try to catch errors*/
  if (strcmp(dname,"")==0) {printf("%s (%d): you must enter destination file name...\n",PROG_NAME,ERROR_ARG_ERROR);return ERROR_ARG_ERROR;}
  if (strcmp(sname,"")==0) {printf("%s (%d): you must enter source file name...\n",PROG_NAME,ERROR_ARG_ERROR);return ERROR_ARG_ERROR;}
  if (scode==0) {printf("%s (%d): you must enter source coding...\n",PROG_NAME,ERROR_ARG_ERROR);return ERROR_ARG_ERROR;}
  if (dcode==0) {printf("%s (%d): you must enter destination coding...\n",PROG_NAME,ERROR_ARG_ERROR);return ERROR_ARG_ERROR;}
  /*Now convert*/
  if (showstatus!=0)
    printf("Converting %s(%d) to %s(%d)\n",sname,scode,dname,dcode);

  if (showstatus==0) call_back=NULL; else call_back=text_call_back;

  res=convert(scode,dcode,sname,dname,call_back);

  if (res==ECANTOPENSOURCEFILE) 
  {
    printf("%s (%d): can't open source file %s...\n",PROG_NAME,ERROR_OPEN_SOURCE,sname);
    return ERROR_OPEN_SOURCE;
  }

  if (res==ECANTCREATEDESTFILE) 
  {
    printf("%s (%d): can't create destination file %s...\n",PROG_NAME,ERROR_CREATE_DESTINATON,dname);
    return ERROR_CREATE_DESTINATON;
  }
  
  if (res==EALLOK && showstatus!=0) printf("OK\n");
  return EXIT_SUCCESS;
}
