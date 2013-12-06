#ifndef CREEP_H
#define CREEP_H

#endif /* MONEY_H */

#include <stdint.h>
#include <event.h>
#include <evhttp.h>
#include "../src/def_sizes.h"

typedef struct Error Error;
typedef struct Findings Findings;
typedef struct Parameters Parameters;
typedef struct Page Page;
typedef struct Target Target;

/* Application error */
struct Error {
   int number;
   char message[DEF_SIZE_ERROR_MESSAGE_LENGTH];
   uint8_t log;
};

/* Page findings */
struct Findings {
   char *comments[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *warns[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *fatals[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *errors[DEF_SIZE_FINDING_ARRAY_SIZE];
   char *tag_code[DEF_SIZE_FINDING_ARRAY_SIZE];
};

/* Page parameters */
struct Parameters {
   char *get;
   char *post;
   /* 64KB */
   char *cookies;
};

/* Main page structure */
struct Page {
   uint8_t crawled;
   int status_code;
   /* URL as is on the page, useful for not
      browsing to the same page multiple times*/
   char raw_url[DEF_SIZE_RAW_URL];
   /* TODO Check NULL byte issue str* functions should be DEF - 1? */
   char url[DEF_SIZE_URL];
   char *source_code;
   Findings findings;
   struct Page *prev_node;
   struct Page *next_node;
};

struct Target {
   char ip[16]; /* v4 only, get over it */
   char domain[DEF_SIZE_DOMAIN];
   struct Page *current_node;
   struct Page *first_node;
   struct Page *last_node;
};

/* Error functions */
int setup_error_messages(Error *error);
uint8_t exit_error(Error error);

/* Page functions */
int setup_error_messages(Error *error);
int addPage(Target *target, char *URL);
int searchPageForURLs(Target *target);
int populatePage(struct evhttp_request *req, Target *target);
void reqhandler(struct evhttp_request *req, void *target);
int crawl(Target *target); /* Parameters struct at some point? */
