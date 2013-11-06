#!/usr/bin/env python3.3

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

from lib import creep
import unittest
import tornado
import tornado.testing
import tornado.gen
import tornado.ioloop
import tornado.httpclient

class TestCreep(unittest.TestCase):
   def setUp(self):
      self.site = creep.Site()
      self.site.target = 'http://pastefu.org/'
      self.url = 'http://pastefu.org/'
      self.site.page[0].url = 'http://pastefu.org/'

# urlCrawled(site, url):
# urlWithinTargetScope(url, site):
# crawl(site, url):

   ###
   # urlCrawled() tests
   def testUrlCrawledReturnsTrue(self):
      self.site.page[len(self.site.page)-1].url = 'http://pastefu.org/cock'
      self.site.page[len(self.site.page)-1].crawled = True
      self.url = 'http://pastefu.org/cock'
      self.assertTrue(creep.urlCrawled(self.site, self.url))

   def testUrlCrawledReturnsFalse(self):
      self.site.page[len(self.site.page)-1].url = 'http://pastefu.org/cock'
      self.site.page[len(self.site.page)-1].crawled = False
      self.url = 'http://pastefu.org/cock'
      self.assertFalse(creep.urlCrawled(self.site, self.url))

   ###
   # urlWithinTargetScope() tests
   def testUrlWithinTargetScopeReturnsTrue(self):
      self.site.target = 'http://pastefu.org/'
      self.url = 'http://pastefu.org/'
      self.assertTrue(creep.urlWithinTargetScope(self.url, self.site))

   def testUrlWithinTargetScopeReturnsFalse(self):
      self.site.target = 'http://pastefu.org/'
      self.url = 'http://test1.com/'
      self.assertFalse(creep.urlWithinTargetScope(self.url, self.site))

   def testAppendURLToSite(self):
      inc = 0

      self.testURLS = [ 'http://pastefu.org/a', 'http://pastefu.org/b', 'http://pastefu.org/c', 'http://pastefu.org/d' ]
      for self.url in self.testURLS:
         creep.Site.appendURLToSite(self.site, self.url)

      for item in self.site.page:
         inc += 1

      print("inc == " + str(inc) + "\nlen(self.self.site.page) == " + str(len(self.site.page)))
      print("testAppendURLToSite == " + self.site.page[1].url)
      self.assertTrue(len(self.site.page) == inc)

###
# Test crawl
class TestCrawl(unittest.TestCase):
   def setUp(self):
      self.site = creep.Site
      self.site.target = 'http://pastefu.org/'
      self.url = 'http://pastefu.org/'

      self.site.page.append(creep.Page)
      self.site.page[0].url = 'http://pastefu.org/'

   def testCrawlGet(self):
      inc = 0

      self.testURLS = [ 'http://pastefu.org/a', 'http://pastefu.org/b', 'http://pastefu.org/c', 'http://pastefu.org/d' ]
      for self.url in self.testURLS:
         creep.Site.appendURLToSite(self.site, self.url)

      creep.Crawl.get(self.site)
      for fucksake in self.site.page:
         print("FUCK SAKE == " + str(fucksake.response))

      for item in self.site.page:
         if item.crawled == True:
            inc += 1

      print("inc == " + str(inc) + "\nlen(self.self.site.page) == " + str(len(self.site.page)))
      #self.assertTrue(len(self.site.page) == inc)

#   @tornado.testing.gen_test
#   def test_http_fetch(self):
#      client = tornado.testing.AsyncHTTPClient(self.io_loop)
#      client.fetch("http://www.pastefu.org/", self.handle_fetch)
#      self.wait()

#   def handle_fetch(self, response):
#      # Test contents of response (failures and exceptions here
#      # will cause self.wait() to throw an exception and end the
#      # test).
#      # Exceptions thrown here are magically propagated to
#      # self.wait() in test_http_fetch() via stack_context.
#      print(response.body)
#      self.assertIn("It works!", str(response.body))
#      self.stop()

#   # creep.crawl() tests
#   def testCrawlReturnsTrue(self):
#      self.assertTrue(creep.crawl(self.site, self.url))
#
#   def testCrawlDoesNotCrawlSameUrlTwice(self):
#      creep.crawl(self.site, self.url)
#      creep.crawl(self.site, self.url)
#      self.assertTrue(self.site.pagesCrawled == 1)
#
#   def testCrawlDoesCrawlDifferentUrls(self):
#      self.url = 'http://pastefu.org/'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://pastefu.org/a'
#      creep.crawl(self.site, self.url)
#      self.assertTrue(self.site.pagesCrawled == 2)
#
#   def testCrawlDoesIncrementCrawlCounter(self):
#      self.url = 'http://pastefu.org/1'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://pastefu.org/2'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://pastefu.org/3'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://pastefu.org/4'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://pastefu.org/5'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://pastefu.org/6'
#      creep.crawl(self.site, self.url)
#      self.assertTrue(self.site.pagesCrawled == 6)
#
#   def testCrawlRejectsUrlNotContainingTarget(self):
#      self.url = 'http://test1.com/'
#      self.assertFalse(creep.crawl(self.site, self.url))
#      self.assertTrue(self.site.pagesCrawled == 0)
#
#   def testCrawlPopulatesSourceAttribute(self):
#      self.url = 'mockobject'
#      self.source = 'mocksource'
#      creep.crawl(self.site, self.url)
#      self.assertTrue(self.site.page[self.url].source == self.source)

def main():
   unittest.main()

if __name__ == '__main__':
   main()
