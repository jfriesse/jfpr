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
Unit SI_U_1;

Interface

Type TSprite=Array[0..7,0..7] of Byte;

Procedure GraphOn; {Procedure turn on graphic mode (320x200x256)}
Procedure GraphOff; {Procedure turn off graphic mode}
Procedure PutPixel(X,Y:Integer;C:Byte); {Procedure Put pixel}
Function GetPixel(X,Y:Integer):Byte; {Function get pixel}
Procedure DrawSprite(X,Y:Integer;Sprite:TSprite); {Procedure DrawSprite}

Implementation

Uses Crt;
Type
  PFontBuffer = ^TFontBuffer;
  TFontBuffer = array[0..8191] of byte;

Var
  sysfont  : PFontBuffer;
  DBuffer   : pointer;


Var OldTextmode:Integer; {Old Text Mode}

Procedure DrawSprite(X,Y:Integer;Sprite:TSprite);
Var I:Integer;
Begin
  For I:=0 to 7 do
  Begin
    Move(Mem[Seg(Sprite):Ofs(Sprite)+I*8],Mem[$A000:(Y+i)*320+X],8);
  End;
End;

Function GetPixel(X,Y:Integer):Byte; {Function get pixel}
Begin
  GetPixel:=Mem[$A000:Y*320+x];
End;

Procedure PutPixel(X,Y:Integer;C:Byte);
Begin
  Mem[$A000:Y*320+x]:=C;
End;

Procedure GraphOn;
Var Gmode:Byte;
Begin
  Write('Switching to 320x200x256...');
  OldTextMode:=LastMode;
  asm;
    mov ah,0;
    mov al,$13
    int $10;
    mov ah,$0F;
    mov al,0;
    int $10;
    mov gmode,al;
  end;
  if gmode<>$13 then
  Begin
    Writeln('Error 1 : Can not switch to vga mode 320x200x256.');
    Halt(1);
  End;
End;

Procedure GraphOff;
Begin
  Textmode(OldTextmode);
End;

Begin
End.
