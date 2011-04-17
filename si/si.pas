Program SpaceInvaders;
{(c) 2001 Jan Friesse (janf@email.cz)

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
}
Uses SI_U_1,SI_U_2,SI_U_3,Crt,Dos;

Var AL1:Palien1;
    AL2:Palien2;
    AL3:Palien3;
    Aliens:Array[0..54] of record
      ALien:Pointer;
      Typ:Integer;
    End;
    AlX,Aly:Integer;
    Alien:TAlien;
    DX,DY,X,y:Integer;
    H,M,S,Sec100:Word;
    NewTime,OldTime,DTime:Longint;
    Key:Word;
    CH:Char;
    Player:PPlayer;
    Shot:PShot;
    Jekonec:Boolean;
    Lives,Score:Integer;
    Level:Integer;
    PomS:String;

Function TestLostLife:Boolean;
Var X,Y:Integer;
    JeTam:Boolean;
Begin
  X:=-1;
  Repeat
    X:=X+1;
    JeTam:=False;
    For Y:=0 to 10 do
      If Aliens[X*11+Y].Alien<>Nil then JeTam:=True;
    If Jetam=False Then
    Begin
{      Writeln;}
    End;
  Until JeTam;
  If Aly>130+X*10 then TestLostLife:=True else TestLostLife:=False;
End;

Begin
  randomize;
  GraphOn;
  WriteText(90,20,1,'SPACE INVADERS');
  WriteText(10,40,5,'THIS PROGRAM IS UNDER GNU/GPL LICENCE');
  WriteText(1,60,15,'Original by Taito/Midway in 1978');
  WriteText(1,68,15,'Author : Toshihiro Nishikado');
  WriteText(1,76,15,'Original name : Space Invaders Arcade');
  WriteText(1,100,14,'Remake by Jan Friesse (janf@email.cz)');
  WriteText(1,108,14,'in 2001. You can download new version');
  WriteText(1,116,14,'from http://www.hyperlink.cz');
  WriteText(1,124,14,'Version 0.5');
  WriteText(1,140,2,'This version is unfinished. Here is not');
  WriteText(1,148,2,'bunkers and alien can''t shot.');
  WriteText(1,160,3,'Keys : 4 - Left, 6 - Right, 5 - Fire');
  WriteText(1,180,2,'Press any key to continue...');
  If readkey=#0 then Readkey;
{  WriteSpec(10,10,'Abba');
  Readln;
  Graphoff;
  Halt(1); }
  {Game}
  {Generate aliens}
  Lives:=1;
  Score:=0;
  Level:=1;
  Repeat
  Str(Level,PomS);
  WriteText(50,50,3,'Prepare to level : '+Poms);
  Delay(1000);
  For x:=0 to 188 do FillChar(Mem[$A000:X*320],320,0);
  AlX:=0;
  aly:=0;
  For Y:=0 to 4 do
  For X:=0 to 10 do
  Begin
    if y=4 then
    Begin
      New(AL3,Create(X*15,40-Y*10,15));
      Aliens[Y*11+x].Alien:=Al3;
      Aliens[y*11+x].Typ:=Alien3;
    End;
    if (y=3) or (y=2) then
    Begin
      New(AL2,Create(X*15,40-Y*10,15));
      Aliens[Y*11+x].Alien:=Al2;
      Aliens[y*11+x].Typ:=Alien2;
    End;
    if (y=1) or (y=0) then
    Begin
      New(AL1,Create(X*15,40-Y*10,15));
      Aliens[Y*11+x].Alien:=Al1;
      Aliens[y*11+x].Typ:=Alien1;
    End;
  End;
  For X:=0 to 54 do PAlien(Aliens[x].Alien)^.Show;
  {And player}
  New(PLayer,Create(1,180,15));
  {Create down line}
  For X:=0 to 319 do PutPixel(X,189,1);
  PrintInfoBar(Lives,Score);
  Player^.Show;
  Shot:=Nil;
  {OK}
  GetTime(H,M,S,Sec100);
  NewTime:=Sec100+S*100+M*100*60+H*100*60*60;
  OldTime:=NewTime;
  DTIme:=10-Level;
  If DTime<1 then DTime:=1;
  DX:=(Level div 2)+1;
  dy:=(Level div 4)+1;
  Repeat
    GetTime(H,M,S,Sec100);
    NewTime:=Sec100+S*100+M*100*60+H*100*60*60;
    If abs(OldTime-NewTime)>DTime then
    Begin
      OldTime:=NewTime;
      If ALX>167-8-dx then
      begin
        ALy:=ALY+dy;
        DX:=0-DX;
        For X:=0 to 54 do
        Begin
          If Aliens[x].Alien<>NIl then
          Begin
            PAlien(Aliens[x].Alien)^.Hide;
            PAlien(Aliens[x].Alien)^.y:=PAlien(Aliens[x].Alien)^.y+dy;
            PAlien(Aliens[x].Alien)^.Show;
          End;
        End;
      End;
      If ALX<0-dx then
      begin
        ALy:=ALY+dy;
        DX:=0-DX;
        For X:=0 to 54 do
        Begin
          If Aliens[x].Alien<>NIl then
          Begin
            PAlien(Aliens[x].Alien)^.Hide;
            PAlien(Aliens[x].Alien)^.y:=PAlien(Aliens[x].Alien)^.y+dy;
            PAlien(Aliens[x].Alien)^.Show;
          End;
        End;
      end;
      ALX:=ALX+DX;
      If DX>0 then
      For X:=54 downto 0 do
      Begin
        If Aliens[x].Alien<>NIl then
        Begin
          PAlien(Aliens[x].Alien)^.Hide;
          PAlien(Aliens[x].Alien)^.X:=PAlien(Aliens[x].Alien)^.X+dx;
          PAlien(Aliens[x].Alien)^.Show;
        End;
      End
      else
      For X:=0 to 54 do
      Begin
        If Aliens[x].Alien<>NIl then
        Begin
          PAlien(Aliens[x].Alien)^.Hide;
          PAlien(Aliens[x].Alien)^.X:=PAlien(Aliens[x].Alien)^.X+dx;
          PAlien(Aliens[x].Alien)^.Show;
        End;
      End;

    End;
    Key:=0;
    If KeyPressed then
    Begin
      ch:=Readkey;
      If ch=#0 then Begin Key:=256;Ch:=ReadKey;End;
      Key:=Ord(ch)+Key;
    End;
    If Key=54 then
    Begin
      Player^.Hide;
      Player^.X:=Player^.X+6;
      Player^.Show;
    End;
    If Key=52 then
    Begin
      Player^.Hide;
      Player^.X:=Player^.X-6;
      Player^.Show;
    End;
    If Key=53 then
    Begin
      If Shot=Nil Then
      Begin
        New(Shot,Create(player^.X+3,Player^.Y-4,1));
        Shot^.Show;
      End;
    End;
    If Shot<>Nil Then
    Begin
      If Abs(NewTime-Shot^.OldTime)>Shot^.DTime then
      Begin
        Shot^.OldTime:=NewTime;
        For X:=0 to 54 do
        Begin
          If Aliens[x].Alien<>NIl then
          Begin
            If ((Shot^.X>=PAlien(Aliens[x].Alien)^.X) and (Shot^.X+1<=PAlien(Aliens[x].Alien)^.X+8)) and
               ((Shot^.y>=PAlien(Aliens[x].Alien)^.y) and (Shot^.y+2<=PAlien(Aliens[x].Alien)^.y+8))
            then
            begin
              Score:=Score+1;
              PrintInfoBar(Lives,Score);
              Dispose(Shot,Done);
              Shot:=Nil;
              Dispose(PALien(Aliens[x].Alien),Done);
              Aliens[x].Alien:=Nil;
              Break;
{              PAlien(Aliens[x].Alien)^.Hide;}
            End;
          End;
        End;
        If (Shot<>Nil)
        then
        begin
          Shot^.Hide;
          Shot^.Y:=Shot^.Y-6;
          Shot^.Show;
          For X:=0 to 54 do
          Begin
            If Aliens[x].Alien<>NIl then
            Begin
              If ((Shot^.X>=PAlien(Aliens[x].Alien)^.X) and (Shot^.X+1<=PAlien(Aliens[x].Alien)^.X+8)) and
                 ((Shot^.y>=PAlien(Aliens[x].Alien)^.y) and (Shot^.y+2<=PAlien(Aliens[x].Alien)^.y+8))
              then
              begin
                Score:=Score+1;
                PrintInfoBar(Lives,Score);
                Dispose(Shot,Done);
                Shot:=Nil;
                Dispose(PALien(Aliens[x].Alien),Done);
                Aliens[x].Alien:=Nil;
  {              PAlien(Aliens[x].Alien)^.Hide;}
                Break;
              End;
            End;
          End;
         If Shot^.Y<0 then
         Begin
            Dispose(Shot,Done);
            Shot:=Nil;
          End;
        End;
      End;
    End;
    JeKonec:=True;
    For X:=0 to 54 do if Aliens[x].Alien<>Nil then JeKOnec:=False;
    If JeKonec Then
    Begin
      WriteText(1,100,3,'Congratulation. You finish this level!');
      WriteText(40,116,3,'Press any key to continue...');
      If Readkey=#0 then Readkey;
      Level:=Level+1;
    End;
    If TestLostLife then
    Begin
      Lives:=Lives-1;
      WriteText(40,100,3,'You lose your life!');
      WriteText(40,116,3,'Press any key to continue...');
      If Readkey=#0 then Readkey;
{      Key:=27;}
    End;
    If Key=27 then
    Begin
    End;
    If (Score mod 200)=0 then begin Lives:=Lives+1;Score:=Score+1;PrintInfoBar(Lives,Score);End;
  Until (key=27) or (TestLostLife) or (JeKonec);
  For X:=0 to 54 do If Aliens[x].Alien<>NIl then PAlien(Aliens[x].Alien)^.Done;
  Until (Lives=-1) or (Key=27);
  Player^.Done;
  GraphOff;
End.
