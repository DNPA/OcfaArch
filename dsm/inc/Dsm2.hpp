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

#ifndef DSM_HPP
#define DSM_HPP

#include <facade/DataStoreModule.hpp>
#include <misc/OcfaException.hpp>
#include <vector>
#include <libpq-fe.h>
#include <map>
#include "MetaFilter.hpp"
/**
 * Module that is responsible for preparing the data for long time
 * storage. Grabs the data and stores the metadata that should be
 * quickly retrievable into different tables.
 * TODO: respond to ACT_COMMIT, ACT_SUSPEND requests.
 * TODO: add escapetodatabasestring stuff 
 */
class Dsm2 : public ocfa::facade::DataStoreModule {

public:

  /**
   * constructor that also checks that the needed extra tables exists.
   */
  Dsm2();

  ~Dsm2();
  /**
   * dissects the evidence message and stores its metadata ino
   *
   */
  virtual void processEvidence();
protected:
  PGconn *openDatabase();
  /**
   * simple helper method to throw a nice exception. 
   *
   */
  void throwDatabaseException(std::string command, PGresult *inResult);
  /**
   * puts a tablemetavalue in to a table for quick retrieval.
   * @param inTableValue the tablevalue that should be stored. 
   * @param inName the name of the metadata.
   *
   */
  void insertMetaTableValue(ocfa::misc::TableMetaValue *inTableValue, std::string inName);

  /**
   * inserts a standardmetavalue (either a array or a scalarmetavalue)
   * into a table for quick retrieval.
   * @param inMetaValue the metavalue that should be stored.
   * @param inNamer the name under which  the metavalue was stored.. 
   *
   */
  void insertStandardMetaValue(ocfa::misc::MetaValue *inMetaValue, std::string inName);

  void insertArrayMetaValue(ocfa::misc::ArrayMetaValue *inMetaValue, std::string inName);

  Dsm2(const Dsm2& dsm):
	  ocfa::facade::DataStoreModule(dsm),
	  mConnection(0)
  {
       throw ocfa::misc::OcfaException("No copying allowed",this);
  }
  const Dsm2& operator=(const Dsm2&) {
       throw ocfa::misc::OcfaException("No assignment allowed",this);
       return *this;
  }

  /**
   * JBS: inserts a location in the database.
   * 
   */
  void insertLocation();
  
private:
   void insertModuleName(std::string modulename,size_t jobno);
   void insertLogLine(std::string modulename,size_t jobno,std::string logline);
  /**
   * helper to escape strings to be used in sql statements
   */
  std::string escape(std::string in);
  /**
   * connection to the database.
   */
  PGconn *mConnection;
  /**
   *  Filter and lazy table creation.
   */
  MetaFilter *mFilter;

  bool mLog2Db;
  bool mModules2Db;
};

#endif
