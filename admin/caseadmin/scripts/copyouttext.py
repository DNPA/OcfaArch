#!/usr/bin/python
#
# !!!! Please review initial parameters !!!!
#
# Makes a copy of all text based documents to a directory
# Main purpose: Text extraction from ocfa for TextMining.
#
#
# uses PyGreSQL for postgresql connection

import sys
import os.path
import os
import shutil
import pg


#
# Parameters
#
casename = 'katla'
databasehost = 'wddb'
ext = '.txt'
min_size = 360
max_size = 10000000


todir = "/var/ocfa/cases/%(case)s/textexport" % { 'case':casename }
repo = "/var/ocfa/cases/%(case)s/repository" % { 'case':casename }


print "Parameters: "
print "\tCasename   = " + casename
print "\tDatabase   = " + databasehost
print "\tRepository = " + repo
print "\tOutputdir  = " + todir
print "\tMax Size   = %(max)s" % { 'max':max_size }
print "\tMin Size   = %(min)s" % { 'min':min_size }


if(os.path.exists(todir) == False):
   print "\nPlease create outputdir: " + todir + "\n\n"
   sys.exit(-1)

#
# SQL Query
#
sql1 = """
SELECT
        s.meta as size, mt.meta as mime,  repname,  location
from
        metadatainfo mi, rowsize s, rowmimetype mt, evidencestoreentity e
where
        e.id=mi.dataid and
        mt.metadataid=mi.metadataid and
        s.metadataid=mi.metadataid and
        mt.meta like 'text%' and"""
sql2 = """
        s.meta > %(min)s and s.meta < %(max)s
""" % { 'max':max_size, 'min':min_size }

#
# Database connection
#
pgcnx = pg.connect(host = databasehost, user = "ocfa", passwd = "ocfa", dbname = casename)
qresult = pgcnx.query(sql1+sql2)

for tuple in qresult.dictresult():
   file = tuple['repname']
   dir = todir + file[:6]
   if(os.path.exists(dir) == False):
      print "Make dir: " + dir
#      os.makedirs(dir)
   src = repo + file
   dst = todir + file + ext
#   shutil.copyfile(src,dst)
   print 'cp ' + src + " " + dst


pgcnx.close()



#for regel in open(sys.argv[1]):
#   tokens = regel.split("|")
#   ext = '.txt'
#   file = tokens[2].lstrip(' ').rstrip(' ')
#   dir1 = todir + file[:3]
#   dir2 = todir + file[:6]
#   if( os.path.exists(dir2) == False):
#      print "Make dir: " + dir2
#      os.makedirs(dir2)
#   src = repo + file
#   dst = todir + file + ext
#   shutil.copyfile(src,dst)
#   print 'cp ' + src + " " + dst
