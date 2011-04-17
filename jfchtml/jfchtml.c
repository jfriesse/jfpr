/************************************************************************
 * JFCHTML v 0.7 - (C) 2001 Jan Friesse (janf@email.cz)                 *
 ************************************************************************
 * Program to compress your HTML files.                                 *
 ************************************************************************
 * This program is developed under GPL licence                          *
 ************************************************************************
 * Features :                                                           *
 * Can compress DOS line ending (#13#10) to space (#32)                 *
 * Can delete spaces                                                    *
 * Ignoring text in <PRE> and <SCRIPT> tags                             *
 * Full support of JAVA SCRIPT                                          *
 ************************************************************************
 * HISTORY :                                                            *
 * v 0.7 - First idea and relase                                        *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1 /*true*/
#define false 0 /*false*/

typedef char passtring[256]; /*standart pascal string */

/*My help*/
void help(void)
{
  printf("JFCHTML v 0.7 - JF Compress HTML - Program to compress your HTML pages.\n");
  printf("Copyright (C) 2001 Jan Friesse (janf@email.cz)\n\n");
  printf("This program is free software; you can redistribute it and/or modify\n");
  printf("it under the terms of the GNU General Public License version 2 as published by\n");
  printf("the Free Software Foundation.\n\n");
  printf("This program is distributed in the hope that it will be useful,\n");
  printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
  printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
  printf("GNU General Public License for more details.\n\n");
  printf("You should have received a copy of the GNU General Public License\n");
  printf("along with this program; if not, write to the Free Software\n");
  printf("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\n");
  printf("Using program : \n");
  printf("JFCHTML source_page.html destination_page.html\n");
  printf("Source_file and destination_file can't be same files!\n");
  printf("Example : \n");
  printf("JFCHTML index.html indexc.html\n");
}

/*Function to open/create file*/
FILE *openfile(passtring FN,passtring FAT,passtring ACT) /*FN = File Name, FAT = File ATtributes, ACT = ACTion*/
{
  FILE *f;

  printf("Traying to %s file \"%s\"...",ACT,FN);
  f=fopen(FN,FAT);
  if (f==NULL)
    printf("Error\nCan't %s file.\n",ACT);
  else
    printf("OK\n");
  return f;
}

/*Function to close file*/
void closefile(FILE *F,passtring FN)
{
  int error;
  printf("Closing file \"%s\"...",FN);
  error=fclose(F);
  if (error!=0)
     printf("Error in closing file \"%s\"!\n",FN);
  else
    printf("OK\n");
}

/*MAIN*/
int main(int argc,char *argv[])
{
  FILE *SF,*DF;
  char ch;
  int IsJavaScript,active,putchar,tag,IsJSpace,IsJApostrof,IsSpace; /*This is all boolean values*/
  passtring tags,s;
  unsigned long int readed;unsigned long int writed;

  /*Test 3 parameters*/
  if (argc!=3)
  {
    help();
    return 1;
  }
  if (strcmp(argv[1],argv[2])==0)
  {
    help();
    printf("Files have same name!\n");
    return 1;
  }
  SF=openfile(argv[1],"rb","open"); /*open*/
  DF=openfile(argv[2],"wb","create");
  if (SF==NULL || DF==NULL) return 2;
  readed=writed=0;
  /*OK. Let's compress*/
  printf("Compressing...\n");
  tag=false;
  IsSpace=false;
  strcpy(tags,"");
  active=true;
  IsJavaScript=false;
  do
  {
    putchar=true;
    ch=fgetc(SF);/* read a char from the file */
    readed++;
    if (ch=='<') tag=true;
    if (tag==false && active==true)
    {
      if (ch==13) putchar=false; /*ignore 13 char*/
      if (ch==10) ch=32; /*change 10 to 32*/
      if (ch==' ')
      {
	if (IsSpace) putchar=false; else IsSpace=true;
      }
      if (ch!=' ' && putchar==true) IsSpace=false;
    }
    if (tag==true)
    {
      if (ch!='<' && ch!='>' && (strlen(tags)<255)) {s[1]=0;s[0]=ch;strcat(tags,s);}
    }
    if (ch=='>') /*End of tag*/
    {
/*      strcpy(uptag,strupr(tags));*/
/*      printf("tag is %s\n",uptag);*/ /*Test to upercase*/
      if (stricmp(tags,"PRE")==0) active=false;
      if (stricmp(tags,"/PRE")==0) active=true;
      if (strnicmp(tags,"SCRIPT",6)==0) active=false;
      if (stricmp(tags,"SCRIPT LANGUAGE=\"JAVASCRIPT\"")==0) {IsJavaScript=true;IsJSpace=false;IsJApostrof=false;}
      if (strnicmp(tags,"/SCRIPT",7)==0) {active=true;IsJavaScript=false;}
      if (stricmp(tags,"P")==0) IsSpace=true;
      if (stricmp(tags,"/P")==0) IsSpace=true;
      if (stricmp(tags,"BR")==0) IsSpace=true;
      strcpy(tags,"");
      tag=false;
    }
    if (IsJavaScript==true)
    {
      if (ch=='\"') IsJApostrof=1-IsJApostrof;
      if (IsJApostrof==false)
      {
	if (ch==13) putchar=false;
	if (ch==10) ch=10;
	if (ch==' ')
	{
	  if (IsJSpace) putchar=false; else IsJSpace=true;
	}
	if (ch!=' ' && putchar==true) IsJSpace=false;
      }
    }
    if (putchar) {writed++;fputc(ch,DF);}/*Put char to output*/
  }
  while (ch != EOF);
  /*We finished. Time to close files...*/
  closefile(DF,argv[2]);
  closefile(SF,argv[1]);
  /*Write final information*/
  readed--;
  printf("File before compresion had %lu bytes.\n",readed);
  printf("File after compresion has %lu bytes.\n",writed);
  printf("Compression ratio is %0.2f\%\n",(float)writed/(float)readed*100);
  /*And logout*/
  return 0;
}