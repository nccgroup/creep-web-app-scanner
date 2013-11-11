#ifndef CREEP_H
#define CREEP_H

#endif /* MONEY_H */

#include <stdint.h>
#include <event.h>
#include <evhttp.h>
#include "../src/def_sizes.h"

// Application error
typedef struct {
   int number;
   char message[DEF_SIZE_ERROR_MESSAGE_LENGTH];
   uint8_t log;
} Error;

// Page findings
typedef struct {
   char *comments[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *warns[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *fatals[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *errors[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *tag_code[DEF_SIZE_FINDING_ARRAY_SIZE];
} Findings;

// Page parameters
typedef struct {
   char *get;
   char *post;
   // 64KB
   char *cookies;
} Parameters;

// Main page structure
typedef struct {
   uint8_t crawled;
   int status_code;
   // TODO Check NULL byte issue str* functions should be DEF - 1?
   char url[DEF_SIZE_URL];
   char *source_code;
   Findings findings;
   struct Page *prev_node;
   struct Page *next_node;
} Page;

// Error functions
int setup_error_messages(Error *error);
uint8_t exit_error(Error error);

// Page functions
int setup_error_messages(Error *error);
int addPage(Page page, char *URL);
int searchPageForURLs(Page page);
int populatePage(Page page);
void reqhandler(struct evhttp_request *req, Page page);
int crawl(Page *page); // Parameters struct at some point?
