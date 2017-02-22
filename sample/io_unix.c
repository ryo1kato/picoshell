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

int pico_getchar(void);
int pico_putchar(int c);
int pico_puts(const char* s);


static int tty;

/* To make sure terminal state to be restored when killed */
void sig_exit(int sig)
{
    pico_putchar('\n');
    (void) sig; /* unused */
    io_close();
    exit(EXIT_SUCCESS);
}

void io_open(void)
{
    /* disable terminal buffering, local echo, and Ctrl-C,
     * to emulate serial IO */
    pico_puts("Type 'exit' or press Ctrl-\\ to quit.\n");
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
    pico_puts("bye\n");
    system("stty intr ^C");
    system("stty echo");
}

int pico_getchar(void)
{
    char buf;
    read(tty, &buf, 1);
    return buf;
}

int pico_putchar(int c)
{
    char buf = c;
    write(tty, &buf, 1);
    return 0;
}

int pico_puts(const char* s)
{
    const char* c = s;
    while ( *c != '\0' ) {
        pico_putchar(*c);
        c++;
    }
    /* unlike libc standard pico_puts(), our pico_puts() doesn't put a trailing newline */
    /* pico_putchar('\n'); */
    return 1;
}
