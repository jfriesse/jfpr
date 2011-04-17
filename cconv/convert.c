/***************************************************************************
                          convert.c  -  main convert unit                      
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

#include "convert.h"
#include <stdio.h>
#include <stdlib.h>

                       /*KAM,W95,LA2,W31,UNX,KO8,COR,IBM*/
int codingtable[44][8]={{160,225,160,160,225,193,225, 97},
                        {143,193,181,181,193,225,193, 65},
                        {132,228,132,132,228,209,228, 97},
                        {142,196,142,142,196,241,196, 65},
                        {135,232,159,159,232,195,163, 99},
                        {128,200,172,171,200,227,131, 67},
                        {131,239,212,212,239,196,164,100},
                        {133,207,210,210,207,228,132, 68},
                        {130,233,130,130,233,215,233,101},
                        {144,201,144,144,201,247,201, 69},
                        {136,236,216,216,236,197,165,101},
                        {137,204,183,183,204,229,133, 69},
                        {161,237,161,161,237,201,237,105},
                        {139,205,214,214,205,233,205, 73},
                        {141,229,146,146,229,203,168,108},
                        {138,197,145,145,197,235,136, 76},
                        {140,190,150,150,181,204,169,108},
                        {156,188,149,149,165,236,137, 76},
                        {164,242,229,229,242,206,172,110},
                        {165,210,213,213,210,238,140, 78},
                        {162,243,162,162,243,207,243,111},
                        {149,211,224,224,211,239,211, 79},
                        {147,244,147,147,244,208,244,111},
                        {167,212,226,226,212,240,212, 79},
                        {148,246,148,148,246,205,246,111},
                        {153,214,153,153,214,237,214, 79},
                        {170,224,234,234,224,198,175,114},
                        {171,192,232,232,192,230,143, 82},
                        {169,248,253,254,248,210,176,114},
                        {158,216,252,253,216,242,144, 82},
                        {168,154,231,231,185,211,178,115},
                        {155,138,230,230,169,243,146, 83},
                        {159,157,156,156,187,212,180,116},
                        {134,141,155,155,171,244,148, 84},
                        {163,250,163,163,250,213,250,117},
                        {151,218,233,233,218,245,218, 85},
                        {150,249,133,133,249,202,183,117},
                        {166,217,222,222,217,234,151, 85},
                        {129,252,129,129,252,200,252,117},
                        {154,220,154,154,220,232,220, 85},
                        {152,253,236,236,253,217,253,121},
                        {157,221,237,237,221,249,221, 89},
                        {145,158,167,166,190,218,186,122},
                        {146,142,166,165,174,250,154, 90}};

char EOL_NAME_STR[3][10]={"DOS","UNIX","MAC"};
char CODING_NAME_STR[8][20]={"Kamenici","Windows 9x/NT","Latin 2","Windows 3.11","Unix","KO8","Cor","IBM"};

int convert(int fromc,int toc,char *file1name,char *file2name,code_call_back_proc call_back)
{
  FILE *sourcef,*destf;
  int ConvertTable[256];
  int i;
  unsigned int c; /*Character*/
  unsigned char buf[CONV_BUF_LEN]; /*Added in version 0.21*/
  int bufsize;
  int IEOLines,OEOLines; /*Input end of lines, output end of lines*/
  /*Added in version 0.2*/
  long int filelen;
  long int filepos;
  int percent;
  int oldfromc,oldtoc;
  
  /*Try open file 1*/
  sourcef=fopen(file1name,"rb");
  if (sourcef==NULL) return ECANTOPENSOURCEFILE;
  
  /*Try create file 2*/
  destf=fopen(file2name,"wb");
  if (destf==NULL) return ECANTCREATEDESTFILE;

  /*Added in version 0.2*/
  oldfromc=fromc;
  oldtoc=toc;

  IEOLines=fromc/10*10; /*Added in version 0.1.1*/
  OEOLines=toc/10*10;
  fromc=fromc%10;
  toc=toc%10;

  /*Make table*/
  for (i=0;i<256;i++) ConvertTable[i]=i;
  for (i=0;i<44;i++)
    ConvertTable[codingtable[i][fromc]]=codingtable[i][toc];

  i=0;

  /*Now get length of source file. I use my method. Jump to end of file, get position and jump to start*/
  filelen=fseek(sourcef,0,SEEK_END); 
  filelen=ftell(sourcef);
  fseek(sourcef,0,SEEK_SET);
  filepos=0;

  if (call_back!=NULL) call_back(file1name,file2name,oldfromc,oldtoc,0);

  while ((bufsize=fread(buf,sizeof(char),CONV_BUF_LEN,sourcef))!=0) /*Added for stupid systems (like DOS) who has not prebuferring*/
  {
    for (i=0;i<bufsize;i++)
    {
      c=buf[i];
      filepos++;
      if (IEOLines==EOLUNIX && c==10) /*Input file is in UNIX coding*/
      {
        if (OEOLines==EOLMAC) fputc(13,destf); /*Mac have only character 13*/
        if (OEOLines==EOLDOS) {fputc(13,destf);fputc(10,destf);}; /*Dos must have 13 and 10*/
        if (OEOLines==EOLUNIX) {fputc(10,destf);}; /*Unix have only 10*/
      }
      if (IEOLines==EOLMAC && c==13) /*Input file is in MAC coding*/
      {
        if (OEOLines==EOLUNIX) fputc(10,destf);
        if (OEOLines==EOLDOS) {fputc(13,destf);fputc(10,destf);};
        if (OEOLines==EOLMAC) fputc(13,destf); 
      }
      if (IEOLines==EOLDOS) /*DOS must have ever something extra*/
      {
        if (OEOLines==EOLUNIX && c==10) fputc(10,destf);
        if (OEOLines==EOLMAC && c==13) fputc(13,destf);
        if (OEOLines==EOLDOS && (c==10 || c==13)) {fputc(c,destf);}; 
      }
      if (c!=10 && c!=13)
      {
        c=ConvertTable[c];
        fputc(c,destf);
      }
      if (filepos%10000==0) /*I call call back method once upon 10000 bytes*/
      {
        percent=(int)((float)filepos/(float)filelen*100);
        if (call_back!=NULL) call_back(file1name,file2name,oldfromc,oldtoc,percent);
      }
    } /*End for*/
  } /*End while*/
  fclose(sourcef); /*Close source file*/
  fclose(destf); /*Close destination file*/

  if (call_back!=NULL) call_back(file1name,file2name,oldfromc,oldtoc,100);

  return EALLOK;
}


/*Get coding of file*/
int getcoding(char *file1name)
{
  FILE *sourcef;
  int x,y;
  int c; /*Character*/
  int num; /*Number of readed chars*/
  int coding[7]; /*Number of codes*/
  int max; /*Maximal value*/
  int noc13,noc10; /*Number Of Character 13, Number Of Character 10*/

  /*Try open file 1*/
  sourcef=fopen(file1name,"rb");
  if (sourcef==NULL) return ECANTOPENSOURCEFILE;

  for (x=0;x<7;x++) coding[x]=0; /*Null*/

  num=0;
  noc13=noc10=0; /*Set default value*/
  while ((c=fgetc(sourcef))!=EOF && num<32000)
  {
    for (x=0;x<7;x++)
    for (y=0;y<44;y++)
      if (c==codingtable[y][x]) /*I find caracter in code table*/
        coding[x]++;
    if (c==13) noc13++;
    if (c==10) noc10++;
    num++;
  }

  fclose(sourcef); /*Close source file*/
  max=coding[0];
  y=0; /*Position of coding*/
  for (x=1;x<7;x++)
    if (coding[x]>max) {max=coding[x];y=x;};

  if (max==0) y=IBMC; /*It is free IBM text*/

  if (noc13==noc10) y+=EOLDOS; /*Get End of lines*/
  if (noc13>noc10) y+=EOLMAC;
  if (noc13<noc10) y+=EOLUNIX;

  return y; /*Return coding*/
}

