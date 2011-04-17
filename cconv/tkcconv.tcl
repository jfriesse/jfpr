#!/usr/bin/wish

# ***************************************************************************
# *                         tkcconv  -  tcl/tk GUI for cconv                *
# *                            -------------------                          *
# *   begin                : So pro  21 8:45:22 CET 2002                    *
# *   copyright            : (C) 2002 by Jan Friesse                        *
# *   email                : janf@email.cz                                  *
# ***************************************************************************

# ***************************************************************************
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# ***************************************************************************

#set definitions
#EOL definitions
set EOL_DOS  1
set EOL_UNIX 2
set EOL_MAC  3
#coding definitions
set COD_KAM 0
set COD_W95 1
set COD_LA2 2
set COD_W31 3
set COD_UNX 4
set COD_KO8 5
set COD_COR 6
set COD_IBM 7

#set standart values
set s_eol $EOL_UNIX
set d_eol $EOL_UNIX
set s_cod $COD_UNX
set d_cod $COD_UNX

set s_name ""
set d_name ""

set cconv_path "cconv"

#set filter
set my_file_types {
    {{Text files}        {.txt}}
    {{Sub files}         {.sub}}
    {{All files}         *    }}

#procedures
proc help {} {
  upvar cconv_path cconv_path
  set cconv_pipe [open "|$cconv_path --version" "r"]
  set pipe_out [read $cconv_pipe]

tk_dialog .info "About" "TKCconv 0.2 - tcl\\tk GUI for cconv\n(c) 2002 Jan Friesse (janf@email.cz)\n\n\
                 This program is free software; you can redistribute it and/or modify\n  \
                 it under the terms of the GNU General Public License as published by\n  \
                 the Free Software Foundation; either version 2 of the License, or\n     \
                 (at your option) any later version.\n                                   \
                 \nCconv version : $pipe_out" info 0 Ok
}
  
proc analyze {} {
  upvar s_name s_name
  upvar s_eol s_eol
  upvar s_cod s_cod
  upvar cconv_path cconv_path
  
  if {$s_name == ""} {tk_dialog .info "Error" "Please enter file name!" error 0 Ok;return}
  set command_line "$cconv_path \"$s_name\"" 

  #open pipe
  set cconv_pipe [open "|$command_line" "r"]
  set pipe_out [read $cconv_pipe]
  #  gets $cconv_pipe pipe_out
 
  if [string match "cconv (2)*" $pipe_out] {
    tk_dialog .info "Error" "Can't open file \"$s_name\"" error 0 OK
  } else {
    #show coding
    set str_pos [string first "\" = " $pipe_out]
    set coding [string range $pipe_out [expr $str_pos+4] [expr $str_pos+6]]
    
    set s_eol [expr $coding / 10]
    set s_cod [expr $coding % 10]
#   puts "$coding $s_eol $s_cod"; #this is debug
  }
  
  return
}

#not needed yet. Maybe in future
proc centerwindow {status_w} {
  wm iconify $status_w
  set parenconf [wm geometry .]
  set parenconf [split $parenconf {+ - x}]
  set winconf [split [wm geometry $status_w] {+ - x}]
  set X [expr [lindex $parenconf 2] + [lindex $parenconf 0]/2 - [winfo reqwidth $status_w]/2]
  set Y [expr [lindex $parenconf 3] + [lindex $parenconf 1]/2 - [winfo reqheight $status_w]/2]

  wm geometry $status_w +$X+$Y
  wm title $status_w "Converting..."

  wm transient $status_w .
  wm resizable $status_w 0 0
  wm deiconify $status_w
  return
}

proc convert {} {
  upvar s_name s_name
  upvar s_eol s_eol
  upvar s_cod s_cod
  upvar d_eol d_eol
  upvar d_cod d_cod
  upvar d_name d_name
  upvar cconv_path cconv_path

  if {$s_name == ""} {tk_dialog .info "Error" "Please enter source file name!" error 0 Ok;return}
  if {$d_name == ""} {tk_dialog .info "Error" "Please enter destination file name!" error 0 Ok;return}
 
 set command_line "$cconv_path \"$s_name\" \"$d_name\" $s_eol$s_cod $d_eol$d_cod" 

  #open pipe
  set cconv_pipe [open "|$command_line" "r"]
  set pipe_out [read $cconv_pipe]

  if {$pipe_out == ""} {
    tk_dialog .info "Info" "Converting was finished successfull!" info 0 Ok
  } else {
    scan $pipe_out "cconv (%d" my_error

    set error_msg ""
    if {$my_error == "2"} {set error_msg "Can't open source file \"$s_name\""}
    if {$my_error == "3"} {set error_msg "Can't create destination file \"$d_name\""}
    if {$my_error == "4"} {set error_msg "Bad number of arguments (this error you might never see, maybe you found bug. Please write me to <janf@email.cz>"}
    if {$my_error == "5"} {set error_msg "Argument error (this error you might never see, maybe you found bug. Please write me to <janf@email.cz>"}
    if {$error_msg == ""} {set error_msg "Unraised error (this error you might never see, maybe you found bug. Please write me to <janf@email.cz>"}

    tk_dialog .info "Error" "Error $my_error: $error_msg" info 0 Ok
  }
#puts "$pipe_out $command_line"

#  toplevel .status_w
#  set status_w .status_w

#  label $status_w.status_label -text "Converting \"$s_name\" to \"$d_name\""
#  pack $status_w.status_label
 
#  centerwindow $status_w

#  exec "/bin/sh" "|&"

#  destroy $status_w
  #  fileevent $cconv_pipe readable "
  return
}

#start of main program
#hide my window
wm iconify .
wm title . "tkcconv"
wm resizable . 0 0

#create source part
radiobutton .rb_se_dos -text "Dos/Windows" -variable s_eol -value $EOL_DOS -justify left
radiobutton .rb_se_unix -text "Unix/Linux" -variable s_eol -value $EOL_UNIX -justify left
radiobutton .rb_se_mac -text "Mac" -variable s_eol -value $EOL_MAC -justify left

radiobutton .rb_sc_kam -text "Kamenici" -variable s_cod -value $COD_KAM -justify left
radiobutton .rb_sc_w95 -text "Windows 95/NT" -variable s_cod -value $COD_W95 -justify left
radiobutton .rb_sc_la2 -text "Latin 2" -variable s_cod -value $COD_LA2 -justify left
radiobutton .rb_sc_w31 -text "Windows 3.11" -variable s_cod -value $COD_W31 -justify left
radiobutton .rb_sc_unx -text "Unix/Linux" -variable s_cod -value $COD_UNX -justify left
radiobutton .rb_sc_ko8 -text "KOI8" -variable s_cod -value $COD_KO8 -justify left
radiobutton .rb_sc_cor -text "COR" -variable s_cod -value $COD_COR -justify left
radiobutton .rb_sc_ibm -text "IBM (without czech)" -variable s_cod -value $COD_IBM -justify left

entry .en_sname -textvariable s_name
button .bt_sname -text "..." -justify center -command {set s_name [tk_getOpenFile -filetypes $my_file_types -initialfile $s_name]}

label .lb_source -text "Source file ..."

#create destination part
radiobutton .rb_de_dos -text "Dos/Windows" -variable d_eol -value $EOL_DOS -justify left
radiobutton .rb_de_unix -text "Unix/Linux" -variable d_eol -value $EOL_UNIX -justify left
radiobutton .rb_de_mac -text "Mac" -variable d_eol -value $EOL_MAC -justify left

radiobutton .rb_dc_kam -text "Kamenici" -variable d_cod -value $COD_KAM -justify left
radiobutton .rb_dc_w95 -text "Windows 95/NT" -variable d_cod -value $COD_W95 -justify left
radiobutton .rb_dc_la2 -text "Latin 2" -variable d_cod -value $COD_LA2 -justify left
radiobutton .rb_dc_w31 -text "Windows 3.11" -variable d_cod -value $COD_W31 -justify left
radiobutton .rb_dc_unx -text "Unix/Linux" -variable d_cod -value $COD_UNX -justify left
radiobutton .rb_dc_ko8 -text "KOI8" -variable d_cod -value $COD_KO8 -justify left
radiobutton .rb_dc_cor -text "COR" -variable d_cod -value $COD_COR -justify left
radiobutton .rb_dc_ibm -text "IBM (without czech)" -variable d_cod -value $COD_IBM -justify left

entry .en_dname -textvariable d_name
button .bt_dname -text "..." -justify center -command {set d_name [tk_getSaveFile -filetypes $my_file_types -initialfile $d_name]}

label .lb_destination -text "Destination file ..."

#create buttons
button .bt_analyze -text "Analyze" -justify center  -command {analyze}
button .bt_convert -text "Convert" -justify center  -command {convert}
button .bt_help -text "Help" -justify center -command {help}
button .bt_exit -text "Exit" -justify center -command {exit} 

#create source part grid
grid .rb_se_dos -row 2 -column 3  -sticky "w" -columnspan 2
grid .rb_se_unix -row 3 -column 3 -sticky "w" -columnspan 2
grid .rb_se_mac -row 4 -column 3 -sticky "w" -columnspan 2

grid .rb_sc_kam -row 2 -column 1 -sticky "w"
grid .rb_sc_w95 -row 3 -column 1 -sticky "w"
grid .rb_sc_la2 -row 4 -column 1 -sticky "w"
grid .rb_sc_w31 -row 5 -column 1 -sticky "w"
grid .rb_sc_unx -row 2 -column 2 -sticky "w"
grid .rb_sc_ko8 -row 3 -column 2 -sticky "w"
grid .rb_sc_cor -row 4 -column 2 -sticky "w"
grid .rb_sc_ibm -row 5 -column 2 -sticky "w"

grid .en_sname -row 1 -column 1 -columnspan 2 -sticky "we"
grid .bt_sname -row 1 -column 3  -sticky "w"

grid .lb_source -row 0 -column 1 -columnspan 2 -sticky "w"
grid .bt_analyze -row 1 -column 4 -sticky "w"

#create destination part grid
grid .rb_de_dos -row 8 -column 3  -sticky "w" -columnspan 2
grid .rb_de_unix -row 9 -column 3 -sticky "w" -columnspan 2
grid .rb_de_mac -row 10 -column 3 -sticky "w" -columnspan 2

grid .rb_dc_kam -row 8 -column 1 -sticky "w"
grid .rb_dc_w95 -row 9 -column 1 -sticky "w"
grid .rb_dc_la2 -row 10 -column 1 -sticky "w"
grid .rb_dc_w31 -row 11 -column 1 -sticky "w"
grid .rb_dc_unx -row 8 -column 2 -sticky "w"
grid .rb_dc_ko8 -row 9 -column 2 -sticky "w"
grid .rb_dc_cor -row 10 -column 2 -sticky "w"
grid .rb_dc_ibm -row 11 -column 2 -sticky "w"

grid .en_dname -row 7 -column 1 -columnspan 3 -sticky "ew"
grid .bt_dname -row 7 -column 4 -sticky "w"

grid .lb_destination -row 6 -column 1 -columnspan 3 -sticky "w"

#create button part grid
grid .bt_convert -row 12 -column 1 -sticky "we" -padx 2
grid .bt_help -row 12 -column 2 -sticky "we" -padx 2
grid .bt_exit -row 12 -column 3 -sticky "we" -columnspan 2 -padx 2

#show main window
wm deiconify .
#try to find cconv
if {[catch {exec $cconv_path "-v"} error_msg]} {
  tk_dialog .info "Error" "On your system isn't cconv. This program is needed for tkcconv. Program aborted" info 0 Ok
  exit
}

