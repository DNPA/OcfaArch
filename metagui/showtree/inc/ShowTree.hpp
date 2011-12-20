
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
#include <misc/OcfaConfig.hpp>
#include <misc/OcfaException.hpp>
#include <libpq-fe.h>
#include <string>
#include <vector>

/**
 * Simple CGI script that shows cases, sources, and items. 
 *
 */
class ShowTree : public ocfa::OcfaObject {
public:
  ShowTree();
  /**
   * sets mConnection to a connection in the database. 
   */
  virtual void openDatabase(ocfa::misc::OcfaConfig *config);
  /**
   * prints a nice overview of cases. 
   */
  void showCases();
  /**
   * prints over view of items within a case and an itemsource. 
   */
  void showItems(std::string inCase, std::string itemSource);
  /**
   * prints all evidence sources within a case. Used by the other show<blah>
   * methods
   *
   */
  void showEvidenceSources(std::string &inCase);

  // not sure if used. 
  void showMetaID(std::string inMetaID);

  /**
   * prints a list of links
   */
  void showList(std::vector<std::string> links);
  /**
   * throws a neat databaseexception
   */
  void throwDatabaseException(std::string inCommand, PGresult *inResult);
protected:
  ShowTree(const ShowTree& st):
	  OcfaObject(st),
	  mConnection(0)
  {
      throw ocfa::misc::OcfaException("No copying allowed for ShowTree",this);
  }
  const ShowTree& operator=(const ShowTree&) {
      throw ocfa::misc::OcfaException("No assignment allowed for ShowTree",this);
      return *this;
  }

private:
  PGconn *mConnection;
}; 
