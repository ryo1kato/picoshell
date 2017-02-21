#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "picoshell_termesc.h"
#include "history.h"


#ifdef MSH_CONFIG_ENABLE_BELL
#define ring_terminal_bell() pico_putchar('\a');
#else
#define ring_terminal_bell() /* disable */
#endif



/*
 * cmdline_t class and its methods (in subtle Object Oriented flavor).
 */
typedef struct cmdline_struct {
    char buf[MSH_CMDLINE_CHAR_MAX];
    int  pos;     /* cursor position (start at 1 orignin, 0 means empty line) */
    int  linelen; /* length of input char of line EXCLUDING trailing null */
#   ifdef MSH_CONFIG_CLIPBOARD
    /* The buffer used for Cut&Paste */
    char clipboard[MSH_CMDLINE_CHAR_MAX];
#   endif
} cmdline_t;

static cmdline_t CmdLine;
static int  bCmdLineInitialized;

/** cmdline_clear()
 * Empty the cmdline buffer, but not displayed line.
 */
static void
cmdline_clear( cmdline_t* pcmdline )
{
    memset(pcmdline->buf, '\0', MSH_CMDLINE_CHAR_MAX);
    pcmdline->pos     = 0;
    pcmdline->linelen = 0;
}

/** cmdline_init()
 * Initialize cmdline_t object. Call this before fast use.
 */
static void
cmdline_init( cmdline_t* pcmdline )
{
    cmdline_clear( pcmdline );
#   ifdef MSH_CONFIG_LINEEDIT
    memset(pcmdline->clipboard, '\0', MSH_CMDLINE_CHAR_MAX);
#   endif
}


static char* prompt_string = MSH_CMD_PROMPT;
void msh_set_prompt(char* str)
{
    prompt_string = str;
}

/* ************************************************************************* *
 *     Basic Line Edit Functions (Enabled regardless of MSH_CONFIG_LINEEDIT)
 * ************************************************************************* */

/** cmdline_kill()
 * Ctrl-U of standard terminal
 */
static void
cmdline_kill( cmdline_t* pcmdline )
{
    int i;
    for ( i = 0;  i < pcmdline->pos;  i++ ) {
        pico_putchar('\b');
    }
    for ( i = 0;  i < pcmdline->linelen;  i++ ) {
        pico_putchar(' ');
    }
    for ( i = 0;  i < pcmdline->linelen;  i++ ) {
        pico_putchar('\b');
    }
    cmdline_clear( pcmdline );
}


/** cmdline_set()
 * Discard current cmdline and set it to specified string
 */
static void
cmdline_set( cmdline_t* pcmdline, const char* str )
{
    int len;

    cmdline_kill(pcmdline);
    len = strlen(str);
    strcpy( pcmdline->buf, str );
    pico_puts( str );
    pcmdline->pos     = len;
    pcmdline->linelen = len;
}


/** cmdline_insert_char()
 * Insert (or append) a charactor 'c' at current cursor position.
 */
static int
cmdline_insert_char( cmdline_t* pcmdline, unsigned char c )
{
    /* Check if the line buffer can hold another one char */
    if ( pcmdline->linelen >= MSH_CMDLINE_CHAR_MAX - 1 ) {
        /* buffer is full */
        ring_terminal_bell();
        return 0;
    }

    pico_putchar(c);
    /* Is cursor at the end of the cmdline ? */
    if ( pcmdline->pos == pcmdline->linelen ) {
        /* just append */
        pcmdline->buf[ pcmdline->pos ] = c;
    } else {
        /* slide the strings after the cursor to the right */
        int i;
        pico_puts(  & pcmdline->buf[ pcmdline->pos ]  );
        for (i = pcmdline->linelen;  i > pcmdline->pos;  i--) {
            pcmdline->buf[ i ] = pcmdline->buf[ i - 1 ];
            pico_putchar('\b');
        }
        pcmdline->buf[ pcmdline->pos ] = c;
    }
    pcmdline->pos++;
    pcmdline->linelen++;
    pcmdline->buf[ pcmdline->linelen ] = '\0'; /* just for safty */
    return 1;
}


/** cmdline_backspace()
 * Delete a charactor at left of the cursor and
 * slide rest of strings to the left.
 */
static int
cmdline_backspace( cmdline_t* pcmdline )
{
    if ( pcmdline->pos <= 0 ) {
        ring_terminal_bell();
        return 0;
    }
    pico_putchar('\b');
    /* Is cursor at the end of the cmdline ? */
    if ( pcmdline->pos == pcmdline->linelen ) {
        pico_putchar(' ');
        pico_putchar('\b');
    } else {
        int i;
        /* slide the characters after cursor position to the left */
        for ( i = pcmdline->pos;  i < pcmdline->linelen;  i++ ) {
            pcmdline->buf[i-1] = pcmdline->buf[i];
            pico_putchar( pcmdline->buf[i] );
        }
        pico_putchar(' ');
        /* put the cursor to its orignlal position */
        /* +1 in for () is for pico_putchar(' ') in the previous line */
        for ( i = pcmdline->pos;  i < pcmdline->linelen + 1;  i++ ) {
            pico_putchar('\b');
        }
    }
    pcmdline->buf[ pcmdline->linelen - 1 ] = '\0';
    pcmdline->pos--;
    pcmdline->linelen--;
    return 1;
}


/** cmdline_delete()
 * Delete a charactor on the cursor and slide rest of strings to the left.
 * Cursor position doesn't change, unlike cmdline_backspace().
 */
static int
cmdline_delete( cmdline_t* pcmdline )
{
    if ( pcmdline->linelen <= pcmdline->pos ) {
        /* No more charactors to delete.
         * i.e, cursor is the rightmost pos of the line.  */
        ring_terminal_bell();
        return 0;
    }
    else
    {
        int i;
        /* slide the chars on and after cursor position to the left */
        for ( i = pcmdline->pos;  i < pcmdline->linelen - 1;  i++ ) {
            pcmdline->buf[i] = pcmdline->buf[ i + 1 ];
            pico_putchar( pcmdline->buf[i] );
        }
        pico_putchar(' ');
        /* put the cursor to its orignlal position */
        for ( i = pcmdline->pos;  i < pcmdline->linelen;  i++ ) {
            pico_putchar('\b');
        }

    }
    pcmdline->buf[ pcmdline->linelen - 1 ] = '\0';
    pcmdline->linelen--;
    return 1;
}



#if 0
/** cmdline_get_lastchar()
 * Get the last input charactor to the cmdline buffer.
 * Maybe usefull for '\' escaping.
 */
static unsigned char
cmdline_get_lastchar( cmdline_t* pcmdline )
{
    if ( pcmdline->pos == 0 ) {
        return '\0';
    } else {
        return pcmdline->buf[ pcmdline->pos - 1 ];
    }
}
#endif


/* ************************************************************************* *
 *     Line Edit Functions
 * ************************************************************************* */
#ifdef MSH_CONFIG_LINEEDIT
/** cmdline_cursor_left()
 ** cmdline_cursor_right()
 * move a cursor on the comdline left and right;
 */
static int
cmdline_cursor_left( cmdline_t* pcmdline )
{
    if ( pcmdline->pos > 0 ) {
        pico_putchar('\b');
        pcmdline->pos--;
        return 1;
    }
    else
    {
        ring_terminal_bell();
        return 0;
    }
}

static int
cmdline_cursor_right( cmdline_t* pcmdline )
{
    if ( pcmdline->pos < pcmdline->linelen ) {
        pico_putchar( pcmdline->buf[pcmdline->pos++] );
        return 1;
    }
    else
    {
        ring_terminal_bell();
        return 0;
    }
}

/** cmdline_cursor_linehead()
 ** cmdline_cursor_linetail()
 * Move the cursor to head/tail of the line.
 */
static void
cmdline_cursor_linehead( cmdline_t* pcmdline )
{
    while ( pcmdline->pos > 0 ) {
        pico_putchar('\b');
        pcmdline->pos--;
    }
}

static void
cmdline_cursor_linetail( cmdline_t* pcmdline )
{
    while ( pcmdline->pos < pcmdline->linelen ) {
        pico_putchar( pcmdline->buf[pcmdline->pos++] );
    }
}
#endif/*MSH_CONFIG_LINEEDIT*/


#ifdef MSH_CONFIG_CLIPBOARD
/** cmdline_yank()
 * Insert clipboard string into current cursor pos
 */
static void
cmdline_yank( cmdline_t* pcmdline )
{
    if ( strlen(pcmdline->clipboard) == 0 ) {
        /* no string in the clipboard */
        ring_terminal_bell();
    } else {
        int i = 0;
        while ( pcmdline->clipboard[ i ] != '\0'
                && cmdline_insert_char( pcmdline, pcmdline->clipboard[i] ) )
        {
                i++;
        }
    }
}

/** cmdline_killtail()
 * kill characters on and right of the cursor and
 * copy them into the clipboard buffer.
 */
static void
cmdline_killtail( cmdline_t* pcmdline )
{
    int i;
    if ( pcmdline->pos == pcmdline->linelen ) {
        /* nothing to kill */
        ring_terminal_bell();
    }

    /* copy chars on and right of the cursor to the clipboar */
    strcpy( pcmdline->clipboard, &pcmdline->buf[pcmdline->pos] );

    /* erase chars on and right of the cursor on terminal */
    for ( i = pcmdline->pos;  i < pcmdline->linelen; i++ ) {
        pico_putchar(' ');
    }
    for ( i = pcmdline->pos;  i < pcmdline->linelen; i++ ) {
        pico_putchar('\b');
    }

    /* erase chars on and right of the cursor in buf */
    pcmdline->buf[pcmdline->pos] = '\0';
    pcmdline->linelen = pcmdline->pos;
}
/** cmdline_killword()
 * kill a (part of) word on and left of the cursor and
 * copy them into the clipboard buffer.
 */
static void
cmdline_killword( cmdline_t* pcmdline )
{
    int i, j;
    if ( pcmdline->pos == 0 ) {
        ring_terminal_bell();
        return ;
    }
    /* search backward for a word to kill */
    i = 0;
    while( i < pcmdline->pos
           && pcmdline->buf[ pcmdline->pos - i - 1 ] == ' ' ) {
        i++;
    }
    while( i < pcmdline->pos
           && pcmdline->buf[ pcmdline->pos - i - 1 ] != ' ' ) {
        i++;
    }

    /* copy the word to clipboard */
    j = 0;
    while ( j < i ) {
        pcmdline->clipboard[ j ] = pcmdline->buf[ pcmdline->pos - i + j ];
        j++;
    }
    pcmdline->clipboard[ j ] = '\0';

    /* kill the word */
    j = 0;
    while ( j < i ) {
        cmdline_backspace( pcmdline );
        j++;
    }
}

#endif /*MSH_CONFIG_CLIPBOARD*/




/*
 * Input a char at the current cursor position.
 * Or move cursor, retrieve command history etc, if Ctrl-X
 *
 * Returns false (=0) when input terminated (by Enter)
 */
#ifdef MSH_CONFIG_CMDHISTORY
    /*
     * Current active history number
     *   Notice: unlinke 'histnum' in history.c, zero value for this histnum means
     *   CURRENT LINE, not the first history. (offset -1)
     */
    static int histnum;
    /* temporary buffer to hold current line */
    char curline[MSH_CMDLINE_CHAR_MAX];
    /* temporary buffer to hold history */
    const char* histline;
#endif
static int
cursor_inputchar( cmdline_t* pcmdline, unsigned char c )
{
    unsigned char input = c;

    /* QUICK HACK
     * Map escape sequences (Arrow keys) to other bind
     */
    if (input == '\033' ) {
        char second, third;
        second = pico_getchar();
        third = pico_getchar();
        if ( second == '[' ) {
            switch (third) {
            case 'A':
                input = MSH_KEYBIND_HISTPREV;
                break;
            case 'B':
                input = MSH_KEYBIND_HISTNEXT;
                break;
            case 'C':
                input = MSH_KEYBIND_CURRIGHT;
                break;
            case 'D':
                input = MSH_KEYBIND_CURLEFT;
                break;
            default:
                ;
                /* do nothing */
            }
        } else {
            /* do nothing */
        }
    }


    switch (input) {
        /*
         * End of input if newline char.
         */
        case MSH_KEYBIND_ENTER:
            pico_putchar('\n');
            return 0;

        case '\t':
            /* tab sould be comverted to a space */
            cmdline_insert_char( pcmdline, ' ');
            break;

        case MSH_KEYBIND_DISCARD:
            cmdline_clear(pcmdline);
            pico_putchar('\n');
            return 0;

        case MSH_KEYBIND_BACKSPACE:
            cmdline_backspace(pcmdline);
            break;

        case MSH_KEYBIND_DELETE:
        case 0x7F: /* ASCII DEL.  Should be used as BS ?*/
            cmdline_delete(pcmdline);
            break;

        case MSH_KEYBIND_CLEAR:
            cmdline_cursor_linehead(pcmdline);
            pico_puts(TERMESC_CLEAR);
            pico_puts(prompt_string);
            cmdline_cursor_linetail(pcmdline);
            break;

#ifdef MSH_CONFIG_LINEEDIT
        case MSH_KEYBIND_KILLLINE:
            cmdline_kill(pcmdline);
            break;

        case MSH_KEYBIND_CURLEFT:
            cmdline_cursor_left(pcmdline);
            break;

        case MSH_KEYBIND_CURRIGHT:
            cmdline_cursor_right(pcmdline);
            break;

        case MSH_KEYBIND_LINEHEAD:
            cmdline_cursor_linehead(pcmdline);
            break;

        case MSH_KEYBIND_LINETAIL:
            cmdline_cursor_linetail(pcmdline);
            break;

#endif /*MSH_CONFIG_LINEEDIT*/
#ifdef MSH_CONFIG_CLIPBOARD
        case MSH_KEYBIND_YANK:
            cmdline_yank(pcmdline);
            break;

        case MSH_KEYBIND_KILLTAIL:
            cmdline_killtail(pcmdline);
            break;

        case MSH_KEYBIND_KILLWORD:
            cmdline_killword(pcmdline);
            break;
#endif /*MSH_CONFIG_CLIPBOARD*/

#ifdef MSH_CONFIG_CMDHISTORY
        case MSH_KEYBIND_HISTPREV:
            if ( histnum == 0 ) {
                /* save current line before overwrite with history */
                strcpy(curline, pcmdline->buf);
            }
            histline = history_get(histnum);
            if ( histline != NULL ) {
                cmdline_set(pcmdline, histline);
                histnum++;
            } else {
                ring_terminal_bell();
            }
            break;

        case MSH_KEYBIND_HISTNEXT:
            if ( histnum == 1 ) {
                histnum = 0;
                cmdline_set(pcmdline, curline);
            }
            else
            if ( histnum > 1 )  {
                histline = history_get(histnum-2);
                if ( histline != NULL ) {
                    cmdline_set(pcmdline, histline);
                    histnum--;
                } else {
                    ring_terminal_bell(); /* no newer hist */
                }
            } else {
                ring_terminal_bell(); /* invalid (negative) histnum value */
            }

            break;
#endif /*MSH_CONFIG_CMDHISTORY*/

        default:
            if ( isprint(c) ) {
                if ( pcmdline->pos  <  MSH_CMDLINE_CHAR_MAX - 1 ) {
                    cmdline_insert_char( pcmdline, c );
                }
            }
            break;
    }

    return 1 /*true*/;
}


int msh_get_cmdline(char* linebuf)
{
    if ( ! bCmdLineInitialized ) {
        cmdline_init( &CmdLine );
        bCmdLineInitialized = 1; /* true */
    } else {
        cmdline_clear( &CmdLine );
    }
    pico_puts(prompt_string);

    while ( cursor_inputchar( &CmdLine, pico_getchar() ) )
        ;

#ifdef MSH_CONFIG_CMDHISTORY
    history_append(CmdLine.buf);
    histnum = 0; /* reset active histnum */
#endif

    strcpy(linebuf, CmdLine.buf);
    return ( strlen( CmdLine.buf ) );
}



#ifdef TEST
#include "test.h"
int main(void)
{
    /*
     * Oh! Please write the test code!!
     */
    return 0;
}
#endif
