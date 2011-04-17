/***************************************************************************
 main.c  -  Main program
 -------------------
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
#include <stdio.h>
#include <time.h>

#define NEXTDAYSEC 24*60*60

int main(int argc,char *argv[])
{
  FILE *source;
  char line[4096],humanname[4096]; /*Line to read, Human name*/
  int day,month,spec_year; /*Day, Mounth, Special, maybe year*/
  int linenow; /*Actual line*/
  time_t the_time,next_day_time;
  struct tm *timestruct; /*Actual time*/
  int fooi,NODates; /*Fooi and Number of printed dates*/
    
  source=fopen("vyroci.txt","rt"); /*Open Source file*/
  
  if (source==NULL)
  {
    fprintf(stderr,"Can't load vyroci.txt\nProgram exiting...\n");
    return -1;
  }
  linenow=0;
  
  the_time=time((time_t*)0 );

  NODates=3;

  if (argc>1) sscanf(argv[1],"%d",&NODates);
  
  for (fooi=0;fooi<NODates;fooi++)
  {
    fseek(source,0,SEEK_SET);
    
    /*First. Get actual date*/
    timestruct=localtime(&the_time);


    printf("Dne %02d.%02d.%04d slaví\n",timestruct->tm_mday,timestruct->tm_mon+1,1900+timestruct->tm_year);  /*Print actualdate*/
    
    while (fgets(line,4095,source)!=NULL) /*Read all lines*/
    {
      linenow++;
      if (line[0]!='[' && line[0]!='\n') /*This is not only comment and not free line*/
      {
        line[strlen(line)-1]=0; /*Delete End of line*/
        day=month=spec_year=-1;  /*Set default values*/
  
        strcpy(humanname,"");
        sscanf(line,"%d.%d %d %s\n",&day,&month,&spec_year,humanname); /*Scan line*/
        if (day==-1 || month==-1 || spec_year==-1 || strcmp(humanname,"")==0)
        {
          fprintf(stderr,"Syntax error in line %d\nLine is \"%s\"\nLine was ignored...\n",linenow,line);
        }
        else /*Ok. All is OK. I can test actual date with line date*/
        {
          strcpy(humanname,line+11); 
          if (day==timestruct->tm_mday && month==timestruct->tm_mon+1)
          {
            if (spec_year==0) printf("Svátek %s\n",humanname);else
            if (spec_year==9999) printf("Svátek %s\n",humanname);else
            if (spec_year==8888) printf("Narozeniny %s\n",humanname);
            else
              printf("%s %d roku\n",humanname,1900+timestruct->tm_year-spec_year);
          }
          /*It is not actual date. Try next*/  
        }/*End of all is ok*/
        
  /*      printf("Linky : %sDekodovano : %02d.%02d %04d %s\n",line,day,month,spec_year,humanname);*/
      } /*End of line is not comment*/

    } /*End of Read all lines*/
    the_time+=NEXTDAYSEC;
  }  
  fclose(source);/*Close source file*/
  return 0;
}
