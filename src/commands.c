#include <string.h>
#include <stdio.h>

#include "picoshell.h"
#include "picoshell_config.h"
#include "picoshell_termesc.h"


/* ***************************************************************************
 *                cmdedit help strings (displayed by 'shellhelp')
 * ***************************************************************************/
#ifdef MSH_CONFIG_HELP_KEYBIND
#ifdef MSH_CONFIG_LINEEDIT
#    define MSH_HELP_LINEEDIT \
        " * Minimal Emacs-like line editting. (Ctrl+F,B,E,A)\n" \
        "    Ctrl-F  Cursor right     ('F'orward)\n"  \
        "    Ctrl-B  Cursor left      ('B'ackward)\n" \
        "    Ctrl-A  Cursor line head ('A'head)\n"  \
        "    Ctrl-E  Cursor line tail ('E'nd)\n"
#else
#   define MSH_HELP_LINEEDIT ""
#endif

#ifdef MSH_CONFIG_CMDHISTORY
#    define MSH_HELP_CMDHISTORY \
        " * Command-line history. (Ctrl+P,N)\n" \
        "    Ctrl-P  Previous history ('P'revious)\n" \
        "    Ctrl-N  Next history     ('N'ext)\n"
#else
#   define MSH_HELP_CMDHISTORY ""
#   define MSH_HELP_CMDHISTORY_KEYS ""
#endif

#ifdef MSH_CONFIG_CLIPBOARD
#    define MSH_HELP_CLIPBOARD  \
        " * Cut & paste. (Ctrl+K,W,Y)\n" \
        "    Ctrl-K  Cut strings after the cursor to clipboard  ('K'ill)\n" \
        "    Ctrl-W  Cut a word before the cursor to clipboard  ('W'ord)\n" \
        "    Ctrl-Y  Paste clipboard content to cursor position ('Y'ank)\n"
#else
#    define MSH_HELP_CLIPBOARD  ""
#endif

#define MSH_CMDEDIT_HELP_DESCRIPTION \
    "* Basic keybinds\n" \
    "    Ctrl-H  Backspace\n" \
    "    Ctrl-D  Delete\n" \
    "    Ctrl-L  Clear screen\n" \
    "    Ctrl-C  Discard line\n" \
    "    Ctrl-U  Kill whole line\n" \
    MSH_HELP_LINEEDIT \
    MSH_HELP_CMDHISTORY \
    MSH_HELP_CLIPBOARD
#endif


/* ***************************************************************************
 *                         the builtin commands
 * ***************************************************************************/
#ifdef MSH_CONFIG_HELP_KEYBIND
static int cmd_shellhelp(int argc, const char** argv)
{
    pico_puts(MSH_CMDEDIT_HELP_DESCRIPTION);
    return 0;
}
#endif


static int cmd_echo(int argc, const char** argv)
{
    int i;
    if ( argc < 1 ) {
        return 0;
    }
    for ( i = 1;  i < argc;  i++ ) {
        pico_puts(argv[i]);
        pico_putchar(' ');
    }
    pico_putchar('\n');
    return 0;
}


/* ***************************************************************************
 *                          command registration
 * ***************************************************************************/

const msh_command_entry msh_builtin_commands[] = {
#ifdef MSH_CONFIG_HELP_KEYBIND
    { "shellhelp", cmd_shellhelp,
        "display help for keybinds of commandline editting",
        "No further help available.\n",
    },
#endif

    { "echo", cmd_echo,
#ifdef MSH_CONFIG_HELP
        "echo all arguments separated by a whitespace",
        "Usage: echo [string ...]\n"
#endif
    },

    MSH_COMMAND_TERMINATOR
};


/*
 * Search built-in command entry.
 */
static const msh_command_entry *
find_command_entry(const msh_command_entry* cmdlist, const char* name)
{
    int i = 0;
    while ( cmdlist[i].name != NULL ) {
        if ( strcmp(cmdlist[i].name, name) == 0 ) {
            return &cmdlist[i];
        } else {
            i++;
        }
    }
    return NULL;
}


/*
 * Find a command 'argv[0]' from cmdlist (using find_command_entry())
 * and executes it.
 */
int msh_do_command(const msh_command_entry* cmdlist, int argc, const char** argv)
{
    const msh_command_entry* cmd_entry;

    if ( argc < 1 ) {
        return -1;
    }

    cmd_entry = find_command_entry(cmdlist, argv[0]);

    if ( cmd_entry != NULL ) {
        return ( cmd_entry->func(argc, argv) );
    } else {
        /*
        pico_puts("command not found: ");
        pico_puts(argv[0]);
        pico_puts("\n");
        */
        return -1;
    }
}

#ifdef MSH_CONFIG_HELP
void msh_print_cmdlist(const msh_command_entry* cmdlist)
{
    int i, j;
    const int indent = 10;

    i = 0;
    while ( cmdlist[i].name != NULL ) {
            pico_puts("    ");
            pico_puts(cmdlist[i].name);
            for (j = indent - strlen(cmdlist[i].name);  j > 0;  j--) {
                pico_putchar(' ');
            }
            pico_puts("- ");
            if ( cmdlist[i].description != NULL ) {
                pico_puts(cmdlist[i].description);
                pico_puts("\n");
            } else {
                pico_puts("(No description available)\n");
            }
        i++;
    }
    return;
}


const char* msh_get_command_usage(const msh_command_entry* cmdlist, const char* cmdname)
{
    const msh_command_entry* cmd_entry;

    cmd_entry = find_command_entry(cmdlist, cmdname);
    if ( cmd_entry == NULL ) {
        return NULL; /* No such command */
    } else if ( cmd_entry->usage == NULL ) {
        return "No help available.\n";
    } else {
        return cmd_entry->usage;
    }
}
#else
void msh_print_cmdlist(const msh_command_entry* cmdlist) { /* do nothing */ }
const char* msh_get_command_usage(const msh_command_entry* cmdlist, const char* cmdname)
{
    return "No help available.\n";
}
#endif /*MSH_CONFIG_HELP*/
