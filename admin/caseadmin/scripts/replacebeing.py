#!/usr/bin/python

#uses PyGreSQL for postgresql connection


import sys
import os.path
import os
import shutil
import pg
import xml.etree.ElementTree as et

#
# function
#	parse the case configuration file
#
def parse_config(filename):
   COMMENT_CHAR = '#'
   OPTION_CHAR = '='
   options = {}
   f = open(filename)
   for line in f:
      if COMMENT_CHAR in line:
         line,comment = line.split(COMMENT_CHAR,1)
      if OPTION_CHAR in line:
         option,value = line.split(OPTION_CHAR,1)
         option = option.strip()
         value = value.strip()
         options[option] = value
   f.close()
   return options

#
# check arguments
#


#
# Read the configuration file
# using the OCFACASE environment variable
#



#
# Database connection and query
#
pgcnx = pg.connect(host = 'wadb', user = 'ocfa', passwd = 'ocfa', dbname = 'holwasparked')

sql1 = """
SELECT
    ms.content,mi.location,mi.metadataid
from
    metadatainfo mi, metastoreentity ms
where
    ms.id=mi.metadataid and
    mi.location='[being processed]'"""
    

qresult = pgcnx.query(sql1)

teller = 0
ok = 0
#
# Main loop reading tuples
#
for tuple in qresult.dictresult():
   xml = tuple['content']
   metadataid = tuple['metadataid']
   e = et.XML(xml)
   elocations = e.findall('location')
   elocbody = elocations[0].text
   elocname = elocations[0].attrib['name']
   if elocbody :
      try:
         newlocation = (unicode(elocbody).encode('utf-8') + "/" + unicode(elocname).encode('utf-8')).replace("'","''")
      except:
         newlocation = '[being processed]'
   else:
      newlocation = "/" + str(elocname)
   location = tuple['location']
   teller = teller + 1
   if location == newlocation:
      ok = ok + 1
   else:
      print "UPDATE metadatainfo SET location = '" + newlocation + "' WHERE metadataid=" + str(metadataid) + ";"



pgcnx.close()


print "\nOK = " + str(ok)
print "Processed a total of " + str(teller) + " entries\n"
