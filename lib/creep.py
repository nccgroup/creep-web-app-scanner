#!/usr/bin/env python3.3

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

import re
import tornado

class Page():
   crawled = 0

class Site():
   page = {}
   pagesCrawled = 0
   target = ''

def urlIncludesTarget(url, target):
   m = re.match(target, url)
   return m != None

def crawl(site, url):
   # Ensure URL starts with target to prevent crawl creep
   if urlIncludesTarget(url, site.target) != True:
      return False

   # Ensure URL has not already been crawled
   if url not in dict(site.page):
      site.pagesCrawled += 1
      site.page[url] = Page()
      site.page[url].crawled = 1



   return True
