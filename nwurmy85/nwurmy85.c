/*
  nwurmy85.c - Hlavni a jediny soubor pro GUX cviceni 2.
  Jan Friesse 2006 (xfries00@stud.fit.vutbr.cz)
*/

/*
 * Standard XToolkit and OSF/Motif include files.
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h> 
#include <X11/xpm.h>
/*
 * Public include files for widgets used in this file.
 */
#include <Xm/MainW.h> 
#include <Xm/Form.h> 
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Protocols.h>
#include <X11/Xmu/Editres.h>
#include <Xm/MessageB.h>
#include <Xm/DialogS.h> 
                           
/*
 * Common C library include files
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*Pixmapy - Vzdy existuje mala (10x10) a velka (20x20) verrze. Pri zvetseni 
  okna dochazi k prepinani mezi jednou z moznosti pixmap*/
#include "body_10.xpm"
#include "body_20.xpm"
#include "food_10.xpm"
#include "food_20.xpm"
#include "grass_10.xpm"
#include "grass_20.xpm"
#include "head_10.xpm"
#include "head_20.xpm"   
#include "toxic_10.xpm"
#include "toxic_20.xpm"


/*Konstanty*/
/*Definice vlastniho symbolu FALSE*/
#define FALSE    0
/*Definice vlastniho symbolu TRUE*/
#define TRUE     1

/*Play Area - Trava*/
#define PA_GRASS  0
/*Play Area - Jidlo*/
#define PA_FOOD   1
/*Play Area - Jed*/
#define PA_TOXIC  2
/*Play Area - Hlava housenky*/
#define PA_HEAD   3
/*Play Area - Telo housenky*/
#define PA_BODY   4

/*Pocet jidel ve hre - baze*/
#define PA_NO_FOOD_BASE  50
/*Pocet jidel ve hre - dle levelu. Konecny pocet jidel = BAZE+LEVEL*FOOD_LVL*/
#define PA_NO_FOOD_LVL    0
/*Pocet jedu ve hre - baze*/
#define PA_NO_TOXIC_BASE 20
/*Pocet jedu ve hre - dle levelu. Konecny pocet jedu = BAZE+LEVEL*TOXIC_LVL*/
#define PA_NO_TOXIC_LVL   5

/*Sirka hraciho pole*/
#define PA_WIDTH  40
/*Vyska hraciho pole*/
#define PA_HEIGHT 40

/*Pocet pixmap, ktere vyuzivame*/
#define NO_PIXMAPS 5

/*Standardni velikost pixmapy*/
#define PIXMAP_WIDTH  10
#define PIXMAP_HEIGHT 10

/*Nasobek velikosti pixmapy - normalni*/
#define SIZE_NORMAL    0
/*Nasobek velikosti pixmapy - dvojita*/
#define SIZE_DOUBLE    1

/*Jak dlouha ma byt housenka na zacatku hry*/
#define WORM_START_SIZE 4

/*Smer housenky*/
#define WORM_DIR_UP     0
#define WORM_DIR_LEFT   1
#define WORM_DIR_DOWN   2
#define WORM_DIR_RIGHT  3

/*Maximalni rychlost*/
#define SPEED_MAX       9
/*Baze rychlosti*/
#define SPEED_BASE    400
/*Diference rychlosti - Vysledna rychlost (kdy znovu chceme udalost od casovace) je vypoctena jako:
 SPEED_BASE-(SPEED_DIFF*gameStatus.speed)+1*/
#define SPEED_DIFF      40

/*Maximalni level*/
#define LEVEL_MAX       9


/*Definice vlastnich typu*/
/*Herni plocha*/
typedef struct PlayAreaS {
  int width,height; /*Sirka, vyska*/
  unsigned char *data; /*Data plochy, viz PA_GRASS, PA_FOOD, PA_TOXIC, PA_BODY, PA_HEIGHT*/
} PlayAreaS;
typedef PlayAreaS *PlayArea;

/*Jeden clanek housenky*/
typedef struct WormItemS {
  int x,y;
  struct WormItemS *prev,*next; /*Predchazejici a nasledujici clanek*/
} WormItemS;
typedef WormItemS *WormItem;

/*Struktura, popisujici housenku*/
typedef struct WormS {
  WormItem head,tail; /*Hlavicka a konecek housenky*/
  int length; /*Delka housenky*/
} WormS;
typedef WormS *Worm;

/*Struktura, obsahujici dulezite promenne aplikace*/
typedef struct GameStatusS {
  Widget drawArea; /*Vlastni kreslici plocha*/
  Display *display;  /*Display kreslici plochy*/
  Screen *screen;    /*Obrazovka kreslici plochy*/
  Colormap cmap;     /*Barevna mapa kreslici plochy*/
  GC gc; /*GC pro drawArea*/
  XGCValues gcv; /*gcv pro drawArea*/
  Dimension width,height; /*Sirka a vyska plochy*/
  XtIntervalId timer;      /*ID casovace*/
  XtAppContext app_context; /*Aplikacni kontext*/

  Pixmap new_pixmaps_10[NO_PIXMAPS]; /*Pixmapy 10x10*/
  Pixmap new_pixmaps_20[NO_PIXMAPS]; /*Pixmapy 20x20*/

  PlayArea playArea; /*Hraci plocha*/
  Worm worm;         /*Housenka*/
  unsigned char wormDir; /*Smer, kam pujde housenka. Viz WORM_DIR_UP/DOWN/LEFT/RIGHT*/
  int gameRunning;       /*Promenna urcujici, zda bezi hra*/
  int startLevel;        /*Startovni level*/
  int level;             /*Aktualni level*/
  int startSpeed;        /*Startovni rychlost*/
  int speed;             /*Aktualni rychlost*/
} GameStatusS;

/*Sdilene promenne*/
GameStatusS gameStatus;


/* FUNKCE */

/**Zobrazeni zpravy s textem. Vytvari tridu MessageBox.
  @param parent Rodic message boxu.
  @param text Text, ktery bude zobrazen.
  @param type Typ zpravy.
*/
void showMessage(Widget parent, char *text, int type) {
  static Widget dialog, shell;
  XmString str;
  Dimension winWidth,winHeight;

  if (dialog == NULL) {
    shell = XtVaCreateWidget("dialog",xmDialogShellWidgetClass,parent,XmNtitle, "Information",NULL);
    dialog = XtVaCreateWidget("dialog",xmMessageBoxWidgetClass,shell,NULL);
    XtUnmanageChild(XmMessageBoxGetChild(dialog,XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog,XmDIALOG_HELP_BUTTON));
  }
  str = XmStringCreateLtoR(text,XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(dialog,XmNmessageString, str,
                       XmNdialogStyle, type,
                       NULL);
  XmStringFree(str);
  
  XtManageChild(dialog);

  XtVaGetValues(shell,XmNwidth,&winWidth,XmNheight, &winHeight,NULL);
  XtVaSetValues(shell,XmNminWidth,winWidth,XmNminHeight,winHeight,NULL);
}

/**Zinicializovani herni plochy. Vsechna policka nastavi na PA_GRASS
 @param playArea Herni plocha.
 */
void pa_init(PlayArea playArea) {
  int width,height;
  int i;

  width=playArea->width;
  height=playArea->height;
  
  for (i=0;i<width*height;i++)
    playArea->data[i]=PA_GRASS;
}

/**Vytvoreni herni plochy. Vytvori pole a provede jeho inicializaci.
  @param width Sirka pole.
  @param height Vyska pole.
  @return Herni plocha, nebo NULL v pripade chyby*/
PlayArea pa_create(int width,int height) {
  PlayArea res;
  
  res=(PlayArea)malloc(sizeof(PlayAreaS));

  if (res==NULL) return NULL;

  res->data=(unsigned char*)malloc(sizeof(unsigned char *)*width*height);
  if (res->data==NULL) {
    free(res);

    return NULL;
  }
  
  res->width=width;
  res->height=height;

  pa_init(res);

  return res;
}

/**Uvolneni herni plochy.
  @param pa Herni plocha.
*/
void pa_free(PlayArea pa) {
  if (pa!=NULL) {
    if (pa->data!=NULL) free(pa->data);
    free(pa);
  }
}

/**Nastaveni polozky, ktera se nachazi na souradnicich x,y. Tato funkce neprovadi prekresleni.
  @param pa Hraci plocha.
  @param x X-ova souradnice.
  @param y Y-ova souradnice.
  @param item Polozka. Viz PA_GRASS, PA_FOOD, ...
*/
void pa_setitem(PlayArea pa,int x,int y,unsigned char item) {
  pa->data[pa->width*y+x]=item;
}

/**Ziskani polozky hraci plochy, ktera lezi na souradnicich x,y.
  @param pa Hraci plocha.
  @param x X-ova souradnice.
  @param y Y-ova souradnice.
  @return Polozka PA_GRASS, PA_FOOD, ...
*/
unsigned char pa_getitem(PlayArea pa,int x,int y) {
  return pa->data[pa->width*y+x];
}

/**Inicializace housenky.
  @return Ukazatel na strukturu housenky, nebo pri chybe NULL.
*/
Worm worm_init() {
  Worm res;

  res=(Worm)malloc(sizeof(WormS));
  if (res==NULL) return NULL;

  res->head=res->tail=NULL;

  res->length=0;
  
  return res;
}

/**Pridani nove hlavicky housenky. 
  @param worm Housenka.
  @param x X-ova souradnice nove hlavicky.
  @param y Y-ova souradnice nove hlavicky.
  @return FALSE pri chybe, jinak TRUE.
*/
int worm_addhead(Worm worm,int x,int y) {
  WormItem tmp;

  tmp=(WormItem)malloc(sizeof(WormItemS));
  if (tmp==NULL) return FALSE;

  tmp->x=x;
  tmp->y=y;
  tmp->prev=worm->head;
  tmp->next=NULL;

  if (worm->head!=NULL) {
    worm->head->next=tmp;
  }
  
  worm->head=tmp;

  if (worm->tail==NULL)
    worm->tail=tmp;


  worm->length++;
  
  return TRUE;
}

/**Umazani konecku housenky.
  @param worm Housenka
  @return FALSE pri chybe, jinak TRUE.
*/
int worm_deltail(Worm worm) {
  WormItem tmp;

  if (worm->tail!=NULL) {
    tmp=worm->tail->next;
    free(worm->tail);

    if (tmp!=NULL) tmp->prev=NULL;

    worm->tail=tmp;
    worm->length--;
  } else
    return FALSE;

  return TRUE;
}

/**Smazani celeho obsahu housenky (vsech clanku).
  @param worm Housenka.
*/
void worm_delete(Worm worm) {
  WormItem tmp,tmp2;

  if (worm!=NULL) {
    tmp=worm->tail;

    while (tmp!=NULL) {
      tmp2=tmp;
      tmp=tmp->prev;
      free(tmp2);
    }

    worm->head=worm->tail=NULL;
  }
}

/**Uvolneni struktury housenky a smazani obsahu.
  @param worm Housenka.
*/
void worm_free(Worm worm) {
  if (worm!=NULL) {
    worm_delete(worm);
    free(worm);
  }
}

/**Vlozeni polozky do hraci plochy na nahodnou pozici.
  @param playArea Hraci plocha.
  @param item Polozka, ktera se ma pridat.
  @param x X-ova souradnice, kam byla polozka pridana.
  @param y Y-ova souradnice, kam byla polozka pridana.
  @return TRUE, pokud se polozku podarilo umistit, jinak FALSE (znamena, ze je cela hraci plocha jiz zaplnena)
*/
int pa_add_generated_item(PlayArea playArea,int item,int *x,int *y) {
  int ix,iy;
  int cycle;
  
  ix=rand()%(PA_WIDTH);
  iy=rand()%(PA_HEIGHT);

  cycle=0;
    
  while (pa_getitem(playArea,ix,iy)!=PA_GRASS && cycle<PA_WIDTH*PA_HEIGHT) {
    ix++;

    if (ix>=PA_WIDTH) {
      ix=0;
      iy++;
    }

    if (iy>=PA_HEIGHT) {
      iy=0;
    }
      
    cycle++;
  }


  if (cycle>=PA_WIDTH*PA_HEIGHT) return FALSE;
  
  pa_setitem(playArea,ix,iy,item);

  *x=ix;
  *y=iy;
  
  return TRUE;
}

/**Vygeneruje novy level.
  @param level Cislo levelu.
*/
void pa_generatelevel(int level) {
  int no_food,no_toxic;
  int wsx,wsy;
  int i;
  int ix,iy;
  
  no_food=PA_NO_FOOD_BASE+PA_NO_FOOD_LVL*level;
  no_toxic=PA_NO_TOXIC_BASE+PA_NO_TOXIC_LVL*level;
  
  worm_delete(gameStatus.worm);
  pa_init(gameStatus.playArea);

  gameStatus.wormDir=WORM_DIR_DOWN;

  wsx=(PA_WIDTH/2);
  wsy=(PA_HEIGHT-WORM_START_SIZE)/2;

  for (i=0;i<WORM_START_SIZE;i++) {
    worm_addhead(gameStatus.worm,wsx,wsy+i);
    pa_setitem(gameStatus.playArea,wsx,wsy+i,(i==WORM_START_SIZE-1?PA_HEAD:PA_BODY));
  }

  for (i=0;i<no_food;i++) {
    pa_add_generated_item(gameStatus.playArea,PA_FOOD,&ix,&iy);
  }
  for (i=0;i<no_toxic;i++) {
    pa_add_generated_item(gameStatus.playArea,PA_TOXIC,&ix,&iy);
  }
}


/**Zobrazeni jednoho policka hraci plochy.
  @param tile Kterou pixmapu zobrazit.
  @param dSX X-ova souradnice plochy, kam ma byt zobrazena pixmapa.
  @param dSY Y-ova souradnice plochy, kam ma byt zobrazena pixmapa.
  @param size Velikost pixmap (SIZE_NORMAL, SIZE_DOUBLE).
*/
void pa_show_tile(unsigned char tile,Position dSX,Position dSY,int size) {
  Pixmap showPixmap;

  showPixmap=(size==SIZE_NORMAL?gameStatus.new_pixmaps_10[tile]:gameStatus.new_pixmaps_20[tile]);

  XCopyArea(gameStatus.display,showPixmap,XtWindow(gameStatus.drawArea),gameStatus.gc,0,0,
        (size+1)*PIXMAP_WIDTH,(size+1)*PIXMAP_HEIGHT, dSX,dSY );
}

/**Vypocita a zaobrazi jedno policko hraci plochy.
  @param x X-ova souradnice policka.
  @param y Y-ova souradnice policka.
  @param tile Kterou pixmapu zobrazit.
*/
void pa_compute_and_show_tile(int x,int y,unsigned char tile) {
  int size;
  Dimension paWidth,paHeight;
  Position paX,paY;

  if (gameStatus.width>=PA_WIDTH*PIXMAP_WIDTH*2 && gameStatus.height>=PA_HEIGHT*PIXMAP_HEIGHT*2) {
    size=1;
  } else {
    size=0;
  }

  paWidth=(size+1)*PIXMAP_WIDTH*PA_WIDTH;
  paHeight=(size+1)*PIXMAP_HEIGHT*PA_HEIGHT;

  paX=(int)((gameStatus.width-paWidth)/2.0+x*PIXMAP_WIDTH*(size+1));
  paY=(int)((gameStatus.height-paHeight)/2.0+y*PIXMAP_HEIGHT*(size+1));

  pa_show_tile(tile,paX,paY,size);
}

/*Prekresleni obrazku. Kompletne vykresli celou hraci plochu. Vyuziva fci. pa_show_tile*/
void redraw_image(void) {
  Dimension paWidth,paHeight;
  Position paSX,paSY,paX,paY;
  int x,y;
  int size;
  
  /*Vypocet vysky a sirky toho, co zobrazit*/
  if (gameStatus.width>=PA_WIDTH*PIXMAP_WIDTH*2 && gameStatus.height>=PA_HEIGHT*PIXMAP_HEIGHT*2) {
    size=1;
  } else {
    size=0;
  }
  
  paWidth=(size+1)*PIXMAP_WIDTH*PA_WIDTH;
  paHeight=(size+1)*PIXMAP_HEIGHT*PA_HEIGHT;
  
  paSX=(int)((gameStatus.width-paWidth)/2.0);
  paSY=(int)((gameStatus.height-paHeight)/2.0);
  
  paX=paSX;
  paY=paSY;
  
  for (y=0;y<PA_HEIGHT;y++) {
    for (x=0;x<PA_WIDTH;x++) {
      pa_show_tile(pa_getitem(gameStatus.playArea,x,y),paX,paY,size);
      paX+=PIXMAP_WIDTH*(size+1);
    }
    paY+=PIXMAP_HEIGHT*(size+1);
    paX=paSX;
  }
}

/**Call back pro udalost expose. Zavola fci redraw_image*/
void expose_call(Widget draw, XtPointer client_data, XtPointer callData) {
  static Boolean first=True;
  XSetWindowAttributes attr;
  
  if (first) {
    /*Nastavime gravity bit. -> nepotrebujeme explicitne volat prekreslovani
      pri zmene velikost, udalost je volana sama.*/
    first=False;
    attr.bit_gravity = ForgetGravity;
    XChangeWindowAttributes(XtDisplay(draw),XtWindow(draw), CWBitGravity, &attr);
  }
  
  redraw_image();  
}

/*Call back pro resize drawArea. Ulozi novou velikost do struktury gameStatus a vyvola udalost expose*/
void resize_call(Widget draw, XtPointer client_data, XtPointer call_data) {
  if (XtIsRealized(draw)) {
    /* get new window size. */
    XtVaGetValues(draw,XmNwidth,&gameStatus.width,XmNheight,&gameStatus.height,NULL);
/* Neni potraba diky gravity bitu*/
/*    XClearArea(XtDisplay(draw), XtWindow(draw), 0, 0, 0, 0, True);*/
  }
}

/*Zpetne volani pro klavesnice*/
void keyboard_action(Widget w, XButtonEvent *event, String *args, Cardinal *num_args) {
  if (strcmp(args[0], "up") == 0) {
    gameStatus.wormDir=WORM_DIR_UP;
  } else if (strcmp(args[0], "left") == 0) {
    gameStatus.wormDir=WORM_DIR_LEFT;
  } else if (strcmp(args[0], "right") == 0) {
    gameStatus.wormDir=WORM_DIR_RIGHT;
  } else if (strcmp(args[0],"down")==0) {
    gameStatus.wormDir=WORM_DIR_DOWN;
  }
}

/*Zpetne volani kliknuti mysi*/
void mouse_action(Widget w, XButtonEvent *event, String *args, Cardinal *num_args) {
  signed int newdir;

  newdir=gameStatus.wormDir;
  
  if (strcmp(args[0], "btn1") == 0) {
    switch (gameStatus.wormDir) {
      case WORM_DIR_UP:newdir=WORM_DIR_LEFT;break;
      case WORM_DIR_DOWN:newdir=WORM_DIR_LEFT;break;
      case WORM_DIR_LEFT:newdir=WORM_DIR_UP;break;
      case WORM_DIR_RIGHT:newdir=WORM_DIR_UP;break;
    }
  } else if (strcmp(args[0], "btn3") == 0) {
    switch (gameStatus.wormDir) {
      case WORM_DIR_UP:newdir=WORM_DIR_RIGHT;break;
      case WORM_DIR_DOWN:newdir=WORM_DIR_RIGHT;break;
      case WORM_DIR_LEFT:newdir=WORM_DIR_DOWN;break;
      case WORM_DIR_RIGHT:newdir=WORM_DIR_DOWN;break;
    }
  }

  if (newdir<WORM_DIR_UP) {
    newdir=WORM_DIR_RIGHT;
  }
  if (newdir>WORM_DIR_RIGHT) {
    newdir=WORM_DIR_UP;
  }

  gameStatus.wormDir=newdir;
}

/**Ukonceni hry a zobrazeni informace, proc hra zkoncila (zakousnuti do jedu, tela, ...)*/
void game_end(char *message) {
  showMessage(gameStatus.drawArea,message,XmDIALOG_INFORMATION);

  gameStatus.gameRunning=FALSE;
  gameStatus.timer=0;
}

/*Callback pro casovac. Hlavni jadro simulace zivota housenky*/
void my_looping_timer(XtPointer client_data, XtIntervalId *id) {
  int x,y,oldX,oldY;
  unsigned char item;
  
  if (gameStatus.gameRunning) {
    /*Hra stale bezi*/
    if (gameStatus.worm->head!=NULL) {
      /*Zacatek housenky*/
      x=gameStatus.worm->head->x;
      y=gameStatus.worm->head->y;
      oldX=x;oldY=y;
      
      /*Vypocet nove pozice*/
      switch (gameStatus.wormDir) {
        case WORM_DIR_UP:y--;break;
        case WORM_DIR_DOWN:y++;break;
        case WORM_DIR_LEFT:x--;break;
        case WORM_DIR_RIGHT:x++;break;
      }
      
      if (x<0 || x>=PA_WIDTH || y<0 || y>=PA_HEIGHT) {
        /*Hrac narazil do steny*/
        game_end("You crashed wall and die!");
        return ;
      }

      item=pa_getitem(gameStatus.playArea,x,y);

      if (item==PA_TOXIC) {
        /*Hrac snedl jed*/
        game_end("You eat toxic and die!");
        return ;
      }
    
      if (item==PA_BODY || item==PA_HEAD) {
        /*Hrac snedl sam sebe*/
        game_end("You eat yourself! Taste good.");
        return ;
      }
      
      if (gameStatus.worm->length>1) {
        /*Puvodni hlavicku nahradime telem*/
        pa_setitem(gameStatus.playArea,oldX,oldY,PA_BODY);
        pa_compute_and_show_tile(oldX,oldY,PA_BODY);
      }
      /*Pridame novou hlavicku -> housenka povyroste*/
      worm_addhead(gameStatus.worm,x,y);
      pa_compute_and_show_tile(x,y,PA_HEAD);
      pa_setitem(gameStatus.playArea,x,y,PA_HEAD);

      if (item==PA_GRASS) {
        /*Pokud je na novem miste trava, housenka se zase zmensi o konecek -> simulujeme pohyb*/
        x=gameStatus.worm->tail->x;
        y=gameStatus.worm->tail->y;
        
        worm_deltail(gameStatus.worm);
        pa_setitem(gameStatus.playArea,x,y,PA_GRASS);
        pa_compute_and_show_tile(x,y,PA_GRASS);
      }
      
      if (item==PA_FOOD) {
        if (gameStatus.worm->length%(LEVEL_MAX+1-gameStatus.level)==0) {
          /*Zkusime pridat jed*/
          if (pa_add_generated_item(gameStatus.playArea,PA_TOXIC,&x,&y)) {
            pa_compute_and_show_tile(x,y,PA_TOXIC);
          }
        }

        /*Pridame jedno jidlo*/
        if (pa_add_generated_item(gameStatus.playArea,PA_FOOD,&x,&y)) {
          pa_compute_and_show_tile(x,y,PA_FOOD);
        }
        
        if (gameStatus.worm->length%(PA_WIDTH*PA_HEIGHT/(SPEED_MAX+1)/4)==0) {
	  if (gameStatus.speed<SPEED_MAX && gameStatus.level>LEVEL_MAX/2) {
            /*Zvysime rychlost*/
            gameStatus.speed++;
          }
        }
      }
      
      /*Znovuvytvorime casovac. Stary s timto volanim zanikne*/
      gameStatus.timer=XtAppAddTimeOut(gameStatus.app_context,
          (unsigned long)SPEED_BASE-(SPEED_DIFF*gameStatus.speed)+1,
          my_looping_timer,client_data) ;
    } else {
      fprintf(stderr,"Critical error in %s:%d. This could never happend!!!",__FILE__,__LINE__);
    }
  } else {
    gameStatus.timer=0;
  }
}

/*Zapocneme novou hru*/
void start_new_game(void) {
  if (gameStatus.timer!=0) {
    XtRemoveTimeOut(gameStatus.timer);
  }

  gameStatus.gameRunning=TRUE;
  pa_generatelevel(gameStatus.startLevel);
  gameStatus.level=gameStatus.startLevel;
  gameStatus.speed=gameStatus.startSpeed;
  redraw_image();
 
  /*Pred zacatkem simulace dame hraci 2 sekundy na rozkoukani, pripadne urceni noveho smeru*/
  gameStatus.timer=XtAppAddTimeOut(gameStatus.app_context,(unsigned long) 2000,my_looping_timer, NULL) ;
}

/*Call back pro polozky z menu game*/
void game_call(Widget w, XtPointer data, XtPointer dummy) {
  switch ((int)data) {
    case 0:
      start_new_game();
    break;

    case 1:
      exit(0);
    break;

    default:
      fprintf(stderr,"game_call: Unknown item %d. This could never happend!\n",(int)data);
    break;
  }
}

/*Call back pro polozky z options. Nemelo by nastat*/
void options_call(Widget w, XtPointer data, XtPointer dummy) {
  fprintf(stderr,"options_call: This could never happend!\n");
}

/*Nastaveni startovniho levelu*/
void options_level_call(Widget w, XtPointer data, XtPointer dummy) {
  gameStatus.startLevel=(int)data;
}

/*Nastaveni startovni rychlosti*/
void options_speed_call(Widget w, XtPointer data, XtPointer dummy) {
  gameStatus.startSpeed=(int)data;
}

/*Vytvoreni podmenu options*/
void create_menu_options(Widget menuBar) {
  XmString levels,speeds;
  XmString nums[10];
  int i;
  char tmpBuf[4];
  
  Widget optionsMenu,optionsLevelMenu,optionsSpeedMenu;
  
  levels = XmStringCreateLocalized("Level");
  speeds = XmStringCreateLocalized("Speed");

  for (i=0;i<10;i++) {
    snprintf(tmpBuf,4,"%d",i+1);
        
    nums[i]=XmStringCreateLocalized(tmpBuf);
  }
  
  optionsMenu=XmVaCreateSimplePulldownMenu(menuBar, "options_menu", 1, options_call,
    XmVaCASCADEBUTTON, levels, 'L', 
    XmVaCASCADEBUTTON, speeds, 'S', 
    NULL);


  optionsLevelMenu=XmVaCreateSimplePulldownMenu(optionsMenu, "options_menu_level", 0, options_level_call,
        XmVaRADIOBUTTON, nums[0], '1', NULL, NULL,
        XmVaRADIOBUTTON, nums[1], '2', NULL, NULL,
        XmVaRADIOBUTTON, nums[2], '3', NULL, NULL,
        XmVaRADIOBUTTON, nums[3], '4', NULL, NULL,
        XmVaRADIOBUTTON, nums[4], '5', NULL, NULL,
        XmVaRADIOBUTTON, nums[5], '6', NULL, NULL,
        XmVaRADIOBUTTON, nums[6], '7', NULL, NULL,
        XmVaRADIOBUTTON, nums[7], '8', NULL, NULL,
        XmVaRADIOBUTTON, nums[8], '9', NULL, NULL,
        XmVaRADIOBUTTON, nums[9], '0', NULL, NULL,
        XmNradioBehavior, True,     
        XmNradioAlwaysOne, True,    
        NULL);                      
  XmToggleButtonSetState(XtNameToWidget(optionsLevelMenu, "button_0"), True, False);

  optionsSpeedMenu=XmVaCreateSimplePulldownMenu(optionsMenu, "options_menu_speed", 1, options_speed_call,
        XmVaRADIOBUTTON, nums[0], '1', NULL, NULL,
        XmVaRADIOBUTTON, nums[1], '2', NULL, NULL,
        XmVaRADIOBUTTON, nums[2], '3', NULL, NULL,
        XmVaRADIOBUTTON, nums[3], '4', NULL, NULL,
        XmVaRADIOBUTTON, nums[4], '5', NULL, NULL,
        XmVaRADIOBUTTON, nums[5], '6', NULL, NULL,
        XmVaRADIOBUTTON, nums[6], '7', NULL, NULL,
        XmVaRADIOBUTTON, nums[7], '8', NULL, NULL,
        XmVaRADIOBUTTON, nums[8], '9', NULL, NULL,
        XmVaRADIOBUTTON, nums[9], '0', NULL, NULL,
        XmNradioBehavior, True,     
        XmNradioAlwaysOne, True,    
        NULL);                      
  XmToggleButtonSetState(XtNameToWidget(optionsSpeedMenu, "button_0"), True, False);
  
  for (i=0;i<10;i++) {
    XmStringFree(nums[i]);
  }

  XmStringFree(levels);
  XmStringFree(speeds);
}

/*Vytvoreni menu pro game*/
void create_menu_game(Widget menuBar) {
  XmString quits,news;

  quits = XmStringCreateLocalized("Quit");
  news = XmStringCreateLocalized("New");

  XmVaCreateSimplePulldownMenu(menuBar, "file_menu", 0, game_call,
    XmVaPUSHBUTTON, news, 'N', NULL, NULL,
    XmVaPUSHBUTTON, quits, 'Q', NULL, NULL,
    NULL);

  XmStringFree(quits);
  XmStringFree(news);
}

/*Vytvoreni vrchniho menu*/
Widget create_menu(Widget mainWin) {
  XmString games,optionss;
  Widget menuBar;
  
  games = XmStringCreateLocalized("Game");
  optionss = XmStringCreateLocalized("Options");
  
  menuBar = XmVaCreateSimpleMenuBar(mainWin, "main_list",
        XmVaCASCADEBUTTON, games, 'G',
        XmVaCASCADEBUTTON, optionss, 'O',
        NULL); 
               
  XtManageChild(menuBar);

  create_menu_game(menuBar);
  create_menu_options(menuBar);

  XmStringFree(games);
  XmStringFree(optionss);

  return menuBar;
}

/*Inicializace pixmap*/
int init_pixmaps(void) {
  int i,size;
  char **identificator=NULL;
  
  for (size=0;size<2;size++) {
    for (i=0;i<NO_PIXMAPS;i++) {
      switch(i) {
        case PA_GRASS:
          identificator=(size==SIZE_NORMAL?grass_10:grass_20);
        break;
        case PA_FOOD:
          identificator=(size==SIZE_NORMAL?food_10:food_20);
        break;
        case PA_TOXIC:
          identificator=(size==SIZE_NORMAL?toxic_10:toxic_20);
        break;
        case PA_HEAD:
          identificator=(size==SIZE_NORMAL?head_10:head_20);
        break;
        case PA_BODY:
          identificator=(size==SIZE_NORMAL?body_10:body_20);
        break;
      }

      if (identificator==NULL) {
        fprintf(stderr,"Internal program error in %s:%d\n",__FILE__,__LINE__);

        return FALSE;
      }
      
      XpmCreatePixmapFromData(gameStatus.display, RootWindowOfScreen(gameStatus.screen),identificator,
           (size==0?&gameStatus.new_pixmaps_10[i]:&gameStatus.new_pixmaps_20[i]), NULL, NULL) ;
    }
  }

  return TRUE;
}

/*Inicializace hry -> inicializace gameStatus, vytvoreni pixmap + inicializace generatoru nahodnych cisel*/
void init_game(Widget drawArea) {
  gameStatus.drawArea=drawArea;
  gameStatus.display=XtDisplay(drawArea);
  gameStatus.screen=XtScreen(drawArea);
  gameStatus.cmap=DefaultColormapOfScreen(gameStatus.screen);
  gameStatus.width=PA_WIDTH*PIXMAP_WIDTH;
  gameStatus.height=PA_HEIGHT*PIXMAP_HEIGHT;
  gameStatus.gameRunning=FALSE;
  gameStatus.startLevel=gameStatus.level=0;
  gameStatus.startSpeed=gameStatus.speed=0;
  gameStatus.timer=0;

  /*Nastavime gc pro drawArea*/
  gameStatus.gcv.foreground = BlackPixelOfScreen(gameStatus.screen);
  gameStatus.gc = XCreateGC(gameStatus.display,RootWindowOfScreen(gameStatus.screen), GCForeground, &(gameStatus.gcv));

  init_pixmaps();

  gameStatus.playArea=pa_create(PA_WIDTH,PA_HEIGHT);
  gameStatus.worm=worm_init();
  
  srand(time(NULL));
}

/*Vstupni fce. programu*/
int main(int argc, char **argv) {
  XtActionsRec actions;
  Widget menuBar; 
  Dimension winWidth,winHeight;
  
  /*Tabulka udalosti pro drawArea*/
  String translations =
        "<Key>osfUp: keyboard_action(up) \n\
        <Key>osfDown: keyboard_action(down) \n\
        <Key>osfLeft: keyboard_action(left) \n\
	<Key>osfRight: keyboard_action(right) \n\
        <Key>j: keyboard_action(left) \n\
        <Key>k: keyboard_action(down) \n\
        <Key>l: keyboard_action(right) \n\
	<Key>i: keyboard_action(up) \n\
        <Btn1Down>: mouse_action(btn1) \n\
        <Btn3Down>: mouse_action(btn3)";

  Widget topLevel,mainWin,drawArea; 

  /*Nastavime standardni jazyk (dle LC_ALL)*/
  XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

  /*Inicializace aplikace*/
  topLevel = XtVaAppInitialize(&gameStatus.app_context,"NWurmy85",NULL,0,&argc, argv,/*fallback_res*/NULL,NULL);

  /*Hlavni okno aplikace*/
  mainWin = XtVaCreateManagedWidget("main_win",xmMainWindowWidgetClass,topLevel,
      XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE,NULL);

  /*Nastaveni akci pro mys a klavesnici*/
  actions.string = "mouse_action";
  actions.proc = (XtActionProc)mouse_action;
  XtAppAddActions(gameStatus.app_context, &actions, 1);
  actions.string = "keyboard_action";
  actions.proc = (XtActionProc)keyboard_action;
  XtAppAddActions(gameStatus.app_context, &actions, 1);
  
  /*Kreslici plocha*/
  drawArea = XtVaCreateManagedWidget(
    "play_area",			/* widget name */
    xmDrawingAreaWidgetClass,		/* widget class */
    mainWin,				/* parent widget*/
    XmNwidth, PIXMAP_WIDTH*PA_WIDTH,			/* set startup width */
    XmNheight, PIXMAP_HEIGHT*PA_HEIGHT,			/* set startup height */
    XmNbackground,BlackPixelOfScreen(XtScreen(mainWin)), /*Pozadi*/
    NULL);				/* terminate varargs list */

  /*Prepiseme translacni tabulku*/
  XtOverrideTranslations (drawArea, XtParseTranslationTable (translations)); 
  
  /*Zinicializujeme globalni promenne*/
  init_game(drawArea);

  /*Vytvorime menu*/
  menuBar=create_menu(mainWin);

  /*Nastavime kreslici plochu, jako "pracovni plochu" hlavniho okna*/
  XtVaSetValues(mainWin,XmNmenuBar,menuBar,
        XmNworkWindow, drawArea,
        NULL);

  /*A zaregistrujeme si odber udalosti*/
  XtAddCallback(drawArea, XmNexposeCallback, expose_call, NULL);
  XtAddCallback(drawArea, XmNresizeCallback, resize_call, NULL);
    
  /*Zrealizujeme hlavni okno*/
  XtRealizeWidget(topLevel);

  /*Nastavime minimalni velikost okna*/
  XtVaGetValues(topLevel,XmNwidth,&winWidth,XmNheight, &winHeight,NULL);
  XtVaSetValues(topLevel,XmNminWidth,winWidth,XmNminHeight,winHeight,NULL);
  
  /*A nechame aplikaci bezet*/
  XtAppMainLoop(gameStatus.app_context);

  return 0;
}
