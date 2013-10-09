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
      self.url = 'http://test.com/aa'
      self.site.target = 'http://test.com/'

   # creep.urlIncludesTarget() tests
   def testUrlIncludesTarget(self):
      self.assertTrue(creep.urlIncludesTarget(self.url, self.site.target))

   def testUrlDoesNotIncludeTarget(self):
      self.url = 'Ahttp://test.com/'
      self.target = 'http://test.com/'
      self.assertFalse(creep.urlIncludesTarget(self.url, self.site.target))

   # creep.crawl() tests
   def testCrawlDoesSetCrawled(self):
      creep.crawl(self.site, self.url)
      self.assertTrue(self.site.page[self.url].crawled == 1)

   def testCrawlDoesNotCrawlSameUrlTwice(self):
      creep.crawl(self.site, self.url)
      creep.crawl(self.site, self.url)
      self.assertTrue(self.site.pagesCrawled == 1)

   def testCrawlDoesCrawlDifferentUrls(self):
      self.url = 'http://test.com/'
      creep.crawl(self.site, self.url)
      self.url = 'http://test.com/a'
      creep.crawl(self.site, self.url)
      self.assertTrue(self.site.pagesCrawled == 2)

   def testCrawlDoesIncrementCrawlCounter(self):
      self.url = 'http://test.com/1'
      creep.crawl(self.site, self.url)
      self.url = 'http://test.com/2'
      creep.crawl(self.site, self.url)
      self.url = 'http://test.com/3'
      creep.crawl(self.site, self.url)
      self.url = 'http://test.com/4'
      creep.crawl(self.site, self.url)
      self.url = 'http://test.com/5'
      creep.crawl(self.site, self.url)
      self.url = 'http://test.com/6'
      creep.crawl(self.site, self.url)
      self.assertTrue(self.site.pagesCrawled == 6)

   def testCrawlRejectsUrlNotContainingTarget(self):
      self.url = 'http://test1.com/'
      self.assertFalse(creep.crawl(self.site, self.url))
      self.assertTrue(self.site.pagesCrawled == 0)

def main():
   unittest.main()

if __name__ == '__main__':
   main()
