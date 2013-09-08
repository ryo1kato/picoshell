#include "history.h"
#ifdef MSH_CONFIG_CMDHISTORY
#include <string.h>
#include <stdbool.h>

/*
 * Easy, we have only one history, the global history.
 */
static char history[MSH_CMD_HISTORY_MAX][MSH_CMDLINE_CHAR_MAX];
static bool histfull;
static int  histlast;


void history_append(const char* line)
{
    /* If a given string is too long or zero-length, just ignore.*/
    int len = strlen(line);
    if ( len >= MSH_CMDLINE_CHAR_MAX || len <= 0) {
        return;
    }

    strcpy(history[histlast], line);

    if ( histlast >= MSH_CMD_HISTORY_MAX - 1 ) {
        histfull = true;
        histlast = 0;
    } else {
        histlast++;
    }
}


const char* history_get(int histnum)
{
    if ( ! histfull ) {
        if ( histnum >= histlast ) {
            return NULL;
        }
    }
    else
    if ( histnum > MSH_CMD_HISTORY_MAX - 1 || histnum < 0 ) {
        return NULL;
    }

    if ( histlast > histnum ) {
        return ( history[histlast - histnum - 1] );
    } else {
        return history[ MSH_CMD_HISTORY_MAX - (histnum - histlast)  - 1];
    }
}



#ifdef TEST
#include "test.h"
int main(void)
{
    return 0;
}
#endif
#endif/*MSH_CONFIG_CMDHISTORY*/
