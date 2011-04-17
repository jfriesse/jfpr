                                          
      _/_/_/            _/  _/    _/      
   _/        _/_/_/    _/      _/_/_/_/   
    _/_/    _/    _/  _/  _/    _/        
       _/  _/    _/  _/  _/    _/         
_/_/_/    _/_/_/    _/  _/      _/_/      
         _/                               
        _/                                v 0.1 for DOS

Description : This program can split one file to parts. You can split file up
              to 676 parts. Program is written in 100% pure assembler, and is
              very fast.
Ussage : Split [arguments] [sourcefile] [prefix]
          
         Arguments:
            -b, --bytes SIZE	Output are files with size defined in argumet                        
                        	SIZE. SIZE may have a multiplier suffix: 
                        	b for 512, k for 1K, m for 1 Meg.
         Sourcefile: This is name of file which you want split
         Prefix : This is prefix of destination filename

         Program get part of sourcefile and create files with names PREFIXaa,
         PREFIXab,PREFIXac, ..., PREFIXzz

         Default prefix is x, and default size is 1457500 bytes. If sourcefile
         is -, then program read standart input.

Bugs : Can split only to 676 parts
Reporting Bugs : Report bugs to <janf@email.cz>
Future : - Better compatibility with Unix split command
         - Add bath file to join
         - Informations before splitting

Copyright:
       Copyright (C) 2002 Jan Friesse <janf@email.cz>
       This is free software; see the source for  copying  condi-
       tions.  There is NO warranty; not even for MERCHANTABILITY
       or FITNESS FOR A PARTICULAR PURPOSE.

FAQ:
  Q. How can I join splited files to one file
  A. You can use standart DOS command COPY. You can write something like
     COPY /B PREFIX?? DESTFILENAME
     For example : If you split file command.com with prefix command., and
     you get files command.aa, command.ab, command.ac, you can use 
     COPY /B command.?? command.new, to split files to one file command.new

  Q. Why is program limited to only 676 parts.
  A. Because I use 26 base integer. Calculate with me. Default sufix is two
     chars, like aa,ab, ... ,zz. Alphabet have 26 characters, and 26*26 is 676.

  Q. This program simulate Unix command split, isn't it?
  A. Yes. I use as my template command split in CYGWIN package.

  Q. Why do you write this program in assembler?
  A. Because I want small and fast program, where source code is 7x large 
     than binary file. And second reason is, that I want learn assembler.

Copyright (C) 2002 Jan Friesse <janf@email.cz>