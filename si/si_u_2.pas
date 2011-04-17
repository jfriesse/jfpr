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
Unit SI_U_2;

Interface

Const Alien3 = 3;
      Alien2 = 2;
      Alien1 = 1;

Procedure DrawEnemy(X,Y:Integer;Color,Typ:Byte);
Procedure ClearEnemy(X,Y:Integer);
Procedure DrawPlayer(X,y:Integer;C:Byte);
Procedure WriteSpec(X,y:Integer;S:String);
Procedure WriteChar(X,y:Integer;ch:char;Co:Byte);
procedure WriteText(Xpos,Ypos : word; Color : byte; ScrTxt : string);
Procedure PrintInfoBar(Lives,Score:Integer);

Implementation

Uses SI_U_1,Dos;

Var FontSegment,FontOfset:Word;

Procedure PrintInfoBar(Lives,Score:Integer);
Var X:Integer;
    S:String;
Begin
  For x:=190 to 199 do fillchar(Mem[$A000:X*320],320,0);
  For X:=1 to lives do DrawPlayer(X*9-8,194,1);
  WriteText(150,191,1,'SCORE : ');
  Str(Score,S);
  WriteText(206,191,1,s);
End;

procedure GetFont(var FontSeg,FontOfs : word);
var R : registers;
begin
  with R do begin
    AX := $1130;
    BH := 3;
    intr($10,R);
    FontSeg := ES;
    FontOfs := BP;
  end;
end;

procedure WriteText(Xpos,Ypos : word; Color : byte; ScrTxt : string);
var I,J,K : byte;
begin
  for I := 1 to length(ScrTxt) do
    for J := 0 to 7 do
      for K := 0 to 7 do
        if ((mem[FontSegment:FontOfset+ord(ScrTxt[I])*8+J] shl K) and 128) <> 0 then
          mem[$a000:(Ypos+J)*320+(I*8)+Xpos-8+K] := Color;
End;

Procedure WriteSpec(X,y:Integer;S:String);
Var I:Integer;
    C:Byte;
Begin
  For I:=1 to length(s) do
  begin
    C:=Random(15)+1;
    WriteChar(X+(I*5*8)-5*8,y,s[i],c);
  end;
End;

Procedure WriteChar(X,y:Integer;ch:char;Co:Byte);
Var C:Char;
    T:Integer;
Begin
  C:=UpCase(ch);
  If C='A' then
  Begin
    T:=Random(3)+1;
    DrawEnemy(X+1*8,Y+8*0,Co,T);
    DrawEnemy(X+2*8,Y+8*0,Co,T);
    DrawEnemy(X+0*8,Y+8*1,Co,T);
    DrawEnemy(X+3*8,Y+8*1,Co,T);
    DrawEnemy(X+0*8,Y+8*2,Co,T);
    DrawEnemy(X+1*8,Y+8*2,Co,T);
    DrawEnemy(X+2*8,Y+8*2,Co,T);
    DrawEnemy(X+3*8,Y+8*2,Co,T);
    DrawEnemy(X+0*8,Y+8*3,Co,T);
    DrawEnemy(X+3*8,Y+8*3,Co,T);
    DrawEnemy(X+0*8,Y+8*4,Co,T);
    DrawEnemy(X+3*8,Y+8*4,Co,T);
  End;
  If C='B' then
  Begin
    T:=Random(3)+1;
    DrawEnemy(X+0*8,Y+8*0,Co,T);
    DrawEnemy(X+1*8,Y+8*0,Co,T);
    DrawEnemy(X+2*8,Y+8*0,Co,T);
    DrawEnemy(X+0*8,Y+8*1,Co,T);
    DrawEnemy(X+3*8,Y+8*1,Co,T);
    DrawEnemy(X+0*8,Y+8*2,Co,T);
    DrawEnemy(X+1*8,Y+8*2,Co,T);
    DrawEnemy(X+2*8,Y+8*2,Co,T);
    DrawEnemy(X+0*8,Y+8*3,Co,T);
    DrawEnemy(X+3*8,Y+8*3,Co,T);
    DrawEnemy(X+0*8,Y+8*4,Co,T);
    DrawEnemy(X+1*8,Y+8*4,Co,T);
    DrawEnemy(X+2*8,Y+8*4,Co,T);
  End;
  If C='C' then
  Begin
    T:=Random(3)+1;
    DrawEnemy(X+1*8,Y+8*0,Co,T);
    DrawEnemy(X+2*8,Y+8*0,Co,T);
    DrawEnemy(X+0*8,Y+8*1,Co,T);
    DrawEnemy(X+3*8,Y+8*1,Co,T);
    DrawEnemy(X+0*8,Y+8*2,Co,T);
    DrawEnemy(X+0*8,Y+8*3,Co,T);
    DrawEnemy(X+3*8,Y+8*3,Co,T);
    DrawEnemy(X+1*8,Y+8*4,Co,T);
    DrawEnemy(X+2*8,Y+8*4,Co,T);
  End;
  If C='D' then
  Begin
    T:=Random(3)+1;
    DrawEnemy(X+0*8,Y+8*0,Co,T);
    DrawEnemy(X+1*8,Y+8*0,Co,T);
    DrawEnemy(X+2*8,Y+8*0,Co,T);
    DrawEnemy(X+0*8,Y+8*1,Co,T);
    DrawEnemy(X+3*8,Y+8*1,Co,T);
    DrawEnemy(X+0*8,Y+8*2,Co,T);
    DrawEnemy(X+3*8,Y+8*2,Co,T);
    DrawEnemy(X+0*8,Y+8*3,Co,T);
    DrawEnemy(X+3*8,Y+8*3,Co,T);
    DrawEnemy(X+0*8,Y+8*4,Co,T);
    DrawEnemy(X+1*8,Y+8*4,Co,T);
    DrawEnemy(X+2*8,Y+8*4,Co,T);
  End;
  If C='E' then
  Begin
    T:=Random(3)+1;
    DrawEnemy(X+0*8,Y+8*0,Co,T);
    DrawEnemy(X+1*8,Y+8*0,Co,T);
    DrawEnemy(X+2*8,Y+8*0,Co,T);
    DrawEnemy(X+3*8,Y+8*0,Co,T);
    DrawEnemy(X+0*8,Y+8*1,Co,T);
    DrawEnemy(X+0*8,Y+8*2,Co,T);
    DrawEnemy(X+1*8,Y+8*2,Co,T);
    DrawEnemy(X+2*8,Y+8*2,Co,T);
    DrawEnemy(X+0*8,Y+8*3,Co,T);
    DrawEnemy(X+0*8,Y+8*4,Co,T);
    DrawEnemy(X+1*8,Y+8*4,Co,T);
    DrawEnemy(X+2*8,Y+8*4,Co,T);
    DrawEnemy(X+3*8,Y+8*4,Co,T);
  End;
  If C='F' then
  Begin
    T:=Random(3)+1;
    DrawEnemy(X+0*8,Y+8*0,Co,T);
    DrawEnemy(X+1*8,Y+8*0,Co,T);
    DrawEnemy(X+2*8,Y+8*0,Co,T);
    DrawEnemy(X+3*8,Y+8*0,Co,T);
    DrawEnemy(X+0*8,Y+8*1,Co,T);
    DrawEnemy(X+0*8,Y+8*2,Co,T);
    DrawEnemy(X+1*8,Y+8*2,Co,T);
    DrawEnemy(X+2*8,Y+8*2,Co,T);
    DrawEnemy(X+0*8,Y+8*3,Co,T);
    DrawEnemy(X+0*8,Y+8*4,Co,T);
  End;

End;

Procedure DrawPlayer(X,y:Integer;C:Byte);
Begin
  FillChar(Mem[$A000:(Y+4)*320+X],8,c);
  FillChar(Mem[$A000:(Y+3)*320+X],8,c);
  FillChar(Mem[$A000:(Y+2)*320+X+1],6,c);
  FillChar(Mem[$A000:(Y+1)*320+X+3],2,c);
  FillChar(Mem[$A000:(Y+0)*320+X+3],2,c);
End;

Procedure ClearEnemy(X,Y:Integer);
Var I:Integer;
Begin
  For I:=0 to 7 do
    FillChar(Mem[$A000:(Y+i)*320+X],8,0);
End;

Procedure DrawEnemy(X,Y:Integer;Color,Typ:Byte);
Var Sprite:TSprite;
    AX,AY:Integer;
Begin
 If Typ=Alien3 then
 Begin
   Sprite[7,0]:=Color;Sprite[7,1]:=0;Sprite[7,2]:=Color;Sprite[7,3]:=0;
   Sprite[7,4]:=0;Sprite[7,5]:=Color;Sprite[7,6]:=0;Sprite[7,7]:=Color;
   Sprite[6,0]:=0;Sprite[6,1]:=color;Sprite[6,2]:=0;Sprite[6,3]:=color;
   Sprite[6,4]:=color;Sprite[6,5]:=0;Sprite[6,6]:=color;Sprite[6,7]:=0;
   Sprite[5,0]:=0;Sprite[5,1]:=0;Sprite[5,2]:=Color;Sprite[5,3]:=0;
   Sprite[5,4]:=0;Sprite[5,5]:=Color;Sprite[5,6]:=0;Sprite[5,7]:=0;
   Sprite[4,0]:=Color;Sprite[4,1]:=Color;Sprite[4,2]:=Color;Sprite[4,3]:=Color;
   Sprite[4,4]:=Color;Sprite[4,5]:=Color;Sprite[4,6]:=Color;Sprite[4,7]:=Color;
   Sprite[3,0]:=Color;Sprite[3,1]:=Color;Sprite[3,2]:=0;Sprite[3,3]:=Color;
   Sprite[3,4]:=Color;Sprite[3,5]:=0;Sprite[3,6]:=Color;Sprite[3,7]:=Color;
   Sprite[2,0]:=0;Sprite[2,1]:=Color;Sprite[2,2]:=Color;Sprite[2,3]:=Color;
   Sprite[2,4]:=Color;Sprite[2,5]:=Color;Sprite[2,6]:=Color;Sprite[2,7]:=0;
   Sprite[1,0]:=0;Sprite[1,1]:=0;Sprite[1,2]:=Color;Sprite[1,3]:=Color;
   Sprite[1,4]:=Color;Sprite[1,5]:=Color;Sprite[1,6]:=0;Sprite[1,7]:=0;
   Sprite[0,0]:=0;Sprite[0,1]:=0;Sprite[0,2]:=0;Sprite[0,3]:=Color;
   Sprite[0,4]:=Color;Sprite[0,5]:=0;Sprite[0,6]:=0;Sprite[0,7]:=0;
 End;
 If Typ=Alien2 then
 Begin
   Sprite[7,0]:=0;
   Sprite[7,1]:=Color;
   Sprite[7,2]:=Color;
   Sprite[7,3]:=0;
   Sprite[7,4]:=0;
   Sprite[7,5]:=Color;
   Sprite[7,6]:=Color;
   Sprite[7,7]:=0;
   Sprite[6,0]:=0;
   Sprite[6,1]:=color;
   Sprite[6,2]:=0;
   Sprite[6,3]:=0;
   Sprite[6,4]:=0;
   Sprite[6,5]:=0;
   Sprite[6,6]:=color;
   Sprite[6,7]:=0;
   Sprite[5,0]:=Color;
   Sprite[5,1]:=Color;
   Sprite[5,2]:=Color;
   Sprite[5,3]:=Color;
   Sprite[5,4]:=Color;
   Sprite[5,5]:=Color;
   Sprite[5,6]:=Color;
   Sprite[5,7]:=Color;
   Sprite[4,0]:=Color;
   Sprite[4,1]:=Color;
   Sprite[4,2]:=Color;
   Sprite[4,3]:=Color;
   Sprite[4,4]:=Color;
   Sprite[4,5]:=Color;
   Sprite[4,6]:=Color;
   Sprite[4,7]:=Color;
   Sprite[3,0]:=Color;
   Sprite[3,1]:=Color;
   Sprite[3,2]:=0;
   Sprite[3,3]:=Color;
   Sprite[3,4]:=Color;
   Sprite[3,5]:=0;
   Sprite[3,6]:=Color;
   Sprite[3,7]:=Color;
   Sprite[2,0]:=0;
   Sprite[2,1]:=Color;
   Sprite[2,2]:=Color;
   Sprite[2,3]:=Color;
   Sprite[2,4]:=Color;
   Sprite[2,5]:=Color;
   Sprite[2,6]:=Color;
   Sprite[2,7]:=0;
   Sprite[1,0]:=0;
   Sprite[1,1]:=0;
   Sprite[1,2]:=Color;
   Sprite[1,3]:=0;
   Sprite[1,4]:=0;
   Sprite[1,5]:=Color;
   Sprite[1,6]:=0;
   Sprite[1,7]:=0;
   Sprite[0,0]:=0;
   Sprite[0,1]:=Color;
   Sprite[0,2]:=0;
   Sprite[0,3]:=0;
   Sprite[0,4]:=0;
   Sprite[0,5]:=0;
   Sprite[0,6]:=Color;
   Sprite[0,7]:=0;
 End;
 If Typ=Alien1 then
 Begin
   Sprite[7,0]:=0;
   Sprite[7,1]:=Color;
   Sprite[7,2]:=Color;
   Sprite[7,3]:=0;
   Sprite[7,4]:=0;
   Sprite[7,5]:=Color;
   Sprite[7,6]:=Color;
   Sprite[7,7]:=0;
   Sprite[6,0]:=Color;
   Sprite[6,1]:=color;
   Sprite[6,2]:=0;
   Sprite[6,3]:=Color;
   Sprite[6,4]:=Color;
   Sprite[6,5]:=0;
   Sprite[6,6]:=color;
   Sprite[6,7]:=Color;
   Sprite[5,0]:=0;
   Sprite[5,1]:=Color;
   Sprite[5,2]:=0;
   Sprite[5,3]:=0;
   Sprite[5,4]:=0;
   Sprite[5,5]:=0;
   Sprite[5,6]:=Color;
   Sprite[5,7]:=0;
   Sprite[4,0]:=Color;
   Sprite[4,1]:=Color;
   Sprite[4,2]:=Color;
   Sprite[4,3]:=Color;
   Sprite[4,4]:=Color;
   Sprite[4,5]:=Color;
   Sprite[4,6]:=Color;
   Sprite[4,7]:=Color;
   Sprite[3,0]:=Color;
   Sprite[3,1]:=0;
   Sprite[3,2]:=0;
   Sprite[3,3]:=Color;
   Sprite[3,4]:=Color;
   Sprite[3,5]:=0;
   Sprite[3,6]:=0;
   Sprite[3,7]:=Color;
   Sprite[2,0]:=Color;
   Sprite[2,1]:=Color;
   Sprite[2,2]:=Color;
   Sprite[2,3]:=Color;
   Sprite[2,4]:=Color;
   Sprite[2,5]:=Color;
   Sprite[2,6]:=Color;
   Sprite[2,7]:=Color;
   Sprite[1,0]:=0;
   Sprite[1,1]:=Color;
   Sprite[1,2]:=Color;
   Sprite[1,3]:=Color;
   Sprite[1,4]:=Color;
   Sprite[1,5]:=Color;
   Sprite[1,6]:=Color;
   Sprite[1,7]:=0;
   Sprite[0,0]:=0;
   Sprite[0,1]:=0;
   Sprite[0,2]:=0;
   Sprite[0,3]:=Color;
   Sprite[0,4]:=Color;
   Sprite[0,5]:=0;
   Sprite[0,6]:=0;
   Sprite[0,7]:=0;
 End;
 DrawSprite(X,Y,Sprite);
End;

Begin
  GetFont(FontSegment,FontOfset);
End.
