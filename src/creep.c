#ifndef CREEP_H
#define CREEP_H

#endif /* MONEY_H */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../src/creep.h"
#include "../src/def_error_messages.h"

int i;

int setup_error_messages(char **errorStr)
{
/*   for(i=0;i<255;i++)
   {
      strcpy(errorStr[i],'a');
   }*/
//   strcpy(errorStr[1][0],'a');

   return 0;
}

char *exit_message(int errorNum, char **errorStr)
{
   strcpy(errorStr[1],DEF_ERROR_1);
   strcpy(errorStr[10],DEF_ERROR_10);

   if (strlen(errorStr[errorNum]) == 0)
   {
      return errorStr[1];
   }

   printf("errorStr[errorNum] == %s\n", errorStr[1]);

   return errorStr[errorNum];
}

uint8_t exit_error(int errorNum)
{
   if (errorNum > 255 || errorNum < 0)
   {
      errorNum = 255;
   }

   printf("errorNum == %d\n", errorNum);

   return errorNum;
}
