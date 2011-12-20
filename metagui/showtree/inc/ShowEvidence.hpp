
//  The Open Computer Forensics Architecture.
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

#include "OcfaObject.hpp"
#include <misc/OcfaException.hpp>
#include "store/EvidenceStoreEntity.hpp"
#include <misc/OcfaConfig.hpp>
#include <libpq-fe.h>


class ShowEvidence : public ocfa::OcfaObject {

public:

  ShowEvidence();
  virtual void openDatabase(ocfa::misc::OcfaConfig *config);
  void showXml(string inCase, string inSource, string inItem, string inId, ostream &out);
  void showXml(string inMetaDataId, ostream &out); 
  void showData(string inCase, string inSource, string inId, string inId2, ostream &out);
  void showData(string inDataId, ostream &out);
  void showDataFilePath(string inCase, string inSource, string inId, string inId2, ostream &out);

  void throwDatabaseException(string command, PGresult *inResult);


  virtual std::string  getEvidenceStoreHandle(string inCase, string inSource, string inItem, string inId);
protected:
  ShowEvidence(const ShowEvidence& se):
	  OcfaObject(se),
	  mConnection(0)
  {
	  throw ocfa::misc::OcfaException("No copying allowed of ShowEvidence",this);
  }
  const ShowEvidence& operator=(const ShowEvidence&) {
	  throw ocfa::misc::OcfaException("No assignment allowed of ShowEvidence",this);
	  return *this;
  }
private:
  PGconn *mConnection;
};
