#ifndef __MSH_H_INCLUDED__
#define __MSH_H_INCLUDED__

#include "picoshell_config.h"


/* ********************************************************************
 * Set a prompt string. MSH_CMD_PROMPT is used as default.
 */
void msh_set_prompt(char* str);


/* ********************************************************************
 * Read user input by getchar(), with Emacs-like line editting.
 * Once input of a command line finished, msh_get_cmdline() copies
 * users input string to  char* cmdline.
 *
 * 'char* cmdline' must have at least MSH_CMDLINE_CHAR_MAX of length
 */

int msh_get_cmdline(char* cmdline);




/* ********************************************************************
 * Returns cmdline it self if succesfully parsed all chars of cmdline.
 * Returns != cmdline if some left unprocessed (i.e, separated by a ';')
 * Returns NULL if syntax or other error
 *
 *     int argc;
 *     char* argv[MSH_CMDARGS_MAX];
 *     char  argbuf[MSH_CMDLINE_CHAR_MAX];
 *
 *     const char* ret = msh_parse_line(linebufp, argbuf, &argc, argv);
 */
const char*
msh_parse_line(const char* cmdline, char* argvbuf, int* pargc, char** pargv);




/* ********************************************************************
 *
 */
typedef struct
{
    const char *  name;
    int           (*func)(int argc, const char** argv);
#ifdef MSH_CONFIG_HELP
    const char *  description;
    const char *  usage;
#endif
} msh_command_entry;

#ifdef MSH_CONFIG_HELP

#    define msh_declare_command(name) \
            int cmd_##name(int argc, const char** argv);\
            extern const char cmd_##name##_desc[]; \
            extern const char cmd_##name##_usage[];
#    define msh_define_help( name, desc, usage ) \
            const char cmd_##name##_desc[] = desc; \
            const char cmd_##name##_usage[] = usage;
#    define msh_define_command(name) \
            {#name, cmd_##name, cmd_##name##_desc, cmd_##name##_usage}
#    define MSH_COMMAND_TERMINATOR  {0, 0, 0, 0}

#else /* MSH_CONFIG_HELP */

#    define msh_declare_command(name) \
            int cmd_##name(int argc, const char** argv);
#    define msh_define_help( name, desc, usage ) /* vanish */
#    define msh_define_command(name) {#name, cmd_##name}
#    define MSH_COMMAND_TERMINATOR  {0, 0}

#endif /* MSH_CONFIG_HELP */


extern const msh_command_entry msh_builtin_commands[];
int msh_do_command(const msh_command_entry* cmdp, int argc, const char** argv);

void msh_print_cmdlist(const msh_command_entry* cmdlist);
const char* msh_get_command_usage(const msh_command_entry* cmdlist, const char* cmdname);

#endif/*__MSH_H_INCLUDED__*/
