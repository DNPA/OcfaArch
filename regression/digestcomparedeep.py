#!/usr/bin/python
import xml.etree.ElementTree
import sys

#Simple holder class for meta data tag information.
class SimpleMetaVal:
  def __init__(self,section,metanode):
    self.section=section
    self.name=metanode.attrib['metaname']
    self.metatype=metanode.attrib['containertype']
    self.scalartype=metanode.attrib['scalartype']
    self.module=metanode.attrib['module']
    self.val=metanode.text
  def __hash__(self):
    return hash(self.name+"."+self.metatype+"."+self.scalartype+"."+self.val)
  def __eq__(self,other):
    if self.name==other.name and self.metatype==other.metatype and self.scalartype==other.scalartype and self.val==other.val:
      return True
    return False
  def __lt__(self,other):
    if self.name<other.name:
      return True
    if self.name>other.name:
      return False
    if self.metatype<other.metatype:
      return True
    if self.metatype>other.metatype:
      return False
    if self.scalartype<other.scalartype:
      return True
    if self.scalartype>other.scalartype:
      return False
    if self.val < other.val:
      return True
    return False

class ExtendedFileNode:
  def __init__(self,section,filenode):
    self.section=section
    self.filenode=filenode
    self.digest=filenode.attrib['hash']
    self.relpath=filenode.attrib['relpath']
  def __hash__(self):
    return hash(self.digest + "." + self.relpath)
  def __eq__(self,other):
    if self.digest==other.digest and self.relpath==other.relpath:
      return True
    return False
  def __lt__(self,other):
   if (self.relpath < other.relpath):
     return True
   if (self.relpath > other.relpath):
     return False
   if (self.digest < other.digest):
     return True
   else:
     return False
  def __getitem__(self,key):
    metanodes=self.filenode.findall('meta')
    for metanode in metanodes:
      if (metanode.attrib['metaname'] == key):
        return SimpleMetaVal(self.section,metanode)
  def get_keys(self):
    names=set()
    metanodes=self.filenode.findall('meta')
    for metanode in metanodes:
      names.add(metanode.attrib['metaname'])
    return names
  def __str__(self):
    return self.section+":"+self.digest+":"+self.relpath

class ExtendedFileSet:
  def __init__(self,filename):
    self.files=set()
    self.filemeta=dict()
    tree=xml.etree.ElementTree.parse(filename)
    for section in ('inputfiles','derivedfiles'):
      sectionnode=tree.find(section)
      allfilenodes=sectionnode.findall('file')
      for setfile in allfilenodes:
        newfile= ExtendedFileNode(section,setfile)
        self.files.add(newfile)
        self.filemeta[newfile]=newfile
  def get_set(self):
    return self.files
  def __call__(self,thefile):
    return self.filemeta[thefile]

if len(sys.argv) < 3:
  print "No comparison files given"
  print "Usage:\n\n\tdigestcomparedeep.pl <referencexml> <testrunxml>\n\n"
  sys.exit(1)
referencefile=sys.argv[1]
testfile=sys.argv[2]
referenceset=ExtendedFileSet(referencefile)
testset=ExtendedFileSet(testfile)
#Deep comparison only works on the intersection of all files in both runs.
sharedset=referenceset.get_set().intersection(testset.get_set())
print "=========== CHANGED META DATA ================"
#Iirst, go deep to see if any meta data has changed meta type, scalar type or value.
count=0
for sharedfile in sharedset:
  referencemetaset=referenceset(sharedfile)
  testmetaset=testset(sharedfile)
  referencemetakeys=referencemetaset.get_keys()
  testmetakeys=testmetaset.get_keys()
  sharedmetakeys=referencemetakeys.intersection(testmetakeys)
  doinc=False
  for sharedkey in sharedmetakeys:
    if referencemetaset[sharedkey] != testmetaset[sharedkey]:
      referencemeta=referencemetaset[sharedkey]
      testmeta=testmetaset[sharedkey]
      if referencemeta.metatype != testmeta.metatype:
        print " Meta field changed meta type. File="+str(sharedfile)+" meta="+sharedkey+" referencemetatype="+referencemeta.metatype+" testmetatype="+testmeta.metatype
        doinc=True
      if referencemeta.scalartype != testmeta.scalartype:
        print " Meta field changed scalar type. File="+str(sharedfile)+" meta="+sharedkey+" referencescalartype="+referencemeta.scalartype+" testscalartype="+testmeta.scalartype
        doinc=True
      if referencemeta.val != testmeta.val:
        print " Meta field changed value. File="+str(sharedfile)+" meta="+sharedkey+" referencevalue="+referencemeta.val+" testval="+testmeta.val
        doinc=True
  if doinc:
    count=count+1
print "Found "+str(count)+" out of "+str(len(sharedset)) +" files with changed meta data."
print "=========== MISSING META DATA ================"
#Now check deeply if there is any meta data missing from the testrun that was in the referencerun.
count=0
for sharedfile in sharedset:
  referencemetaset=referenceset(sharedfile)
  testmetaset=testset(sharedfile)
  referencemetakeys=referencemetaset.get_keys()
  testmetakeys=testmetaset.get_keys()
  missingkeys=referencemetakeys - testmetakeys
  doinc=False
  for missingkey in missingkeys:
    print " Meta field found in reference set but missing from test set. File="+str(sharedfile)+" meta="+missingkey
    doinc=True
  if doinc:
    count=count+1
print "Found "+str(count)+" out of "+str(len(sharedset)) +" files with missing meta data"
print "=========== NEW META DATA ================"
#Finaly, check deeply to see if there is any meta data new to the test run that wasn't in the reference run.
count=0
for sharedfile in sharedset:
  referencemetaset=referenceset(sharedfile)
  testmetaset=testset(sharedfile)
  referencemetakeys=referencemetaset.get_keys()
  testmetakeys=testmetaset.get_keys()
  newkeys=testmetakeys -referencemetakeys
  doinc=False
  for newkey in newkeys:
    print " Meta field new to test set. File="+str(sharedfile)+" meta="+newkey
    doinc=True
  if doinc:
    count=count+1
print "Found "+str(count)+" out of "+str(len(sharedset)) +" files with meta data that was not found in referenceset"

