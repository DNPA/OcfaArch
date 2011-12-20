#!/usr/bin/python

#uses PyGreSQL for postgresql connection


import sys
import os.path
import os
import shutil
import pg
import ConfigParser

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
if len(sys.argv) != 2:
   print "\nText export python script\n"
   print "Uses a given destination directory to copy"
   print "text files from the repository. For each source, item"
   print "pair a sub directory is created to store the text files\n"
   print "\tUsage: " + sys.argv[0] + " <destinationi dir>\n\n"
   sys.exit(2)

todir = sys.argv[1]


#
# Read the configuration file
# using the OCFACASE environment variable
#
try:
   casename = os.environ['OCFACASE']
except KeyError:
   print "ERROR: No environmentkey OCFACASE found"
   sys.exit(2)

conf = '/var/ocfa/cases/' + casename + '/etc/ocfa.conf'
if(os.path.exists(conf) == False):
   print "ERROR: No conffile on " + conf
   sys.exit(1)

options = parse_config(conf)
dbhost = options['storedbhost']
dbuser = options['storedbuser']
dbpass = options['storedbpasswd']
repo = options['repository']


print "Config file  = " + conf
print "Databasehost = " + dbhost 
print "Databaseuser = " + dbuser
print "Databasepass = " + dbpass
print "Casename     = " + casename
print "Destination  = " + todir
print "Evidece repo = " + repo

#
# Check repository and destination path
#
if(os.path.exists(repo) == False):
   print "ERROR: Repository " + repo + " path doesn't exists!\n"
   sys.exit(1)

if(os.path.exists(todir) == False):
   print "Make destination: " + todir
   os.makedirs(todir)


ext = '.txt'
mime_type = 'text'
min_size = 360
max_size = 10000000


#
# Database connection and query
#
pgcnx = pg.connect(host = dbhost, user = dbuser, passwd = dbpass, dbname = casename)

sql1 = """
SELECT
        i.evidencesource as source, i.item, s.meta as size, mt.meta as mime,  repname,  location
from
        metadatainfo mi, rowsize s, rowmimetype mt, evidencestoreentity e, item i
where
	i.itemid=mi.itemid and
        e.id=mi.dataid and
        mt.metadataid=mi.metadataid and
        s.metadataid=mi.metadataid and
        mt.meta like 'text%' and"""
sql2 = """
        s.meta > %(min)s and s.meta < %(max)s
""" % { 'max':max_size, 'min':min_size }


qresult = pgcnx.query(sql1+sql2)

teller = 0

#
# Main loop reading tuples
#
for tuple in qresult.dictresult():
   file = tuple['repname']
   itemsource = tuple['source'] + '_' + tuple['item']
   dir = todir + '/' + itemsource + file[:6]
   if(os.path.exists(dir) == False):
      print "Make dir: " + dir
      os.makedirs(dir)
   src = repo + file
   dst = todir + '/' + itemsource + file + ext
   shutil.copyfile(src,dst)
   print 'cp ' + src + " " + dst
   teller = teller + 1


pgcnx.close()

print "Copied a total of " + teller + " files\n"
