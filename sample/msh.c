/* *************************************************************************** *
 *                       Stub IO routines for our sample
 * *************************************************************************** */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void io_open(void);
void io_close(void);

int getchar(void);
int putchar(int c);
int puts(const char* s);


static int tty;

/* To make sure terminal state to be restored when killed */
void sig_exit(int sig)
{
    putchar('\n');
    (void) sig; /* unused */
    io_close();
    exit(EXIT_SUCCESS);
}

void io_open(void)
{
    /* disable terminal buffering, local echo, and Ctrl-C,
     * to emulate serial IO */
    puts("Type 'exit' or press Ctrl-\\ to quit.\n");
    system("stty -icanon min 1 time 0");
    system("stty -echo");
    system("stty intr ''");
    tty = 0;

    {
        struct sigaction sigact;
        sigact.sa_handler = sig_exit;
        sigact.sa_flags   = 0; /* no flags */
        sigaction(SIGQUIT, &sigact, 0);
    }
}

void io_close(void)
{
    /* close(tty), restore Ctrl-C */
    puts("bye\n");
    system("stty intr ^C");
    system("stty echo");
}

#undef getchar
int getchar(void)
{
    char buf;
    read(tty, &buf, 1);
    return buf;
}

#undef putchar
int putchar(int c)
{
    char buf = c;
    write(tty, &buf, 1);
    return 0;
}

#undef puts
int puts(const char* s)
{
    const char* c = s;
    while ( *c != '\0' ) {
        putchar(*c);
        c++;
    }
    /* unlike libc standard puts(), our puts() doesn't put a trailing newline */
    /* putchar('\n'); */
    return 1;
}




/* *************************************************************************** *
 *                   Tiny sample shell using msh routines.
 * *************************************************************************** */

#include "msh.h"
#include "msh_termesc.h"

#define PUTS_BLUE_BACK(charp) \
{ \
    puts(TERMESC_BACK_BLUE); \
    puts(charp); \
    puts(TERMESC_FONT_NORMAL);\
}


#define PUTS_GREEN_BACK(charp) \
{ \
    puts(TERMESC_BACK_GREEN); \
    puts(charp); \
    puts(TERMESC_FONT_NORMAL);\
}

/*
 * Prototypes for our commands
 */
msh_declare_command( help );
msh_declare_command( myecho );
msh_declare_command( exit );


const msh_command_entry my_commands[] = {
    msh_define_command( help ),
    msh_define_command( myecho ),
    msh_define_command( exit ),
    MSH_COMMAND_TERMINATOR
};


/*
 * Define some original commands over built-ins
 *     We have to define cmd_help by yourself...
 */

msh_define_help( help, "display help for available commands",
        "Usage: help [command]\n"
        "    Displays help for 'command', or all commands and their\n"
        "    short descriptions.\n" );
int cmd_help(int argc, const char** argv)
{
    if ( argc == 1) {
        msh_print_cmdlist(my_commands);
        msh_print_cmdlist(msh_builtin_commands);
    }
    else
    {
        const char* usage;
        usage = msh_get_command_usage(my_commands, argv[1]);

        if ( usage == NULL ) {
            usage = msh_get_command_usage(msh_builtin_commands, argv[1]);
        }
        if ( usage == NULL ) {
            printf("No such command: '%s'\n", argv[1]);
        }
        else
        {
            puts(usage);
        }
    }
    return 0;
}


msh_define_help( myecho, "display a line of text",
        "Usage: myecho [STRING]...\n");
int cmd_myecho(int argc, const char** argv)
{
    int i;
    if ( argc < 1 ) {
        return 0;
    }
    for ( i = 1;  i < argc;  i++ ) {
        puts(argv[i]);
        putchar('_'); /* '_' instead of ' ' */
    }
    putchar('\n');
    return 0;
}


msh_define_help( exit, "exit the msh sample",
        "Usage: exit [EXIT_CODE]\n" );
int cmd_exit(int argc, const char** argv)
{
    if ( argc > 2 ) {
        puts("exit: too many arguments\n");
        return 1;
    } else {
        int exit_code = 0; /* default success */
        if ( argc == 2 ) {
            exit_code = atoi(argv[1]);
        }
        io_close();
        exit(exit_code);
    }
    return 0;
}

/*
 * main() just loopingly read input line, parse, and execute.
 */

int main(void)
{
    char  linebuf[MSH_CMDLINE_CHAR_MAX];

    io_open();
    msh_set_prompt("SAMPLE> ");


    /*
     * Loop for reading line (forever).
     */
    int   argc;
    char* argv[MSH_CMDARGS_MAX];
    char  argbuf[MSH_CMDLINE_CHAR_MAX];
    while ( 1 ) {
        /*
         * Read a input line
         */
        if (  msh_get_cmdline(linebuf) == 0 ) {
            /* got empty input */
            continue;
        }

        /*
         * Loop for parse line and executing commands.
         */
        const char *linebufp = linebuf;
        while ( 1 ) {
            int i;
            const char* ret_parse;
            int ret_command;

            ret_parse = msh_parse_line(linebufp, argbuf, &argc, argv);

            if ( ret_parse == NULL ) {
                puts("Syntax error\n");
                break; /* discard this line */
            }
            if ( strlen(argv[0]) <= 0 ) {
                break; /* empty input line */
            }

            puts(">> ");
            for ( i = 0; i < argc; i++ ) {
                PUTS_GREEN_BACK(argv[i]);
                if ( i < argc ) {
                    puts(", ");
                }
            }
            puts("\n");

            ret_command = msh_do_command(my_commands, argc, (const char**)argv);
            if ( ret_command < 0 ) {
                /* If the command not found amoung my_commands[], search the
                 * buildin */
                ret_command =
                    msh_do_command(msh_builtin_commands, argc, (const char**)argv);
            }
            if ( ret_command < 0 ) {
                puts("command not found: \'");
                puts(argv[0]);
                puts("'\n");
            }

            /*
             * Do we have more sentents remained in linebuf
             * separated by a ';' or a '\n' ?
             */
            if ( ret_parse == linebufp ) {
                /* No, we don't */
                break;
            } else {
                /* Yes, we have. We have to parse rest of lines,
                 * which begins with char* ret_parse; */
                linebufp = ret_parse;
            }
        }
    }

    io_close();
    return 0;
}



