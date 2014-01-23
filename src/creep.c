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
#include <sys/queue.h>

#include "/usr/local/include/gumbo.h"

#include "../src/creep.h"
#include "../src/def_error_messages.h"
#include "../src/def_sizes.h"

#define DEF_SIZE_HEADER 128

/* Globals */
Error error[DEF_SIZE_ERROR_INSTANCES];
struct hostent *he;
struct in_addr **addr;

/* Command line arguments */
char argDomain[2048];
char argIP[16];
char argPort[6];
int argDebug = 0, argDomainSet = 0, argCrawlSubs = 0, argNoCrawl = 0;
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

int setup_error_messages()
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
   strcpy(error[30].message, DEF_ERROR_30);
   error[30].log = 0x0;

   //DEF_ERROR_31 "Domain is not valid"
   error[31].number = 31;
   strcpy(error[31].message, DEF_ERROR_31);
   error[31].log = 0x0;

   //DEF_ERROR_40 "Connection failed"
   error[40].number = 40;
   strcpy(error[40].message, DEF_ERROR_40);
   error[40].log = 0x0;

   //DEF_ERROR_50 "Could not resolve domain"
   error[50].number = 50;
   strcpy(error[50].message, DEF_ERROR_50);
   error[50].log = 0x0;

   return 0;
}

uint8_t exit_error(int x)
{
   printf("Error %d: %s\n", error[x].number, error[x].message);

   exit(error[x].number);
}

/*
 *
 *
 * CRAWLING STUFF
 *
 *
 */

/* Check domain in absolute URLs. Will add subdomain support at some point TODO */
/* Return 0 if URL is relative or absolute and in scope */
int checkDomain(char *url)
{
   char *domain = NULL;
   debugPrintf("checkDomain url == %s\n", url);

   /*
      * start with http? - check domain to second /
      * start with /? - relative, OK
      * start with something else? Probably relative, OK?
   */

   debugPrintf("checkDomain url and \"http://\" compare == %d\n", strncmp(url,"http://",7));

   /* Uppercase support? TODO */
   if (strncmp(url,"http://",7) == 0)
   {
      domain = &url[7];
   } else if (strncmp(url,"https://",8) == 0)
   {
      domain = &url[8];
   } else {
      debugPrintf("checkDomain url == %s\n", url);
      debugPrintf("checkDomain domain == %s\n", domain);
      return 0;
   }

   debugPrintf("checkDomain domain == %s\n", domain);
   debugPrintf("checkDomain argDomain == %s\n", argDomain);
   debugPrintf("checkDomain argDomain and domain compare == %d\n",strncmp(argDomain,domain,strnlen(argDomain,DEF_SIZE_DOMAIN)));
   if (strncmp(argDomain,domain,strnlen(argDomain,DEF_SIZE_DOMAIN)) == 0)
   {
      return 0;
   }

   return 1;
}

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
      /* Only care for URLs in scope */
      if (checkDomain((char *) attr->value) == 0)
      {
         debugPrintf("findURLs calling addPage\n");
         addPage(target,(char *) attr->value);
      }
   }

   if (root->v.element.tag == GUMBO_TAG_SCRIPT &&
      (attr = gumbo_get_attribute(&root->v.element.attributes, "src"))) {
      debugPrintf("script src == %s\n", attr->value);
      /* Only care for URLs in scope */
      if (checkDomain((char *) attr->value) == 0)
      {
         debugPrintf("findURLs calling addPage\n");
         addPage(target,(char *) attr->value);
      }
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
   target->current_node->headers_raw = malloc(DEF_SIZE_HEADERS); /* TODO check */
   char *line = malloc(DEF_SIZE_SOURCE_CODE_LINE); /* TODO check */

   debugPrintf("populatePage target == %s\n", (char *) target);
   if (req == NULL) {
      debugPrintf("timed out!\n");
   } else if (req->response_code == 0) {
      debugPrintf("connection refused!\n");
   } else if (req->response_code != 200) {
      debugPrintf("error: %u %s\n", req->response_code, req->response_code_line);
   } else {
      evbuffer_copyout(req->input_buffer, target->current_node->source_code, datalen);
      scrapeHeaders(req,target);
      debugPrintf("source? %s\n", target->current_node->source_code);
   }
}

int processRobotsTxt(Target *target)
{
   return 0;
}

//void reqhandler(struct evhttp_request *req, Target *target)
void reqhandler(struct evhttp_request *req, void *vTarget)
{
   /*
      - Crawl and find more pages, need to add pages
      - Check all pages have been crawled, search pages?
   */

   /* NOTE: No reentrant allowed (doHTTPRequest) */

   struct evhttp_uri *new_uri = NULL;
   const char *new_location = NULL;
   char cleanURLStr[DEF_SIZE_URL], relativeURLStr[DEF_SIZE_URL], newLocationStr[DEF_SIZE_URL];

   /* Typecast */
   Target *target = (Target *) vTarget;

   if (req == NULL)
   {
      exit_error(40);
   }

   /* Deal with robots.txt */
   if (!strncmp(target->current_node->url,"/robots.txt",DEF_SIZE_URL))
   {
      processRobotsTxt(target);
   }

   debugPrintf("reqhandler req == %d\n", req);
   debugPrintf("reqhandler req->response_code == %d\n", req->response_code);

   /* Sort redirects */
   switch(req->response_code)
   {
      case HTTP_OK:
      /* 
      * Response is received. No futher handling is required.
      * Finish
      */
      //event_base_loopexit(ctx->base, 0);
      break;

      case HTTP_MOVEPERM:
      case HTTP_MOVETEMP:
      new_location = evhttp_find_header(req->input_headers, "Location");
      strncpy(newLocationStr,new_location,DEF_SIZE_URL);

      strncpy(cleanURLStr,cleanURL(newLocationStr),DEF_SIZE_URL);
      strncpy(relativeURLStr,makeURLRelative(target,cleanURLStr),DEF_SIZE_URL);
      strncpy(target->current_node->url,relativeURLStr,DEF_SIZE_URL); // TODO check DEF
      if (!new_location)
         return;

      //strncpy(target->current_node->url,evhttp_uri_parse(new_location),DEF_SIZE_URL); // TODO check DEF
      if (!target->current_node->url)
      {
         return;
      }

      return;
   }

   /*
    *
    * POPULATE PAGE STRUCT
    *
    */
   populatePage(req,target);

   /*
    *
    * SCRAPE
    *
    */
   //scrapeComments(target);
   //scrapeMessages(target);
   
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

int doHTTPRequest(Target *target)
{
   /* TODO specify port as argument */
   unsigned int port = 80;
   int i = 0, allPagesCrawled = 0;
   struct evhttp_request *req, *req2;

   req = evhttp_request_new(reqhandler, target);
   evhttp_add_header(req->output_headers, "Host", target->domain);
   evhttp_add_header(req->output_headers, "Content-Length", "0");
   evhttp_make_request(target->libevent_conn, req, EVHTTP_REQ_GET, target->current_node->url);
   event_dispatch();

   return 0;
}

int crawl(Target *target) /* Parameters struct at some point? */
{
   Page *target_next_node;

   unsigned int port = 80;
   const char *addr = target->ip;

   debugPrintf("target->ip = %s\n", target->ip);
   debugPrintf("target->domain = %s\n", target->domain);
   debugPrintf("target->current_node->url = %s\n", target->current_node->url);
   debugPrintf("crawl while loop target->current_node->next_node == %x\n", target->current_node->next_node);

   debugPrintf("initializing libevent subsystem..\n");
   event_init();

   /*
      - Have some pages to crawl, need to populate pages[x]
      - Crawl and find more pages, need to add pages
      - Check all pages have been crawled, search pages?
   */

   /* TODO Free conns? */
   target->libevent_conn = evhttp_connection_new(addr, port);
   /* TODO timeout spceifier as cmd arg */
   evhttp_connection_set_timeout(target->libevent_conn, 5);

   do {
      /* Need this because last element will not be crawled otherwise.. */
      target_next_node = target->current_node->next_node;
      doHTTPRequest(target);
   } while((target->current_node != NULL) && (argNoCrawl == 0));

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
         debugPrintf("checkURLUnique strcmp(url,target->current_node->url) %d\n",strcmp(url,target->current_node->url));

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
char *makeURLRelative(Target *target, char *url)
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

   //debugPrintf("makeURLRelative target->current_node->url == %s\n", target->current_node->url);

   return path;
}

/* Remove # */
char *cleanURL(char *url)
{
   char *hashLocation;

   debugPrintf("cleanURL url before == %s\n",url);

   hashLocation = strnstr(url,"#",DEF_SIZE_URL);

   if (hashLocation == 0)
   {
      return url;
   }

   debugPrintf("cleanURL hashLocation == %s\n",hashLocation);

   *hashLocation = '\0';

   debugPrintf("cleanURL url after == %s\n",url);

   return url;
}

/* Returns 1 if header is 'boring' */
int scrapeHeadersSearch(struct evkeyval *header)
{
   debugPrintf("scrapeHeadersSearch header->key == %s\n", header->key);

   /* TODO Need to compare headers across requests to detect discrepencies.
      Maybe more than one box serving site. */
   /* Boring headers */
   if (strncmp(header->key,"Description",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Example",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Accept",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Accept-Charset",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Accept-Encoding",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Accept-Language",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Accept-Datetime",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Authorization",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Connection",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Cookie",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Content-Length",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Content-MD5",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Content-Type",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Date",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Expect",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"From",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Host",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"If-Match",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"If-Modified-Since",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"If-None-Match",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"If-Range",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Max-Forwards",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Origin",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Pragma",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Range",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Referrer",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"User-Agent",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Via",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Description",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Example",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"X-Forwarded-Proto",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Front-End-Https",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"X-ATT-DeviceId",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"X-Wap-Profile",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Proxy-Connection",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Vary",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Etag",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Transfer-Encoding",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"X-Cache-Action",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"X-Cache-Hits",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"X-Cache-Age",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"Cache-Control",DEF_SIZE_HEADER) == 0) return 1;
   if (strncmp(header->key,"X-LB-NoCache",DEF_SIZE_HEADER) == 0) return 1;

   return 0;
}

int scrapeHeaders(struct evhttp_request *req, Target *target)
{
   // TODO This code works but you need to sort the req stuff out
   struct evkeyvalq *evheaders;
   struct evkeyval *header;

   evheaders = evhttp_request_get_input_headers(req);

   TAILQ_FOREACH(header, evheaders, next)
   {
      /* Interesting header? */
      if (scrapeHeadersSearch(header) == 0)
      {
         debugPrintf("scrapeHeaders Interesting header? %s --- %s\n", header->key, header->value);
      }
   }

   return 0;
}

/* Add new node and copy URL to new node and then return to current node */
int addPage(Target *target, char *url)
{
   Page *old_current_node;

   debugPrintf("addPage url == %s\n",url);

   if (checkURLUnique(target, makeURLRelative(target,cleanURL(url))))
   {
      return 1;
   }

   debugPrintf("addPage checkURLUnique returned 0 for url %s\n\n", url);

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
   strncpy(target->current_node->url, makeURLRelative(target,cleanURL(url)),DEF_SIZE_URL);
   debugPrintf("addPage Added target->current_node->url == %s\n",target->current_node->url);
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

   debugPrintf("bootPages target->current_node->url == %s\n",target->current_node->url);

   if (argNoCrawl != 1)
   {
      addPage(target,"/robots.txt");
   }

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

int showHelp()
{
   printf("Help is (not) here\n");

   exit(1);
}

int assignArgs(int argc, char **argv)
{
   /* Flag set by '--verbose'. */
   static int argVerboseFlag;

   int c;
     
   while (1)
   {
      static struct option long_options[] =
      {
         /* These options set a flag. */
         {"verbose",     no_argument,       &argVerboseFlag, 1},
         {"debug",       no_argument,       &argDebug,       1},
         //{"brief",       no_argument,       &argVerboseFlag,    0},
         {"subdomain",   no_argument,       &argCrawlSubs,   1},
         {"no-crawl",    no_argument,       &argNoCrawl,   1},
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

      debugPrintf("assignArgs argc == %d\n", argc);

      /* domain is only required argument (ip can be determined via domain) */
      if (argc == 1)
      {
         /* Only binary called */
         showHelp();
      }
 
      /* Detect the end of the options. */
      if (c == -1)
         break;
     
      switch (c)
      {
         case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
               break;

            debugPrintf("option %s", long_options[option_index].name);
            if (optarg)
               debugPrintf(" with arg %s", optarg);
            debugPrintf("\n");
            break;

         case 'd':
            debugPrintf("option -d with value `%s'\n", optarg);
            strncpy(argDomain, optarg, DEF_SIZE_DOMAIN);
            argDomainSet = 1;
            break;

         case 'b':
            puts ("option -b\n");
            break;

         case 'i':
            debugPrintf("option -i with value `%s'\n", optarg);
            strncpy(argIP, optarg, 16);
            break;

         case 'p':
            debugPrintf("option -p with value `%s'\n", optarg);
            strncpy(argPort, optarg, 6);
            break;

         case 'f':
            debugPrintf("option -f with value `%s'\n", optarg);
            break;

         case '?':
            /* getopt_long already printed an error message. */
            break;

         default:
            debugPrintf("assignArgs abort to be called\n");
            abort();
      }
   }

   /* Instead of reporting '--verbose'
      and '--brief' as they are encountered,
      we report the final status resulting from them. */
   if (argVerboseFlag)
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

int resolveIP(char IP[16])
{
   struct hostent *he;
   struct in_addr **addr_list;
   int i;
      
   if ((he = gethostbyname(argDomain)) == NULL) 
   {
      /* Get host information */
      herror("gethostbyname");
      return 1;
   }

   addr_list = (struct in_addr **) he->h_addr_list;
   
   for(i=0;addr_list[i]!=NULL;i++) 
   {
      //Return the first one;
      strcpy(IP,inet_ntoa(*addr_list[i]));
      return 0;
   }
   
   return 1;
}

int validateIP(char IP[16])
{
   if (strnlen(IP,16) == 0)
   {
      if (resolveIP(IP))
      {
         /* Can't resolve? */
         exit_error(50);
      }
   }

   struct sockaddr_in sa;
   int result = inet_pton(AF_INET, IP, &(sa.sin_addr));

   return result != 0;
}

int validateDomain(char domain[DEF_SIZE_DOMAIN])
{

   return 0;
}

int showCrawledURLs(Target *target)
{
   Page *target_next_node;

   target->current_node = target->first_node;

   do {
      printf("URL added: %s\n",target->current_node->url);

      /* Need this because last element will not be crawled otherwise.. */
      target->current_node = target->current_node->next_node;

      target_next_node = target->current_node->next_node;
   } while(target->current_node != NULL);

   debugPrintf("Last node URL: %s\n",target->last_node->url);

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
      - What if the domain name and IP are not specified? Bail.
   */

   if (argDomainSet == 0)
   {
      showHelp();
   }

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
