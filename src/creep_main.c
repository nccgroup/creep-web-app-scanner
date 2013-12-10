#ifndef CREEP_H
#define CREEP_H

#endif /* CREEP_H */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <event.h>
#include <evhttp.h>
#include <getopt.h>
#include "../src/creep.h"
#include "../src/def_error_messages.h"
#include "../src/def_sizes.h"

/*
 *
 *
 * MAIN FUNCTION
 *
 *
 */

int main(int argc, char *argv[])
{
   /*
    - Check cmd args
    - Start loop
    - Populate files
    */

   /* Double linked list */
   Target target;
   target.first_node = malloc(sizeof(Page)); /* TODO check */
   target.current_node = target.first_node;

   assignArgs(argc,argv);
   processArgs(&target);
   bootPages(&target);
   crawl(&target);

   return 0;
}
