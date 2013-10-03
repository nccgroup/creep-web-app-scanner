#!/usr/bin/env python2.7

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

import optparse
from bs4 import BeautifulSoup, Comment
from datetime import datetime
import random
import urllib
import urlparse
import re
import time

# TODO
# - Unit testing, the lights are on but no one is home with that atm
# * Recursive bashing in objects discovered through file/dir bashing..
# * Parse HTML/CSS/JS for 'interesting' comments
# - Google site:www search
# - Form pulling/bruting (?)
# - Definitions. 200 should be a 404? Specify custom 404 code..
# - Threads? Initial research suggests not easy in Python
# - Save output to file
# - Cross reference version strings with CVE database and print
# - Parameters (GET first)
# - Search source for form action = "xxx"
# -- SQLite for object load and store
# -- Lots of places to go once data is in sql db. Web app, parsers etc..
# -- Thinking about some rulesets one could write which define rules to flag
#    interesting components in a page..
#    I'm thinking about this because i don't like having hard coded
#    directories in functions like crawl()

def usage():
   "Returns parser"
   usage = "usage: %prog [options] -o output_prefix http://target"
   parser = optparse.OptionParser(usage)
   parser.add_option("-r", "--recon", dest="recon", action="store_true",
      help="discover and investigate only, no attacks", metavar="FILE")
   parser.add_option("-q", "--quiet", action="store_false", dest="verbose",
      default=False, help="don't print status messages to stdout")
   parser.add_option("-o", "--prefix", dest="outputPrefix", type=str,
      help="Prefix of files to be created during scan. Required")
   parser.add_option("-p", "--port",
      dest="port", default=80, type=int, help="specify the target port")
   (options, args) = parser.parse_args()
   group = optparse.OptionGroup(parser, "Notes",
      "This application picks the low fruit in PHP web applications, "
      "and is designed to be quick(ish). For this reason, this script "
      "does not offer brute force capabilities.")
   parser.add_option_group(group)

   return options, args, parser

def checkUsage(options, args, parser):
   if options.outputPrefix == "":
      parser.print_help()
      return 1

   if len(args) == 0:
      parser.print_help()
      return 1

   target = args[0]

   # Check that target domain name has been specified and starts with http://
   if target[0:7] != 'http://':
      parser.print_help()
      return 1

   # Cool.
   return 0

class Site:
   def __init__(self, arg_base_url, arg_prefix):
      self.base_url = arg_base_url
      self.options_prefix = arg_prefix

   page = []

class Page:
   def __init__(self, arg_url):
      self.url = arg_url
      self.status_code = 0
      self.crawled = 0
      if arg_url[-1] == '/':
         self.pageType = 'directory'
      else:
         self.pageType = 'file'

      self.findings = {}
      # /home/ /var/ /etc/ ...
      self.findings['paths'] = []
      # Not 'proper' source code, PHP tags..
      self.findings['tag_code'] = []
      self.findings['source_code'] = []
      self.findings['comments'] = []

      self.findings['errors'] = {}
      self.findings['errors']['warns'] = []
      self.findings['errors']['fatals'] = []
      self.findings['errors']['errors'] = []

      self.findings['parameters'] = {}
      self.findings['parameters']['get'] = [] 
      self.findings['parameters']['post'] = []

def writeFile(fileName, content, interfaceType='w'):
   myFile = open(fileName, interfaceType)
   myFile.write(content)
   myFile.close()

def matchDictionary(search,dictionary):
   if search in dictionary:
      return True
   return False

def urlList(site):
   for i in site.pages:
      # Not sure how else to get the URLs in the form needed
      # for crawl()
      urlListReturn.append(site.page[i].url)
   return urlListReturn

def processLink(search,site,page,link):
   # def these..
   if (search in dict(link.attrs)):
      url = urlparse.urljoin(page.url,link[search])
      if url.find("'")!=-1: return
      url = url.split('#')[0] # remove location portion
      # Will keep crawling the same pages..
      urlAdded = 0
      for i in site.page:
         if i.url == url:
            urlAdded = 1
      if urlAdded == 0:
         writeFile(site.options_prefix + '_urls', url + '\n', 'a')
         site.page.append(Page(url))
         # File or directory?
         determinePageType(site.page[len(site.page)-1])

def crawl(site,page,depth=5):
   for i in range(depth):
      newpages = set()
      # No crawl creep or multiple crawls of the same page
      if re.findall(r"^" + site.base_url + "*?", page.url) and page.crawled == 0:
         try:
            c = urllib.urlopen(page.url)
            page.crawled = 1
         except Exception as x:
            print "Can't grab " + page.url
            continue

         print 'crawl() ' + page.url
         page.status_code = c.getcode()

         soup = BeautifulSoup(c.read())
         page.source_code = soup.get_text()
         source_html = soup.prettify()

         comments = soup.findAll(text=lambda text:isinstance(text, Comment))

         # Need an object which contains individual URLs and info..
         # Pull stuff from page
         paths = re.findall("(/boot/.*\..[a-zA-Z0-9~]*)", page.source_code)
         paths.append(re.findall("(/dev/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/etc/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/home/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/lost+found/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/mnt/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/opt/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/proc/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/root/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/run/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/srv/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/sys/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/tmp/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/usr/.*\..[a-zA-Z0-9~]*)", page.source_code))
         paths.append(re.findall("(/var/.*\..[a-zA-Z0-9~]*)", page.source_code))
         errors = re.findall("(.*error.*)", page.source_code, re.IGNORECASE)
         fatals = re.findall("(.*fatal.*)", page.source_code, re.IGNORECASE)
         warns = re.findall("(.*warn.*)", page.source_code, re.IGNORECASE)
         # ADD THIS BELOW SOMETIME..
         php_source = re.findall("(.*<\?.*)", source_html, re.IGNORECASE)

         for path in paths:
            if matchDictionary(path,page.findings['paths']) == False:
               page.findings['paths'].append(path)

         for error in errors:
            if matchDictionary(error,page.findings['errors']['errors']) == False:
               page.findings['errors']['errors'].append(error)

         for fatal in fatals:
            if matchDictionary(fatal,page.findings['errors']['fatals']) == False:
               page.findings['errors']['fatals'].append(fatal)

         for warn in warns:
            if matchDictionary(warn,page.findings['errors']['warns']) == False:
               page.findings['errors']['warns'].append(warn)

         for comment in comments:
            if matchDictionary(comment,page.findings['comments']) == False:
               page.findings['comments'].append(comment)

         for tag_code in php_source:
            if matchDictionary(comment,page.findings['tag_code']) == False:
               page.findings['tag_code'].append(tag_code)

         links = soup('a')
         for link in links:
            processLink('href',site,page,link)
            processLink('src',site,page,link)
   return

def determinePageType(page):
   # Search for 'Index of' and check URL for trailing /
   # Set site.page.pageType to 'file' or 'directory'
   # No 'proper' way to tell directory or file in HTTP..
   # (I don't think)
   if page.url[-1] == '/':
      page.pageType = 'directory'
   else:
      page.pageType = 'file'

def addSpeculativeURLs(speculativeURLs, bashFile, target):
   # Populate SpeculativeURLs
   count = len(speculativeURLs.page)

   with open(bashFile) as f:
      content = open(bashFile).read().splitlines()
   for i in content:
      specURLAdded = 0

      for k in speculativeURLs.page:
         #time.sleep(.1)
         if (target + i) == k.url:
            specURLAdded = 1

      if specURLAdded == 0:
         speculativeURLs.page.append(Page(target))
         speculativeURLs.page[count].url = target + i
         # File or directory?
         determinePageType(speculativeURLs.page[count])
         count += 1

   count -= 1

   #for i in speculativeURLs.page:
   #   print('spec ' + i.url)

def appendSpeculativeURLsToDirectory(speculativeURLs, bashFile, site):
   for i in site.page:
      if i.pageType == 'directory':
         target = i.url
         addSpeculativeURLs(speculativeURLs, bashFile, target)

def startCrawl(site, speculativeURLs, bashFile):
   # Initial crawl
   crawlStale = False
   while crawlStale == False:
      for i in site.page:
         crawl(site,i)
      # Count URLs and store
      pagesCount = len(site.page)
      addSpeculativeURLs(speculativeURLs, bashFile, site.base_url)
      appendSpeculativeURLsToDirectory(speculativeURLs, bashFile, site)
      # Check URL count and set crawlStale = True if the same
      if pagesCount == len(site.page):
         crawlStale = True
