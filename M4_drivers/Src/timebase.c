#include "timebase.h""
#include "stm32f4xx.h"


#define ONE_SEC_LOAD       16000000

#define CTRL_ENABLE        (1<<0)
#define CTRL_TICKINT       (1<<8)
#define CTRL_CLCKSRC       (1<<2)
#define CTRL_COUNTFLAG     (1<<16)



void timebase_init(void)
{

}
