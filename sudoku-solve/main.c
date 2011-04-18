/***************************************************************************
 main.c  -  Main program
 -------------------
 begin                : 2006-01-07
 copyright            : (C) 2006 by Jan Friesse
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
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

const int SA_WIDTH=9;
const int SA_HEIGHT=9;
const int SA_DEFAULT_VALUE=0;
const int SA_MAX_VALUE=9;
const int SA_RECTANGLE_WIDTH=3;
const int SA_RECTANGLE_HEIGHT=3;
#define SA_SIZE 81

//const int SA_WIDTH=16;
//const int SA_HEIGHT=16;
//const int SA_DEFAULT_VALUE=0;
//const int SA_MAX_VALUE=16;
//const int SA_RECTANGLE_WIDTH=4;
//const int SA_RECTANGLE_HEIGHT=4;
//#define SA_SIZE 256

typedef struct sudokuArrayItem {
  int value;
  bool constant;
} sudokuArrayItem;

typedef sudokuArrayItem sudokuArray[SA_SIZE];

void clearArray(sudokuArray sa) {
  for (int x=0;x<SA_WIDTH;x++) 
    for (int y=0;y<SA_HEIGHT;y++) {
      sa[y*SA_WIDTH+x].value=SA_DEFAULT_VALUE;
      sa[y*SA_WIDTH+x].constant=false;
    }
}

void printArray(sudokuArray sa) {
  for (int y=0;y<SA_HEIGHT;y++) {
    for (int x=0;x<SA_WIDTH;x++) {
      printf("%d ",sa[y*SA_WIDTH+x].value);
    }
    printf("\n");
  }
}

bool isPosibleMove(sudokuArray sa,int pos,int what) {
  int x=pos%SA_WIDTH;
  int y=pos/SA_WIDTH;
  bool res=true;
  
  if (!(x>=0 && x<SA_WIDTH)) {
    fprintf(stderr,"Debug error. X (%d) is not valid!\n",x);
    return false;
  }
  if (!(y>=0 && y<SA_HEIGHT)) {
    fprintf(stderr,"Debug error. Y (%d) is not valid!\n",y);
    return false;
  }
  
  if (!(what>=0 && what<=SA_MAX_VALUE)) {
    fprintf(stderr,"Debug error. What (%d) is not valid!\n",what);
    return false;
  }

  //Test horizontal line
  for (int hx=0;hx<SA_WIDTH;hx++) {
    if (sa[y*SA_WIDTH+hx].value==what) {res=false;return res;}
  }

  if (!res) return res;
  
  //Test vertical line
  for (int hy=0;hy<SA_HEIGHT;hy++) {
    if (sa[hy*SA_WIDTH+x].value==what) {res=false;return res;}
  }
  
  if (!res) return res;

  //Now test rectangle
  int rectangleSX=x/SA_RECTANGLE_WIDTH;
  int rectangleSY=y/SA_RECTANGLE_HEIGHT;
  
  for (int hx=0;hx<SA_RECTANGLE_WIDTH;hx++) 
    for (int hy=0;hy<SA_RECTANGLE_HEIGHT;hy++) {
      if (sa[(hy+rectangleSY*SA_RECTANGLE_HEIGHT)*SA_WIDTH+hx+rectangleSX*SA_RECTANGLE_WIDTH].value==what) {res=false;return res;}
    }
  
  return res;
}

int findFirstBackwardNonConstant(sudokuArray sa,int pos) {
  int i=pos-1;
  
  while (i>=0 && sa[i].constant) i--;

  return i;
}

bool solve(sudokuArray sa) {
  int pos=0;
  
  while (pos>=0) {
    //Test what we can...
    
    if (pos>=SA_SIZE) {
      //Finish!!! We found solution...
      printf("Solution :\n");
      printArray(sa);
/*      char t[2];
     
      gets(t);*/
      pos=findFirstBackwardNonConstant(sa,pos-1);
    }

    if (sa[pos].constant) {
      //We cant do nothing with constant... go one step
      pos++;
    } else {
      //It's variable. So increment my value
      int value=sa[pos].value+1;
      
      if (value>SA_MAX_VALUE) {
        //This is no way. So we must go backward to first not constant value
        //But before this, we set default value for this
        sa[pos].value=SA_DEFAULT_VALUE;

        pos=findFirstBackwardNonConstant(sa,pos);
      } else {
        int oldPos=pos;
        
        //Test posibility of this move
        if (isPosibleMove(sa,pos,value)) pos++;

        //We put value
        sa[oldPos].value=value;
      }
    }
  }

  return true;
}

void setConstant(sudokuArray sa,int x,int y,int value) {
  sa[y*SA_WIDTH+x].constant=true;
  sa[y*SA_WIDTH+x].value=value;
}

void readStdin(sudokuArray sa) {
  int ch;

  for (int y=0;y<SA_HEIGHT;y++) {
    for (int x=0;x<SA_WIDTH;x++) {
      while (((ch = fgetc(stdin)) != EOF) && ((ch < '0') || (ch > '9'))) ;

      if (ch == EOF) {
        fprintf(stderr, "Unexpected end of file\n");
        exit(1);
      }

      if (ch != '0') {
        setConstant(sa,x,y,ch-'0');
      }
    }
  }
}

int main(void) {
  sudokuArray sa;

  clearArray(sa);

  readStdin(sa);

  printf("Readed :\n");
  printArray(sa);

  solve(sa);
  return 0;
}
