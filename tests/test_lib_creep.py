#!/usr/bin/env python3.3

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

from lib import creep
import unittest

class TestCreep(unittest.TestCase):
   def setUp(self):
      self.site = creep.Site()
      self.site.target = 'http://test.com/'
      self.url = 'http://test.com/'
      self.site.page[self.url] = creep.Page

# urlNotCrawled(site, url):
# urlWithinTargetScope(url, site):
# crawl(site, url):

   ###
   # urlNotCrawled() tests
   def testUrlNotCrawledReturnsTrue(self):
      self.url = 'http://test1.com/'
      self.assertTrue(creep.urlNotCrawled(self.site, self.url))

   def testUrlNotCrawledReturnsFalse(self):
      self.url = 'http://test.com/'
      self.assertFalse(creep.urlNotCrawled(self.site, self.url))

   ###
   # urlWithinTargetScope() tests
   def testUrlWithinTargetScopeReturnsTrue(self):
      self.site.target = 'http://test.com/'
      self.url = 'http://test.com/'
      self.assertTrue(creep.urlWithinTargetScope(self.url, self.site))

   def testUrlWithinTargetScopeReturnsFalse(self):
      self.site.target = 'http://test.com/'
      self.url = 'http://test1.com/'
      self.assertFalse(creep.urlWithinTargetScope(self.url, self.site))

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
#      self.url = 'http://test.com/'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://test.com/a'
#      creep.crawl(self.site, self.url)
#      self.assertTrue(self.site.pagesCrawled == 2)
#
#   def testCrawlDoesIncrementCrawlCounter(self):
#      self.url = 'http://test.com/1'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://test.com/2'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://test.com/3'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://test.com/4'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://test.com/5'
#      creep.crawl(self.site, self.url)
#      self.url = 'http://test.com/6'
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
