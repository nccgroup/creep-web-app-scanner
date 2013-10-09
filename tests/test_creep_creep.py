#!/usr/bin/env python3.3

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

from app import creep_app
from lib import creep
import unittest

class TestCreepApp(unittest.TestCase):
   def setUp(self):
      self.site = creep.Site()
      self.site.target = 'http://test.com/'
      self.url = 'http://test.com/'
      self.site.page[self.url] = creep.Page
