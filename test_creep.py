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
from lib import creep
import unittest

# usage():
# checkUsage():
# writeFile(fileName, content, interfaceType='w'):
# matchDictionary(search,dictionary):
# urlList(site):
# addSpeculativeURLs(speculativeURLs, pages):
# crawl(crawlTarget,page,depth=5):
# initialCrawl():
# speculativeURLs():

class TestCreepFunctions(unittest.TestCase):
   def setUp(self):
      # test_writeFile
      creep.filename = ['/tmp/py_test', '/tmp/py_test1']
      creep.content = 'aaa'
      creep.f = [0,0]
      creep.output = [0,0]

      # test_matchDictionary
      creep.searchString = 'test'
      creep.dictionary = ['test','what']

   def test_writeFile(self):
      # Write using function
      creep.writeFile(creep.filename[0],creep.content)

      # Write using Python functions
      creep.f[0] = open(creep.filename[1],'w')
      creep.f[0].write(creep.content)
      creep.f[0].close()

      # Open both files
      creep.f[0] = open(creep.filename[0],'r')
      creep.output[0] = creep.f[0].read()
      creep.f[0].close()

      creep.f[1] = open(creep.filename[0],'r')
      creep.output[1] = creep.f[1].read()
      creep.f[1].close()

      self.assertEqual(creep.output[0], creep.output[1])

   def test_matchDictionary(self):
      self.assertTrue(creep.matchDictionary('test', creep.dictionary))

   def test_speculativeURLs(self):
      self.speculativeURLs = 1

   def test_addSpeculativeURLs(self):
      # Lots of vars
      site = creep.Site()
      site.site_url = 'http://test.com'
      site.page.append(creep.Page(site.site_url))
      site.page.append(creep.Page(site.site_url))
      site.page[0].url = site.site_url + '/'
      site.page[1].url = site.site_url + '/robots.txt'

      speculativeURLs = creep.Site()

      creep.addSpeculativeURLs(creep.speculativeURLs, creep.page)
      self.test = 1

      #for i in pages:
         #for k in speculativeURLs:
      self.assertTrue(1, 1)

   #def test_urlList(self):
   #   with self.assertRaises(ValueError):
   #      random.sample(self.seq, 20)
   #   for element in random.sample(self.seq, 5):
   #      self.assertTrue(element in self.seq)

if __name__ == '__main__':
   unittest.main()
