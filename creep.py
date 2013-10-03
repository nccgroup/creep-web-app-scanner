#!/usr/bin/env python2.7

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

from lib import creep

def setUp(initialURL,options):
   # Base URL
   # Used for crawl creep check
   site = creep.Site(initialURL, options.outputPrefix)

   site.page.append(creep.Page(initialURL))
   site.page[0].url = initialURL
   site.page[0].pageType = 'directory'

   site.page.append(creep.Page(initialURL))
   site.page[1].url = initialURL + 'robots.txt'
   site.page[1].pageType = 'file'

   # Speculative URLs
   speculativeURLs = creep.Site(initialURL, options.outputPrefix)

   count = 0

   return site, speculativeURLs

def exitError(errorNum):
   print "Problemo senor"
   exit(errorNum)

def start():
   options, args, parser = creep.usage()

   if creep.checkUsage(options, args, parser) == 1:
      exitError(1)
   site, speculativeURLs = setUp(args[0],options)

   bashFile='data/files-quick.txt'

   creep.startCrawl(site, speculativeURLs, bashFile)

   return site

##
## GOGOGO
##
site = start()

for i in site.page:
   if i.crawled == 1 and i.status_code != 404:
      print 'url         ' + i.url
      print 'crawled?    ' + str(i.crawled)
      print 'status_code ' + str(i.status_code)
      print 'paths       ' + str(i.findings['paths'])
      print 'source_code ' + 'omitted' #str(i.findings['source_code'])
      print 'comments    ' + str(i.findings['comments'])
      print 'warns       ' + str(i.findings['errors']['warns'])
      print 'fatals      ' + str(i.findings['errors']['fatals'])
      print 'errors      ' + str(i.findings['errors']['errors'])
      print 'tag_code    ' + str(i.findings['tag_code'])
      print 'get         ' + str(i.findings['parameters']['get'])
      print 'post        ' + str(i.findings['parameters']['post'])
      print '# '*10
