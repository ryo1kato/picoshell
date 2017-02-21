#ifndef __MSH_HISTORY_H_INCLUDED__
#define __MSH_HISTORY_H_INCLUDED__

#include "picoshell_config.h"

#ifdef MSH_CONFIG_CMDHISTORY


#ifndef MSH_CMD_HISTORY_MAX
#define MSH_CMD_HISTORY_MAX (32)
#endif



/*
 *  Add a 'line' to the command line history ring-buffer.
 *  If MSH_CMD_HISTORY_MAX exceeds, it overwrites the most old history
 *  If 'line' it too long (> MSH_CMD_HISTORY_MAX-1), just ignore.
 */
void history_append(const char* line);

/*
 * Retrieve histnum-th histroy.
 *
 *
 *
 */
const char* history_get(int histnum);



#endif /*MSH_CONFIG_CMDHISTORY*/
#endif /*__MSH_HISTORY_H_INCLUDED__*/

