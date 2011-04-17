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
Unit SI_U_3;

Interface

Uses SI_U_2,SI_U_1;

Type PALien=^TAlien;
     TAlien=Object
       X,Y:Integer;
       Color:Byte;
       Typ:Byte;
       Procedure Show;virtual;
       Procedure Hide;virtual;
       Destructor Done;virtual;
     End;

    PAlien1=^TAlien1;
    TALien1=Object(TAlien)
      Procedure Show;virtual;
      Procedure Hide;virtual;
      Destructor Done;virtual;
      Constructor Create(AX,AY:Integer;C:Byte);
    End;

    PAlien2=^TAlien2;
    TALien2=Object(TAlien)
      Procedure Show;virtual;
      Procedure Hide;virtual;
      Destructor Done;virtual;
      Constructor Create(AX,AY:Integer;C:Byte);
    End;

    PAlien3=^TAlien3;
    TALien3=Object(TAlien)
      Procedure Show;virtual;
      Procedure Hide;virtual;
      Destructor Done;virtual;
      Constructor Create(AX,AY:Integer;C:Byte);
    End;

    PPlayer=^TPlayer;
    TPlayer=Object
      X,Y:Integer;
      Color:Byte;
      Procedure Show;
      Procedure Hide;
      Destructor Done;
      Constructor Create(AX,AY:Integer;C:Byte);
    End;

    PShot=^TShot;
    TShot=Object
      X,Y:Integer;
      Color:Byte;
      OldTime,DTime:Longint;
      Procedure Show;
      Procedure Hide;
      Destructor Done;
      Constructor Create(AX,AY:Integer;C:Byte);
    End;

Implementation

Uses Dos;

Constructor TShot.Create(AX,AY:Integer;C:Byte);
Var h,m,s,sec100:Word;
Begin
  DTime:=1;
  GetTime(H,M,S,Sec100);
  OldTime:=Sec100+S*100+M*100*60+H*60*60*100;
  X:=AX;
  Y:=AY;
  Color:=C;
End;

Procedure TShot.Show;
Begin
  FillChar(Mem[$A000:(Y+2)*320+X],2,Color);
  FillChar(Mem[$A000:(Y+1)*320+X],2,Color);
  FillChar(Mem[$A000:(Y+0)*320+X],2,Color);
End;

Procedure TShot.Hide;
Begin
  FillChar(Mem[$A000:(Y+2)*320+X],2,0);
  FillChar(Mem[$A000:(Y+1)*320+X],2,0);
  FillChar(Mem[$A000:(Y+0)*320+X],2,0);
End;

Destructor TShot.Done;
Begin
  Hide;
End;

Constructor TPlayer.Create(AX,AY:Integer;C:Byte);
Begin
  X:=AX;
  Y:=AY;
  Color:=C;
End;

Procedure TPlayer.Show;
Begin
  DrawPlayer(x,y,color);
End;

Procedure TPlayer.Hide;
Begin
  ClearEnemy(X,y);
End;

Destructor TPlayer.Done;
Begin
  Hide;
End;

Constructor TAlien2.Create(AX,AY:Integer;C:Byte);
Begin
  X:=AX;
  Y:=AY;
  Color:=C;
  Typ:=Alien2;
End;

Procedure TAlien1.Show;
Begin
  DrawEnemy(X,y,color,Typ);
End;

Procedure TAlien1.Hide;
Begin
  ClearEnemy(X,y);
End;

Destructor TAlien1.Done;
Begin
  Hide;
End;

Constructor TAlien1.Create(AX,AY:Integer;C:Byte);
Begin
  X:=AX;
  Y:=AY;
  Color:=C;
  Typ:=Alien1;
End;

Procedure TAlien2.Show;
Begin
  DrawEnemy(X,y,color,Typ);
End;

Procedure TAlien2.Hide;
Begin
  ClearEnemy(X,y);
End;

Destructor TAlien2.Done;
Begin
  Hide;
End;

Procedure TAlien3.Show;
Begin
  DrawEnemy(X,y,color,Typ);
End;

Procedure TAlien3.Hide;
Begin
  ClearEnemy(X,y);
End;

Destructor TAlien3.Done;
Begin
  Hide;
End;

Constructor TAlien3.Create(AX,AY:Integer;C:Byte);
Begin
  X:=AX;
  Y:=AY;
  Color:=C;
  Typ:=Alien3;
End;

Procedure TAlien.Show;
Begin
End;

Procedure TAlien.Hide;
Begin
End;

Destructor TAlien.Done;
Begin
  Hide;
End;

Begin
End.
