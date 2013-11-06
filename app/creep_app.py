#!/usr/bin/env python3.3

# Released as open source by NCC Group Plc - http://www.nccgroup.com/
# Developed by Aidan Marlin, aidan dot marlin at nccgroup dot com
# https://github.com/nccgroup/creep-web-app-scanner
# Released under AGPL. See LICENSE for more information

import re
import tornado

# class Page():
# class Site():

# def urlNotCrawled(site, url):
# def urlWithinTargetScope(url, site):
# def crawl(site, url):

def usage():
   "Returns parser"
   usage = "usage: %prog [options] -o output_prefix http://target"
   parser = optparse.OptionParser(usage)
   parser.add_option("-r", "--recon", dest="recon", action="store_true",
      help="discover and investigate only, no attacks", metavar="FILE")
   parser.add_option("-q", "--quiet", action="store_false", dest="verbose",
      default=False, help="don't print status messages to stdout")
   parser.add_option("-o", "--prefix", dest="outputPrefix", type=str,
      help="prefix of files to be created during scan. Required")
   parser.add_option("-p", "--port", dest="port", default=80, type=int,
      help="specify the target port")
   (options, args) = parser.parse_args()
   group = optparse.OptionGroup(parser, "Notes",
      "This application picks the low fruit in PHP web applications, "
      "and is designed to be quick(ish). For this reason, this script "
      "does not offer brute force capabilities.")
   parser.add_option_group(group)

   return options, args, parser

def main():
   options, args, parser = creep_app.usage()
   pass
