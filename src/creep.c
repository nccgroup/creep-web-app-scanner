#ifndef CREEP_H
#define CREEP_H

#endif /* CREEP_H */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <event.h>
#include <evhttp.h>
#include "../src/creep.h"
#include "../src/def_error_messages.h"
#include "../src/def_sizes.h"

/* Globals */
Error error[DEF_SIZE_ERROR_INSTANCES];
// Double linked list 
Page *page;

/*
 *
 *
 * ERROR FUNCTIONS
 *
 *
 */

int setup_error_messages(Error *error)
{
   /*typedef struct {
      int number;
      char message[64];
      uint8_t log;
   } Error;*/

   //DEF_ERROR_1 "Could not find error message for error number, you shouldn't really see this.."
   error[1].number = 1;
   strcpy(error[1].message, DEF_ERROR_1);
   error[1].log = 0x0;

   //DEF_ERROR_10 "Missing argument"
   error[10].number = 10;
   strcpy(error[10].message, DEF_ERROR_10);
   error[10].log = 0x0;

   return 0;
}

uint8_t exit_error(Error error)
{
   printf("Error %d: %s\n", error.number, error.message);

   exit(error.number);
}

/*
 *
 *
 * CRAWLING STUFF
 *
 *
 */

int addPage(Page page, char *URL)
{
   // TODO Add page to page array. Page arg is wrong..
}

int searchPageForURLs(Page page)
{
   // TOOD Library which can search source for URLs..
   char URL[64] = "tmp";

   addPage(page, URL);
}

// Interface for req to page struct
int populatePage(Page page)
{
   /*printf("in reqhandler. state == %s\n", (char *) state);
   if (req == NULL) {
       printf("timed out!\n");
   } else if (req->response_code == 0) {
       printf("connection refused!\n");
   } else if (req->response_code != 200) {
       printf("error: %u %s\n", req->response_code, req->response_code_line);
   } else {
       printf("success: %u %s\n", req->response_code, req->response_code_line);
   }*/
}

void reqhandler(struct evhttp_request *req, Page page)
{
   /*
      - Crawl and find more pages, need to add pages
      - Check all pages have been crawled, search pages?
   */

   /*
    *
    * POPULATE PAGE STRUCT
    *
    */
   populatePage(page);
   
   /*
    *
    * SEARCH PAGE
    *
    */
   searchPageForURLs(page);

   event_loopexit(NULL);
}

int crawl(Page *page) // Parameters struct at some point?
{
   // Be careful about including test IPs here..
   const char *addr = "127.0.0.1";
   unsigned int port = 80;
   int i = 0, allPagesCrawled = 0;
   struct evhttp_connection *conn;
   struct evhttp_request *req, *req2;
   Page *pagePtr;

   printf("initializing libevent subsystem..\n");
   event_init();

   conn = evhttp_connection_new(addr, port);
   // TODO timeout spceifier as cmd arg
   evhttp_connection_set_timeout(conn, 5);

   /*
      - Have some pages to crawl, need to populate pages[x]
      - Crawl and find more pages, need to add pages
      - Check all pages have been crawled, search pages?
   */

   while(allPagesCrawled != 1)
   {
      //pagePtr = page[i];
      // reqhandler will popluate pages
      req = evhttp_request_new(reqhandler, pagePtr);
      evhttp_add_header(req->output_headers, "Host", addr);
      evhttp_add_header(req->output_headers, "Content-Length", "0");
      evhttp_make_request(conn, req, EVHTTP_REQ_GET, "/");
      event_dispatch();
   }

   printf("starting event loop..\n");

   return 0;
}


/*
 *
 *
 * PAGE MAINTENANCE FUNCTIONS
 *
 *
 */

int bootPages(Page *page)
{
   page = malloc(sizeof(Page));

   if (page == NULL){
      exit_error(error[20]);
   }

/*   // Main page structure
   typedef struct {
      uint8_t crawled;
      int status_code;
      char *url;
      char *source_code;
      Findings findings;
      Page *prev_node;
      Page *next_node;
   } Page;*/

   // Check funcs and fix bad var name. cmd_arg_url on second line includes /robots.txt
   strncat(cmd_arg_url,"/robots.txt", DEF_SIZE_URL);
   strncpy(page->url, cmd_arg_url, DEF_SIZE_URL);
}

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

   bootPages(page);

   return 0;
}
