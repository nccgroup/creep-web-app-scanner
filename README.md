creep-web-app-scanner
=====================

A (very) primitive website scanner currently under development by an NCC Group employee and University graduate with 20% research time. creep currently crawls a site, and searches for potentially interesting information within each page. creep uses the following output procedure:

for i in site.page:
   if i.crawled == 1 and i.status_code != 404:
      print i.url
      print i.crawled
      print i.status_code
      print 'paths ' + str(i.findings['paths'])
      print 'source_code ' + str(i.findings['source_code'])
      print 'comments ' + str(i.findings['comments'])
      print 'warns ' + str(i.findings['errors']['warns'])
      print 'fatals ' + str(i.findings['errors']['fatals'])
      print 'errors ' + str(i.findings['errors']['errors'])
      print 'get ' + str(i.findings['parameters']['get'])
      print 'post ' + str(i.findings['parameters']['post'])

creep will crawl your (HTTP only) target and pull interesting info on the site, including:

* Source code comments
* Errors (MySQL errors, warnings, fatals, etc)
* Linux file paths

creep will also perform file/dir bashing on existing known locations to try and locate potentially interesting files/dirs (test.php, admin/, etc)

creep will (in the near future) perform parameter bashing and compile a list of potentially interesting results by calculating the hashes of resource output, requesting the page using specific parameters, and then comparing the hash of the new output with that of the original to determine if a different output was generated.

Other things and features to be added to creep include:

* Unit testing
* Search engine, site specific search (Google's site: inurl: etc..)
* Form processing. Pulling action resources and (possibly) brute forcing
* Equivalent status code definitions. Some websites return 200 regardless of whether the requested resource is actually available/exists
* Concurrent network IO
* Loading/saving data to SQL db (SQLite)
* Cross reference version strings with CVE database
* Rulesets. Ability to customise what creep should be looking for

Comments and questions should be sent to aidan dot marlin at nccgroup dot com
