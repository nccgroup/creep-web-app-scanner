#ifndef CREEP_H
#define CREEP_H

#endif /* CREEP_H */

#include <arpa/inet.h>
#include <errno.h>
#include <event.h>
#include <evhttp.h>
#include <getopt.h>
#include <netdb.h> /* hostent */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../src/creep.h"
#include "../src/def_error_messages.h"
#include "../src/def_sizes.h"

/* Globals */
Error error[DEF_SIZE_ERROR_INSTANCES];
struct hostent *he;
struct in_addr **addr;

/* Command line arguments */
char argDomain[2048];
char argIP[16];
char argPort[6];

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

   //DEF_ERROR_20 "Could not malloc"
   error[20].number = 20;
   strcpy(error[10].message, DEF_ERROR_20);
   error[20].log = 0x0;

   //DEF_ERROR_30 "IP address supplied is not a valid v4 address"
   error[30].number = 30;
   strcpy(error[30].message, DEF_ERROR_20);
   error[30].log = 0x0;

   //DEF_ERROR_31 "Domain is not valid"
   error[31].number = 31;
   strcpy(error[31].message, DEF_ERROR_20);
   error[31].log = 0x0;

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

int addPage(Target *target, char *URL)
{
   /* TODO Add page to page array. Page arg is wrong.. */
}

int searchPageForURLs(Target *target)
{
   /* TOOD Library which can search source for URLs.. */
   char URL[64] = "tmp";

   addPage(target, URL);
}

/* Interface for req to page struct */
int populatePage(Target *target)
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

void reqhandler(struct evhttp_request *req, Target *target)
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
   populatePage(target);
   
   /*
    *
    * SEARCH PAGE
    *
    */
   searchPageForURLs(target);

   /* Move forward */
   target->current_node = target->current_node->next_node;

   event_loopexit(NULL);
}

int crawl(Target *target) /* Parameters struct at some point? */
{
   /* Be careful about including test IPs here.. */
   //const char *addr = "127.0.0.1";

   printf("target->ip = %s\n", target->ip);
   printf("target->domain = %s\n", target->domain);
   printf("target->current_node->url = %s\n", target->current_node->url);

   const char *addr = target->ip;
   unsigned int port = 80;
   int i = 0, allPagesCrawled = 0;
   struct evhttp_connection *conn;
   struct evhttp_request *req, *req2;

   printf("initializing libevent subsystem..\n");
   event_init();

   conn = evhttp_connection_new(addr, port);
   /* TODO timeout spceifier as cmd arg */
   evhttp_connection_set_timeout(conn, 5);

   /*
      - Have some pages to crawl, need to populate pages[x]
      - Crawl and find more pages, need to add pages
      - Check all pages have been crawled, search pages?
   */

   do {
      //pagePtr = page[i];
      // reqhandler will popluate pages
      req = evhttp_request_new(reqhandler, target);
      evhttp_add_header(req->output_headers, "Host", target->domain);
      evhttp_add_header(req->output_headers, "Content-Length", "0");
      evhttp_make_request(conn, req, EVHTTP_REQ_GET, target->current_node->url);
      event_dispatch();
   } while(target->current_node->next_node != NULL);

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

int bootPages(Target *target)
{
   Page *prev_node_tmp;

   //printf("argDomain in bootPages == %s\n",argDomain);

   /* Page URL */
   strncpy(target->current_node->url,"/",DEF_SIZE_URL);

   //printf("target->current_node->url in bootPages == %s\n",target->current_node->url);

   /* First item in the list */
   target->current_node->prev_node = NULL;
   /* Last item in the list */
   target->current_node->next_node = malloc(sizeof(Page));
   /* Store current node position */
   prev_node_tmp = target->current_node;
   /* Move on to next node */
   target->current_node = target->current_node->next_node;
   /* Set next next node to nothing */
   target->current_node->next_node = NULL;
   /* Set current node's previous node to tmp node position */
   target->current_node->prev_node = prev_node_tmp;

   //strncat(argDomain,"/robots.txt", DEF_SIZE_URL);
   strncpy(target->current_node->url,"/robots.txt", DEF_SIZE_URL);

   /* Move back to the beginning */
   target->current_node = target->current_node->prev_node;

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

   /* Check funcs and fix bad var name. cmd_arg_url on second line includes /robots.txt */
   //strncat(argDomain,"/robots.txt", DEF_SIZE_URL);
   //strncpy(target->current_node.url, argDomain, DEF_SIZE_URL);
}

int assignArgs(int argc, char **argv)
{
   /* Flag set by '--verbose'. */
   static int verbose_flag;
     
   int c;
     
   while (1)
   {
      static struct option long_options[] =
      {
         /* These options set a flag. */
         {"verbose", no_argument,       &verbose_flag, 1},
         {"brief",   no_argument,       &verbose_flag, 0},
         /* These options don't set a flag.
            We distinguish them by their indices. */
         {"append",  no_argument,       0, 'b'},
         {"domain",  required_argument, 0, 'd'},
         {"ip",      required_argument, 0, 'i'},
         {"port",    required_argument, 0, 'p'},
         {"file",    required_argument, 0, 'f'},
         {0, 0, 0, 0}
      };

      /* getopt_long stores the option index here. */
      int option_index = 0;
     
      c = getopt_long(argc, argv, "bd:i:p:f:", long_options, &option_index);
     
      /* Detect the end of the options. */
      if (c == -1)
         break;
     
      switch (c)
      {
         case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
               break;

            printf ("option %s", long_options[option_index].name);
            if (optarg)
               printf (" with arg %s", optarg);
            printf ("\n");
            break;

         case 'd':
            printf ("option -d with value `%s'\n", optarg);
            strncpy(argDomain, optarg, DEF_SIZE_DOMAIN);
            break;

         case 'b':
            puts ("option -b\n");
            break;

         case 'i':
            printf ("option -i with value `%s'\n", optarg);
            strncpy(argIP, optarg, 16);
            break;

         case 'p':
            printf ("option -p with value `%s'\n", optarg);
            strncpy(argPort, optarg, 6);
            break;

         case 'f':
            printf ("option -f with value `%s'\n", optarg);
            break;

         case '?':
            /* getopt_long already printed an error message. */
            break;

         default:
            abort ();
      }
   }

   /* Instead of reporting '--verbose'
      and '--brief' as they are encountered,
      we report the final status resulting from them. */
   if (verbose_flag)
      puts ("verbose flag is set");
    
   /* Print any remaining command line arguments (not options). */
   if (optind < argc)
   {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
         printf ("%s ", argv[optind++]);
      putchar ('\n');
   }

   return 0;
}

int validateIP(char IP[16])
{
   struct sockaddr_in sa;
   int result = inet_pton(AF_INET, IP, &(sa.sin_addr));

   return result != 0;
}

int validateDomain(char domain[DEF_SIZE_DOMAIN])
{

   return 0;
}

int processArgs(Target *target)
{
   /*
      - Get args in structures
      - If IP not set then determine through supplied domain name
      - Validate args
         - Domain
         - IP

      - What if the IP is specified and the domain is not valid? It should probably pass..
   */

   if (argIP == NULL)
   {
      struct hostent *lh = gethostbyname(argDomain);

      if (lh)
      {
         puts(lh->h_name);
         strncpy(argIP, lh->h_name, 16);
      } else {
         herror("gethostbyname");
         exit(1); /* TODO fix this */
      }
   }

   /* Problem? */
   if (validateIP(argIP))
   {
      /* exit_error TODO */
   }

   /* Problem? */
   if (validateDomain(argDomain))
   {
      /* exit_error TODO */
   }

   strncpy(target->ip,argIP,16);
   strncpy(target->domain,argDomain,DEF_SIZE_DOMAIN);

   return 0;
}
