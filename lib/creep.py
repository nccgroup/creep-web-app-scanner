#!/usr/bin/env python3.3

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

import re
import tornado
import tornado.gen
import tornado.ioloop
import tornado.httpclient
from functools import partial

class Page():
   url = ''
   crawled = False
   response = ''

class Site():
   page = []
   # Potentially useful for checking if anymore pages need to be crawled
   pagesCrawled = 0
   target = ''

   def appendURLToSite(site, url):
      # To prevent race conditions we need to declare a new instance of Page,
      # assign variables, and then append..
      newPage = Page()
      newPage.url = url

      site.page.append(newPage)
      print("appendURLToSite == " + site.page[len(site.page)-1].url)

def urlCrawled(site, url):
   # Ensure URL has not already been crawled
   for item in site.page:
      if url == item.url:
         return item.crawled

def urlWithinTargetScope(url, site):
   # Ensure URL starts with target to prevent crawl creep
   m = re.match(site.target, url)
   return m != None

class Crawl(Site):
   #site.pagesCrawled += 1
   #site.page[url] = Page()
   #site.page[url].crawled = 1

   def waitForCrawl(site):
      for item in site.page:
         if site.page.crawled == False:
            return True
      return False

   @classmethod
   def handle_request(cls, response, page):
      page.response = response
      page.crawled = True
      print(response)
      tornado.ioloop.IOLoop.instance().stop()

#      if (waitForCrawl(site)) == False:
#         tornado.ioloop.IOLoop.instance().stop()

   def get(site):
      http_client = tornado.httpclient.AsyncHTTPClient()
      item = []

      for item in site.page:
         print("item = " + item.url)
         if urlCrawled(site, item.url) == False:
            http_client.fetch(item.url, partial(Crawl.handle_request, page=item))
      tornado.ioloop.IOLoop.instance().start()
