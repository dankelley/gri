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
