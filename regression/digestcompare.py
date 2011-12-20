#!/usr/bin/python
import xml.etree.ElementTree
import sys

class SimpleInput:
  def __init__(self,digest,relpath):
    self.digest=digest
    self.relpath=relpath
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

class SimpleOutput:
  def __init__(self,digest,relpath,parent):
    self.digest=digest
    self.relpath=relpath
    self.parent=parent
  def __hash__(self):
    return hash(self.digest + "." + self.relpath + "." + self.parent)
  def __eq__(self,other):
    if self.digest==other.digest and self.relpath==other.relpath and self.parent==other.parent:
      return True
    return False
  def __lt__(self,other):
    if (self.relpath < other.relpath):
      return True
    if (self.relpath > other.relpath):
      return False
    if (self.parent < other.parent):
      return True
    else:
      return False
    if (self.digest < other.digest):
      return True
    else:
      return False
  

class SimpleMetaType:
  def __init__(self,containertype,scalartype,metaname):
    self.containertype=containertype
    self.scalartype=scalartype
    self.metaname=metaname
  def __hash__(self):
    return hash(self.containertype + "." + self.scalartype + "." + self.metaname)
  def __eq__(self,other):
    if self.containertype==other.containertype and self.scalartype==other.scalartype and self.metaname==other.metaname:
      return True
    return False
  def __lt__(self,other):
    if (self.metaname < other.metaname):
      return True
    if (self.metaname > other.metaname):
      return False
    if (self.scalartype < other.scalartype):
      return True
    if (self.scalartype > other.scalartype):
      return False
    if (self.containertype < other.containertype):
      return True
    else:
      return False

class InputSet:
  def __init__(self,filename):
    tree=xml.etree.ElementTree.parse(filename)
    inputsection=tree.find('inputfiles')
    inputfilenodes=inputsection.findall('file')
    self.inputset=set()   
    self.routemap=dict()
    for inputfile in inputfilenodes:
      digest=inputfile.attrib['hash'] 
      relpath=inputfile.attrib['relpath']
      route=inputfile.attrib['route']
      input=SimpleInput(digest,relpath)
      self.routemap[input]=route
      self.inputset.add(input)      
  def __call__(self,reference):
    print "   Validating input"
    if self.inputset == reference.inputset:
      return True
    if  self.inputset.isdisjoint(reference.inputset):
      print "   Sets are disjoint, these input files could not refer to the same test item"
      return False
    if  self.inputset.issubset(reference.inputset):
      print "   Testset is missing input files from reference set"
      missing=reference.inputset - self.inputset
      for missingfile in sorted(missing):
         print "    Missing file: digest="+ missingfile.digest + " relpath="+missingfile.relpath 
      return False
    if self.inputset.issuperset(reference.inputset):
      print "   Testset has additional input files that are not in reference set"
      missing=self.inputset - reference.inputset
      for missingfile in sorted(missing):
         print "    Undefined reference set file: digest="+ missingfile.digest + " relpath="+missingfile.relpath
      return False
    sdif=self.inputset.symetric_difference(reference.inputset)
    if len(sdif) > 0:
      print "   Testset and reference set share some input files, but both have files that are not in the other"
      return False
    print "   The input sets are equal"
    return True
  def compare_routes(self,reference):
    intersection=self.inputset.intersection(reference.inputset)
    count=0
    for inputfile in intersection:
      if self.routemap[inputfile] != reference.routemap[inputfile]:
        count=count+1
        print "    Different routing for inputfile: digest="+inputfile.digest+" relpath="+inputfile.relpath+" referencesetroute=" + reference.routemap[inputfile] + " testsetroute ="+self.routemap[inputfile]
    print "   "+str(count)+" out of "+ str(len(intersection)) + " files follow a route that differs from that of the reference set." 
  def get_set(self):
    return self.inputset
 
class OutputSet:
  def __init__(self,filename):
    tree=xml.etree.ElementTree.parse(filename)
    outputsection=tree.find('derivedfiles')
    outputfilenodes=outputsection.findall('file')
    self.outputset=set()
    self.routemap=dict()
    for thefile in outputfilenodes:
      digest=thefile.attrib['hash'] 
      relpath=thefile.attrib['relpath']
      parent=thefile.attrib['parent']
      route=thefile.attrib['route']
      output=SimpleOutput(digest,relpath,parent)
      self.routemap[output]=route
      self.outputset.add(output)
  def compare_routes(self,reference):
    intersection=self.outputset.intersection(reference.outputset)
    count=0
    for outputfile in intersection:
      if self.routemap[outputfile] != reference.routemap[outputfile]:
        count=count+1
        print "    Different routing for outputfile: digest="+outputfile.digest+" relpath="+outputfile.relpath+" referencesetroute=" + reference.routemap[outputfile] + " testset route="+self.routemap[outputfile]
    print "   "+str(count)+" out of "+ str(len(intersection)) + " files follow a route that differs from that of the reference set."
  def __call__(self,reference):
    print "   Validating output"
    if self.outputset == reference.outputset:
      print "   Output files are the same"
      return True
    if len(self.outputset) == 0:
      print "   Testset has no output files while reference set has."
      return False
    if len(reference.outputset) == 0:
      print "   Reference set has no output files while testset has." 
      return False
    if  self.outputset.isdisjoint(reference.outputset):
      print "   Reference set and Test set produce ONLY completely different output files."
      return False
    if  self.outputset.issubset(reference.outputset):
      print "   Testset is missing output files that are in reference set"
      missing=reference.outputset - self.outputset
      for missingfile in sorted(missing):
         print "    Missing output file: parent="+missingfile.parent+" digest="+ missingfile.digest + " relpath="+missingfile.relpath
      return False
    if self.outputset.issuperset(reference.outputset):
      print "   Testset produced additional output files that are not in reference set"
      missing=self.outputset - reference.outputset
      for missingfile in sorted(missing):
         print "    Undefined output reference set file: digest="+ missingfile.digest + " relpath="+missingfile.relpath+" parent="+missingfile.parent
      return False
    sdif=self.outputset.symetric_difference(reference.outputset)
    if len(sdif) > 0:
      print "   Testset and reference set share some output files, but both have files that are not in the other"
      missing=reference.outputset - self.outputset
      for missingfile in sorted(missing):
         print "    Missing output file: digest="+ missingfile.digest + " relpath="+missingfile.relpath+" parent="+missingfile.parent
      missing=self.outputset - reference.outputset
      for missingfile in sorted(missing):
         print "    Undefined output reference set file: digest="+ missingfile.digest + " relpath="+missingfile.relpath+" parent="+missingfile.parent
      return False
    print "   The output sets are equal when looking at them in a shalow way."
    return True
  def get_set(self,other):
    returnset=self.outputset.intersection(other.outputset)
    for output in returnset:
      simple=SimpleInput(output.digest,output.relpath)
      yield simple

    

if len(sys.argv) < 3:
  print "No comparison files given"
  print "Usage:\n\n\tdigestcompare.pl <referencexml> <testrunxml>\n\n"
  sys.exit(1)
referencefile=sys.argv[1]
testfile=sys.argv[2]
referencesetinput=InputSet(referencefile)
testsetinput=InputSet(testfile)
print "============= STAGE 1: COMPATIBILITY CHECK ==================="
if testsetinput(referencesetinput):
  print "  Sets are compatible, proceeding with stage 2"
  print "============= STAGE 2: SHALLOW COMPARE ==================="
  referenceoutout=OutputSet(referencefile)
  testsetoutput=OutputSet(testfile) 
  print " ------------ 2A: INPUT FILE ROUTING -----------------"
  testsetinput.compare_routes(referencesetinput)
  print " ---------------- 2B: FILES OUT ----------------------"
  if testsetoutput(referenceoutout):
    print "  The files generated are the same"
  print " ---------- 2C: DERIVED FILE ROUTING -----------------"
  testsetoutput.compare_routes(referenceoutout)  
else:
  print "  Please fix the above problems"
