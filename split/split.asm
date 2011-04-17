; Split - program to split large file
;(c) 2002 Jan Friesse (janf@email.cz)

;    This program is free software; you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation; either version 2 of the License, or
;    (at your option) any later version.

;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.

;    You should have received a copy of the GNU General Public License
;    along with this program; if not, write to the Free Software
;    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

org	100h ; code starts at offset 100h
use16    ; use 16-bit code

;********** Macros ************
macro PrintStr var
{
  push var
  call printstr
}

macro PrintStrLn var
{
  push var
  call printstrln
}

macro strcpy dest,source
{
  push dest
  push source
  call StrCpy
}

macro strcat dest,source
{
  push dest
  push source
  call StrCat
}

macro ProcessCommandLine
{
  mov cx,0
  mov cl,[80h] ;Put to cl number of args
  cmp cl,0 ;Test no args
  je WriteBadNOArgs
;  dec cl  ;Decrement NO args

  mov si,cx
  add si,82h
  mov [si],byte 13h

  mov [isword],FALSE
  mov [isquote],FALSE
  mov [TempStrPos],0

  mov [foow],0

  mov [CL_NextInput],CL_NEXTINPUT_NO
  mov [CL_FileNumber],CL_FILENUMBER_SOURCE

  .GCLLoop: 		      ; local label
    mov si,[foow]
    add si,82h ;Make adress of char
    mov dl,[si] ;In dl is now char

    ; Test spaces and tabs
    cmp dl,' '
    je .GCLLIsSpace
    cmp dl,9
    je .GCLLIsSpace

    ;Test if char is ENTER
    cmp dl,13
    je .GCLLIsEnter

    ;It is normal char
    cmp dl,'"'
    je .GCLLIsQuote ; Test if character is '"'
    mov [isword],TRUE ;No, it is normal char
    mov di,[TempStrPos]
    mov [TempStr+di],dl
    inc [TempStrPos]
    jmp .GCLLEnd

    .GCLLIsQuote:
      mov al,1
      sub al,[isquote]
      mov [isquote],al
      jmp .GCLLEnd

    .GCLLIsSpace:
      cmp [isword],FALSE
      je .GCLLEnd

      cmp [isquote],TRUE
      jne .GCLLIsntquote
      ;Add char and goto loop
      mov di,[TempStrPos]
      mov [TempStr+di],dl
      inc [TempStrPos]
      jmp .GCLLEnd

        .GCLLIsntquote:
          ;*****COMMAND PROCESS******
          ;lets process command
          mov [isword],FALSE
          ;add zero to end of command
          mov di,[TempStrPos]
          mov [TempStr+di],0

          ;TEST FLAGS
          ;Test size flag
          cmp [CL_NextInput],CL_NEXTINPUT_SIZE
          je  .GCLL_SetSize

          mov [TempStrPos],0 ;And set TempStrPos to zero. Command is here, because I want to write errors

          ;TEST OPTIONS
          ;Compare str to help string
          push TempStr
          push CL_HelpStr
          call CompareStr
          cmp [cs_res],TRUE
          je WriteHelp

          ;Compare str to -b str
          push TempStr
          push CL_SizeStr1
          call CompareStr
          cmp [cs_res],TRUE
          je .GCLL_SetSizeFlag
          push TempStr
          push CL_SizeStr2
          call CompareStr
          cmp [cs_res],TRUE
          je .GCLL_SetSizeFlag

          ;Ok, now, it can be only set of filename or prefix
          cmp [CL_FileNumber],CL_FILENUMBER_SOURCE
          je .GCLL_FileNumber_Source_Label
          cmp [CL_FileNumber],CL_FILENUMBER_PREFIX
          je .GCLL_FileNumber_Prefix_Label

          ;If I come here, I can't recognize argument. I have too much arguments
          jmp WriteTooManyArgs

          .GCLL_FileNumber_Source_Label:
            inc [CL_FileNumber]
            push SourceFileName
            push TempStr
            call StrCpy
            jmp .GCLLEnd

          .GCLL_FileNumber_Prefix_Label:
            inc [CL_FileNumber]
            push DestinationFilePrefix
            push TempStr
            call StrCpy
            jmp .GCLLEnd

          ;JUMP TO END
          jmp .GCLLEnd

        .GCLL_SetSizeFlag:
          mov [CL_NextInput],CL_NEXTINPUT_SIZE
          jmp .GCLLEnd

        .GCLL_SetSize:
          mov [CL_NextInput],CL_NEXTINPUT_NO
          ;Try to get last character
          mov si,[TempStrPos]
          dec si
          mov al,[TempStr+si]
          cmp al,'b'
          je .GCLL_SetSize_SetMul_B
          cmp al,'k'
          je .GCLL_SetSize_SetMul_K
          cmp al,'m'
          je .GCLL_SetSize_SetMul_M

          .GCLL_SetSize_SetMulLabel: ;This is label, where program jump after GCLL_SetSize_SetMul_?

          push TempStr
          call StrToDWord
          ;test error
          cmp [stdw_res],FALSE
          je WriteBadNumberOfBytes

          mov [TempStrPos],0 ;And set TempStrPos to zero.
          ;In eax is my number
          imul eax,[OnePartSizeMul]
          mov [OnePartSize],eax
          jmp .GCLLEnd

          .GCLL_SetSize_SetMul_B:
            mov [OnePartSizeMul],512
            jmp .GCLL_SetSize_SetMul_End
          .GCLL_SetSize_SetMul_K:
            mov [OnePartSizeMul],1024
            jmp .GCLL_SetSize_SetMul_End
          .GCLL_SetSize_SetMul_M:
            mov [OnePartSizeMul],1024*1024
            jmp .GCLL_SetSize_SetMul_End
          .GCLL_SetSize_SetMul_End:
            mov [TempStr+si],0
            jmp .GCLL_SetSize_SetMulLabel

    .GCLLIsEnter:
      mov [isquote],FALSE
      jmp .GCLLIsntquote ;Process last command

    .GCLLEnd:
;    mov di,[foow]
;    mov [var+di],dl

    inc [foow]

    dec cx
  jnz .GCLLoop

  ;Now test some variables
  cmp [CL_NextInput],CL_NEXTINPUT_SIZE
  je WriteBytesNeedArg

  ;Ok all is ok
}

macro Split
{
  push SourceFileName
  push DefSourceFileName
  call CompareStr
  cmp [cs_res],TRUE
  je .Split_SetStandartInput

  ;Try to open source file
  mov ah,03Dh
  mov dx,SourceFileName
  mov al,0
  int 21h
  jc WriteCantOpenSourceFile
  jmp .Split_SetInput

  .Split_SetStandartInput:
  mov ax,0

  .Split_SetInput:
  mov [SourceFileHandle],ax

  mov [ActualPart],0
  mov [DestFilePos],0

  .Split_Loop:
    cmp [ActualPart],0
    je .Split_CreateFile

    mov eax,[OnePartSize]
    cmp [DestFilePos],eax
    jne .Split_Read ;I mustn't create new file. Just read

    ;CLose file
    mov ah,03Eh
    mov bx,[DestFileHandle]
    int 21h
    ;Create destination filename
    .Split_CreateFile:
    inc [ActualPart]
    ;Test Actual Part. If actual part is larger than MAXPARTS, then goto end
    cmp [ActualPart],MAXPARTS
    ja WriteTooMuchParts

    strcpy DestinationFileName,DestinationFilePrefix
    push TempStr
    mov ax,[ActualPart]
    dec ax
    push ax
    call IntToASCII
    mov [TempStr+2],0
    strcat DestinationFileName,TempStr

    ;Open destination file
    mov ah,03Ch
    mov dx,DestinationFileName
    mov cx,0
    int 21h
    jc WriteCantOpenDestFile
    mov [DestFileHandle],ax
    mov [DestFilePos],0

  .Split_Read:
    ;Ok. Lets get NO bytes to read
    mov eax,[OnePartSize]
    sub eax,[DestFilePos]
    cmp eax,BUF_LEN
    jna .Split_ReadBuffer
    mov eax,BUF_LEN
    .Split_ReadBuffer: ;Now we can read data
    mov [ToRead],ax
    mov ah,03Fh
    mov bx,[SourceFileHandle]
    mov dx,Buffer
    mov cx,[ToRead]
    int 21h
    mov [ReadedBytes],ax
    ;Write no bytes
    mov ah,40h
    mov bx,[DestFileHandle]
    mov dx,Buffer
    mov cx,[ReadedBytes]
    int 21h

    mov eax,[DestFilePos]
    mov ebx,0
    mov bx,[ReadedBytes]
    add eax,ebx
    mov [DestFilePos],eax

    ;Test end of file
    mov al,06h
    mov ah,44h
    mov bx,[SourceFileHandle]
    int 21h
  cmp al,0
  jne .Split_Loop


  ;Close dest file
  mov ah,03Eh
  mov bx,[DestFileHandle]
  int 21h
  ;Close source file
  mov ah,03Eh
  mov bx,[SourceFileHandle]
  int 21h

;  push DestinationFileName
;  call MyCWrite
}

;************* Start of program ********
ProgramStart:
  PrintStrLn AboutText

  ;Set default values
  strcpy SourceFileName,DefSourceFileName
  strcpy DestinationFilePrefix,DefDestinationFilePrefix
  ProcessCommandLine

  ;Ok. All is ok. We can split
  Split

;  push SourceFileName
;  call MyCWrite
;  push DestinationFilePrefix
;  call MyCWrite

  jmp ProgramEnd

WriteTooMuchParts:
  PrintStr BadNumberOfBytes1
  PrintStrLn TooMuchParts
  jmp ProgramEnd

WriteBadNOArgs:
  PrintStrLn BadNOArgsText
  PrintStrLn TryText
  jmp ProgramEnd

WriteTooManyArgs:
  PrintStrLn TooManyArgs
  PrintStrLn TryText
  jmp ProgramEnd

WriteBadNumberOfBytes:
  PrintStr BadNumberOfBytes1
  mov si,[TempStrPos]
  mov [TempStr+si],24h ;Little change to string
  PrintStr TempStr
  PrintStrLn BadNumberOfBytes2
  PrintStrLn TryText
  jmp ProgramEnd

WriteBytesNeedArg:
  PrintStr BadNumberOfBytes1 ;This string is same
  PrintStrLn BytesNeedArg
  PrintStrLn TryText
  jmp ProgramEnd

WriteCantOpenSourceFile:
  PrintStr BadNumberOfBytes1
  push SourceFileName
  call MyCWrite
  PrintStr CantOpenSourceFile
  jmp ProgramEnd

WriteCantOpenDestFile:
  PrintStr BadNumberOfBytes1
  push DestinationFileName
  call MyCWrite
  PrintStr CantOpenDestFile
  ;Close source file
  mov ah,03Eh
  mov bx,[SourceFileHandle]
  int 21h
  jmp ProgramEnd

WriteHelp:
  PrintStrLn UssageText
  jmp ProgramEnd

ProgramEnd:
;  mov eax,[OnePartSize]
  int 20h

;******* Procedures definition ****
;StrToDWord
; Convert string to DWord. Need one arg, adress of string
; Return : in eax is DWord and in stdw_res is TRUE, if all is ok, else FALSE
StrToDWord:
  pop bp ;save bp register

  pop si
  xor eax,eax ;In eax is my numbers
  mov [stdw_res],TRUE

  xor ebx,ebx
  StrToDWordLoop:
    mov bl,[si]
    cmp bl,0
    je StrToDWordEnd ;It is end

    cmp bl,'0'
    jc StrToDWordError   ;This is not number
    cmp bl,'9'
    ja StrToDWordError ;This is not number
    imul eax,10
    sub ebx,'0'
    add eax,ebx

    inc si
    jmp StrToDWordLoop

  StrToDWordError:
    mov [stdw_res],FALSE

  StrToDWordEnd:

  push bp
  ret

;CompareStr
; Compare two string. compare is case sensitive
; Return : in cs_res is TRUE, if strings are equal, else FALSE
CompareStr:
  pop bp

  pop si
  pop di

  mov [cs_res],TRUE
  CompareStrLoop:
    mov al,[si]
    mov bl,[di]
    cmp al,0
    je CompareStrTest
    cmp bl,0
    je CompareStrTest
    inc si
    inc di
    cmp al,bl
    je CompareStrLoop

    CompareStrTest:
      cmp al,bl
      je CompareStrEnd
      mov [cs_res],FALSE
  ;Little bug. Repaired
  CompareStrEnd:

  push bp
  ret

;StrCpy
; Copy one to string to another string
; First argument must be destination and second is source

StrCpy:
  pop bp

  pop si
  pop di

  StrCpyLoop:
    mov al,[si]
    mov [di],al
    inc si
    inc di

    cmp al,0
    jne StrCpyLoop

  push bp
  ret

;StrCat
; Concat two strings
; First argument must be destination and second is source

StrCat:
  pop bp

  pop si
  pop di

  StrCatLoop1:
    mov al,[di]
    inc di
    cmp al,0
    jne StrCatLoop1

  dec di
  StrCatLoop2:
    mov al,[si]
    mov [di],al
    inc si
    inc di
    cmp al,0
    jne StrCatLoop2

  push bp
  ret

;MyCWrite
; Write, but not like DOS, where last char is $, but like C where last char is with code 0h
; First argument must be string
MyCWrite:
  pop bp

  pop si

  MyCWriteLoop:
    mov dl,[si]

    cmp dl,0
    je MyCWriteEnd

    mov ah,02h
    int 21h
    inc si
    jmp MyCWriteLoop

  MyCWriteEnd:
  push bp
  ret

;IntToASCII
;Convert number to 26 base ASCII
;Example 1 - aa
;        2 - ab
;        26 - ba
;        ...

IntToASCII:
  pop bp

  pop ax
  pop di

  ;Now div
  mov bl,26
  div bl

  add ah,'a'
  add al,'a'
  mov [di],al
  mov [di+1],ah

  push bp
  ret

printstr:
  pop bp

  pop dx
  mov ah,9
  int 21h

  push bp
ret

printstrln:
  pop bp

  pop dx
  mov ah,9
  int 21h
  mov ah,02h
  mov dl,13
  int 21h
  mov dl,10
  int 21h

  push bp
ret

;****** Constants definition *****
TRUE = 1
FALSE = 0

CL_NEXTINPUT_NO   = 0
CL_NEXTINPUT_SIZE = 1

CL_FILENUMBER_SOURCE = 0
CL_FILENUMBER_PREFIX = 1
CL_FILENUMBER_ERROR  = 2

BUF_LEN = 512

MAXPARTS = 676

;*****Variables*******
AboutText db 'Split v0.1 - Program to split large file',13,10,'(c) Jan Frisse 2002. Made in pure assembler FASM. Published under GPL licence.',24h ; Licence str
BadNOArgsText db 'split : to few no option arguments',24h
TryText db 'Try "split --help" for more information.',24h
UssageText db 'Ussage : split [OPTION] [INPUT] [PREFIX]',13,10,'Output fixed-size pieces of INPUT to PREFIXaa, PREFIXab, ...',13,10,'Default PREFIX is "x". If input is "-", read standart input.',13,10,'Options : ',13,10,'  -b, --bytes SIZE	Put SIZE bytes per output file.',13,10,13,10,'Default SIZE is 1457500',13,10,'SIZE may have a multiplier suffix: b for 512, k for 1K, m for 1 Meg.',13,10,13,10,'Report bugs to <janf@email.cz>',24h
BadNumberOfBytes1 db 'split: ',24h
BadNumberOfBytes2 db ': invalid number of bytes',24h
BytesNeedArg db 'option "--bytes" or "-b" requires an argument',24h
TooManyArgs db 'split: too many arguments',24h
CantOpenSourceFile db ': No such file or directory',24h
CantOpenDestFile db ': Can',27h,'t open file for writing',24h
TooMuchParts db 'Too much parts of splited file. Maximal is 676 parts.',24h
CL_HelpStr db '--help',0
CL_SizeStr1 db '-b',0
CL_SizeStr2 db '--bytes',0
OnePartSize dd 1457500
;OnePartSize dd 105535
OnePartSizeMul dd 1 ;One Part Multiplificator
DefSourceFileName db '-',0
DefDestinationFilePrefix db 'x',0
;DefSourceFileName db 'fallout2.exe',0
;DefDestinationFilePrefix db 'E:\Test\a',0
CL_FileNumber db ? ;Number of file, which is now enter
CL_NextInput db ? ;Next Input. If you enter -b, next input have to be number
TempStrPos  dw ?   ;Temp str position
SourceFileHandle dw ? ;Source file handle
DestFileHandle dw ? ;Destination file handle
DestFilePos dd ? ;Destination file position
ToRead dw ? ;NO byte to read
ReadedBytes dw ? ; Really readed bytes
ActualPart dw ? ;Actual part of file
foob db ? ; Foo byte
foow dw ? ; Foo word
isword db ? ; Is word
isquote db ? ;Is quote
cs_res db ? ;Compare string res
stdw_res db ? ;String To Double Word res
CommandLine rb 7Fh ;Command line
TempStr     rb 7Fh ;Temp line
SourceFileName rb 7Fh ;Name of source file
DestinationFilePrefix rb 7Fh ;Prefix of destination file; After prefix are automaticaly added aa,ab,ac,...
DestinationFileName rb 7Fh; Destination file name
Buffer rb BUF_LEN; Buffer