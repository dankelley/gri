/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#if !defined(_command_h_)

#define _command_h_
#include        <stdio.h>
#include        "gr.hh"

int             cmd_being_done(void);
bool            is_system_command(const char *cmdline);
bool            is_create_new_command(const char *cmdline);
bool            get_command_line(void);
bool            massage_command_line(const char *cmdline);
int             match_gri_syntax(const char *cmdline, int flag);
bool            perform_gri_program(void);
int             perform_gri_cmd(int cmd);
void            pop_command_word_buffer(void);
void            pop_cmd_being_done_stack(void);
void            push_command_word_buffer(void);
void            push_cmd_being_done_stack(int cmd);

#endif				// _command_h_
