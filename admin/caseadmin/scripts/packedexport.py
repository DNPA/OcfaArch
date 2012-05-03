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
   print "\nPacked-binary export python script\n"
   print "Uses a given destination directory to copy"
   print "packed binary files from the repository. For each source, item"
   print "pair a sub directory is created to store the binary files\n"
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


ext = '.exe'

#
# Database connection and query
#
pgcnx = pg.connect(host = dbhost, user = dbuser, passwd = dbpass, dbname = casename)

sql = """select distinct repname from evidencestoreentity where id in (
            select dataid from metadatainfo where metadataid in (
               select metadataid from rowfileextension where meta='exe' and metadataid in (
                   select metadataid from rowpacked
               )
            )
         )"""

qresult = pgcnx.query(sql)

teller = 0

#
# Main loop reading tuples
#
for tuple in qresult.dictresult():
   file = tuple['repname']
   dir = todir + '/' + file[:6]
   if(os.path.exists(dir) == False):
      print "Make dir: " + dir
      os.makedirs(dir)
   src = repo + file
   dst = todir + '/' + file + ext
   shutil.copyfile(src,dst)
   print 'cp ' + src + " " + dst
   teller = teller + 1


pgcnx.close()

print "Copied a total of " + teller + " files\n"
