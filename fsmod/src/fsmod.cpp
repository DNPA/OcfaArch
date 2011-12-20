
//  The Open Computer Forensics Architecture moduleset.
//  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <fsmod.hpp>
#include <stdexcept>
#include <vector>
#include <regex.h>
using namespace
  ocfa::facade;
using namespace
  ocfa::evidence;
using namespace
  ocfa::misc;
void
FsModuleDissector::processEvidence ()
{
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject ();
  if (evidence == 0)
    throw OcfaException ("Evidence without evidence store", this);
  Evidence *output=derive(evidence->getAsFilePath(),Scalar("output","LATIN1"),"fsmodoutput");
  submitEvidence(output);
  delete(output);
  return;  
}


int
main (int argc, char *argv[])
{
  //FIXME: check if argc is valid and fill attributes array with od and even arguments: key name key name etc
  FsModuleDissector *fsmod = 0;
  try
  {
    string bname=basename(argv[0]);
    string fsmodkey=string("fsmod_") + bname;
    string fsmodstr=OcfaConfig::Instance()->getValue(fsmodkey);
    if (fsmodstr == "") throw OcfaException("No fs module found for FsModuleDissector: " + fsmodkey ,0);
    fsmod = new FsModuleDissector(bname,fsmodstr,0);
  } catch (OcfaException &ex)
  {
    cerr << "Ocfa Exeption cougth in module constructor: " << ex.
      what () << "\n";
    return 2;
  }
  try
  {
    fsmod->run ();
  }
  catch (OcfaException &ex)
  {
    fsmod->getLogStream (LOG_CRIT) << "Ocfa Exeption cougth: " << ex.
      what () << "\n";
  }
  catch (...)
  {
    fsmod->getLogStream (LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  fsmod->PrintObjCount ();
  delete fsmod;
  return 1;
}
