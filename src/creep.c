#ifndef CREEP_H
#define CREEP_H

#endif /* CREEP_H */

#include <arpa/inet.h>
#include <assert.h>
#include <bsd/string.h>
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

#include "/usr/local/include/gumbo.h"

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
int argDebug = 0;
char path[DEF_SIZE_URL];

/*
 *
 *
 * DEBUG FUNCTIONS
 *
 *
 */

int debugPrintf(char msg[DEF_SIZE_DEBUG_MESSAGE], ...)
{
   if (argDebug == 1)
   {
      va_list args;
      va_start(args, msg);

      vprintf(msg, args);

      va_end(args);
   }

   return 0;
}

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

const char *findURLs(Target *target, GumboNode *root)
{
   int i;
   if (root->type != GUMBO_NODE_ELEMENT) {
      return;
   }

   GumboAttribute* attr;
   if (root->v.element.tag == GUMBO_TAG_A &&
      (attr = gumbo_get_attribute(&root->v.element.attributes, "href"))) {
      debugPrintf("a href == %s\n", attr->value);
      addPage(target, (char *) attr->value);
   }

   if (root->v.element.tag == GUMBO_TAG_SCRIPT &&
      (attr = gumbo_get_attribute(&root->v.element.attributes, "src"))) {
      debugPrintf("script src == %s\n", attr->value);
      addPage(target, (char *) attr->value);
   }

   GumboVector* children = &root->v.element.children;
   for (0; i < children->length; ++i) {
      findURLs(target, children->data[i]); /* Recursive... */
   }
}

int searchPageForURLs(Target *target)
{
   GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, target->current_node->source_code,
                         strnlen(target->current_node->source_code, DEF_SIZE_SOURCE_CODE));

   //const char* title = find_title(output->root);
   //printf("%s\n", title);
   //gumbo_destroy_output(&kGumboDefaultOptions, output);
   //free(input);

   findURLs(target, output->root);
}

/* Interface for req to page struct */
int populatePage(struct evhttp_request *req, Target *target)
{
   size_t datalen = DEF_SIZE_SOURCE_CODE; /* Max source code size */
   target->current_node->source_code = malloc(DEF_SIZE_SOURCE_CODE); /* TODO check */
   char *line = malloc(DEF_SIZE_SOURCE_CODE_LINE); /* TODO check */

   //debugPrintf("in reqhandler. state == %s\n", (char *) target);
   if (req == NULL) {
      //debugPrintf("timed out!\n");
   } else if (req->response_code == 0) {
      //debugPrintf("connection refused!\n");
   } else if (req->response_code != 200) {
      //debugPrintf("error: %u %s\n", req->response_code, req->response_code_line);
   } else {
      evbuffer_copyout(req->input_buffer, target->current_node->source_code, datalen);
      //debugPrintf("source? %s\n", target->current_node->source_code);
   }
}

//void reqhandler(struct evhttp_request *req, Target *target)
void reqhandler(struct evhttp_request *req, void *vTarget)
{
   /*
      - Crawl and find more pages, need to add pages
      - Check all pages have been crawled, search pages?
   */

   /* Typecast */
   Target *target = (Target *) vTarget;

   /*
    *
    * POPULATE PAGE STRUCT
    *
    */
   populatePage(req,target);
   
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
   debugPrintf("target->ip = %s\n", target->ip);
   debugPrintf("target->domain = %s\n", target->domain);
   debugPrintf("target->current_node->url = %s\n", target->current_node->url);
   debugPrintf("crawl while loop target->current_node->next_node == %x\n", target->current_node->next_node);

   const char *addr = target->ip;
   unsigned int port = 80;
   int i = 0, allPagesCrawled = 0;
   struct evhttp_connection *conn;
   struct evhttp_request *req, *req2;
   Page *target_next_node;

   debugPrintf("initializing libevent subsystem..\n");
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
      /* Need this because last element will not be crawled otherwise.. */
      target_next_node = target->current_node->next_node;
      req = evhttp_request_new(reqhandler, target);
      evhttp_add_header(req->output_headers, "Host", target->domain);
      evhttp_add_header(req->output_headers, "Content-Length", "0");
      evhttp_make_request(conn, req, EVHTTP_REQ_GET, target->current_node->url);
      event_dispatch();
   } while(target_next_node != NULL);

   debugPrintf("starting event loop..\n");

   return 0;
}

/*
 *
 *
 * PAGE MAINTENANCE FUNCTIONS
 *
 *
 */

int checkURLUnique(Target *target, char url[DEF_SIZE_URL])
{
   Page *old_current_node = target->current_node;

   /* Let's go back to the beginning */
   target->current_node = target->first_node;

   /* While not at the end of the list */
   //while(target->current_node->next_node != NULL)
   while(target->current_node != NULL)
   {
      if (strcmp(url,target->current_node->url) == 0)
      {
         printf("checkURLUnique strcmp(url,target->current_node->url) %d\n",strcmp(url,target->current_node->url));

         debugPrintf("checkURLUnique Not adding %s because %s ### found in ### %s\n",url,url,target->current_node->url);
         /* Return to the node we were on */
         target->current_node = old_current_node;

         /* URL found, ditch */
         return 1;
      }

      /* Next node */
      target->current_node = target->current_node->next_node;
   }

   /* Return to the "old current" node */
   target->current_node = old_current_node;

   return 0;
}

/* Will make URL relative if it is not */
char *makeURLRelative(Target *target, char url[DEF_SIZE_URL])
{
   /* TODO Review DEF_SIZE_URL name, it's actually referring
           to the path, not the full URL */

   /* printf("makeURLRelative url == %s\n", url);
   printf("makeURLRelative strnstr / working == %d\n", strnstr(url,"/",DEF_SIZE_URL) - url);
   printf("makeURLRelative strnstr http working == %d\n", strnstr(url,"http",DEF_SIZE_URL) - url);
   printf("checkURLRelative target == %s\n", url);
   printf("checkURLRelative strnstr == %s\n", strnstr(url,"http",DEF_SIZE_URL));
   printf("checkURLRelative strnstr str offset == %d\n",strnstr(url,"http",DEF_SIZE_URL) - url); */

   /* Starts with http */
   if ((strnstr(url,"http",DEF_SIZE_URL) - url) == 0)
   {
      /* Pull path, 3rd /
         + 8 to account for https:// */
      strncpy(path,strnstr((url + 8),"/",DEF_SIZE_URL),DEF_SIZE_URL);
      debugPrintf("makeURLRelative path == %s\n", path);
      //strncpy(target->current_node->url,path,DEF_SIZE_URL);
   /* Does not start with / ? Then add */
   } else if ((strnstr(url,"/",DEF_SIZE_URL) - url) != 0)
   {
      //strncpy(target->current_node->url,"/",DEF_SIZE_URL);
      //strncat(target->current_node->url,path,DEF_SIZE_URL);
      strncpy(path,"/",DEF_SIZE_URL);
      strncat(path,url,DEF_SIZE_URL);
   /* URL is relative, move on */
   } else {
      //strncpy(target->current_node->url,url,DEF_SIZE_URL);
      strncpy(path,url,DEF_SIZE_URL);
   }

   debugPrintf("makeURLRelative target->current_node->url == %s\n", target->current_node->url);

   return path;
}

/* Remove # */
char *cleanURL(char url[DEF_SIZE_URL])
{
   char *hashLocation;

   printf("cleanURL url before == %s\n",url);

   hashLocation = strnstr(url,"#",DEF_SIZE_URL);

   if (hashLocation == 0)
   {
      return url;
   }

   printf("cleanURL hashLocation == %s\n",hashLocation);

   *hashLocation = '\0';

   printf("cleanURL url after == %s\n",url);

   return url;
}

/* Add new node and copy URL to new node and then return to current node */
int addPage(Target *target, char url[DEF_SIZE_URL])
{
   Page *old_current_node;

   if (checkURLUnique(target, makeURLRelative(target,cleanURL(url))))
   {
      return 1;
   }

   printf("addPage checkURLUnique returned 0 for url %s\n\n", url);

   /* Store current node position */
   old_current_node = target->current_node;

   /* Get to the end of the list */
   target->current_node = target->last_node;

   /* Last item in the list */
   target->current_node->next_node = malloc(sizeof(Page)); /* TODO check */
   /* Move on to next node */
   target->current_node = target->current_node->next_node;
   /* Set next next node to nothing */
   target->current_node->next_node = NULL;
   /* Set current node's previous node to tmp node position */
   target->current_node->prev_node = old_current_node;
   /* Copy url to new node */
                                      /* Fix absolute URLs | Remove # */
   strncpy(target->current_node->url, makeURLRelative(target,cleanURL(url)), DEF_SIZE_URL);
   printf("addPage Added target->current_node->url == %s\n",target->current_node->url);
   /* Set the last node */
   target->last_node = target->current_node;
   /* Set current node to old_current_node */
   target->current_node = target->current_node->prev_node;

   return 0;
}

int bootPages(Target *target)
{
   /* First item in the list */
   target->current_node->prev_node = NULL;

   /* Set last node to current node */
   target->last_node = target->current_node;

   /* Page URL */
   strncpy(target->current_node->url,"/",DEF_SIZE_URL);

   debugPrintf("target->current_node->url in bootPages == %s\n",target->current_node->url);

   addPage(target,"/robots.txt");
   debugPrintf("bootPages current_node url == %s\n", target->current_node->url);

   /* Move back to the beginning */
   target->current_node = target->first_node;
   debugPrintf("addPage target->current_node->next_node == %x\n", target->current_node->next_node);
   debugPrintf("addPage target->current_node->url == %s\n", target->current_node->url);

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
         {"debug",   no_argument,       &argDebug,     1},
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

            debugPrintf ("option %s", long_options[option_index].name);
            if (optarg)
               debugPrintf (" with arg %s", optarg);
            debugPrintf ("\n");
            break;

         case 'd':
            debugPrintf ("option -d with value `%s'\n", optarg);
            strncpy(argDomain, optarg, DEF_SIZE_DOMAIN);
            break;

         case 'b':
            puts ("option -b\n");
            break;

         case 'i':
            debugPrintf ("option -i with value `%s'\n", optarg);
            strncpy(argIP, optarg, 16);
            break;

         case 'p':
            debugPrintf ("option -p with value `%s'\n", optarg);
            strncpy(argPort, optarg, 6);
            break;

         case 'f':
            debugPrintf ("option -f with value `%s'\n", optarg);
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
      debugPrintf ("non-option ARGV-elements: ");
      while (optind < argc)
         debugPrintf ("%s ", argv[optind++]);
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
