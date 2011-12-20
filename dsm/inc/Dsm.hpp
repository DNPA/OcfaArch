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

#include <facade/XMLAccessor.hpp>
#include <misc/OcfaException.hpp>
#include <vector>
#include <libpq-fe.h>
#include <map>

/**
 * Module that is responsible for preparing the data for long time
 * storage. Grabs the data and stores the metadata that should be
 * quickly retrievable into different tables.
 * TODO: respond to ACT_COMMIT, ACT_SUSPEND requests.
 * TODO: add escapetodatabasestring stuff 
 */
class Dsm : public ocfa::facade::XMLAccessor {

public:

  /**
   * constructor that also checks that the needed extra tables exists.
   */
  Dsm();

  ~Dsm();
  /**
   *
   * different from the default in that it does not sent something back to 
   * the router.
   */
  virtual void processEvidenceMessage(const ocfa::message::Message &);
  
  /**
   * dissects the evidence message and stores its metadata ino
   *
   */
  virtual void processEvidence();
  //  virtual void initialize(std::string inName, std::string inNameSpace);
  //virtual void initialize(std::string inName, std::string inNamespace);
protected:
  PGconn *openDatabase();
  void insertLocation();
  void ensureLocationTable();
  void ensureColumn(std::string inColumnName, std::string inType, 
		    std::string inDefault);
  
  /**
   * checks whether a table for the specified metaname exists.
   * @param inMetaName the name of the metadata for which a metdata shoudl exist.
   * @param connection the connection to the database.
   * @return whether the table exists or not. 
   */
  bool tableExists(string inMetaName);
  /**
   * creates the needed metatables if necessary. Used the ocfa configutatio metatabledescription to
   * find the file that contains the data for the metatable.
   *
   */
  void createMetaTables();
  /**
   * ensures that a table exists for tableName which is a tablemetavalue with headers for its columnNames.
   * @param metaName the  name of the metadata whose contents should be stored in a table.
   * @param headers the headers that make up the columns for the tablemetavalues that should be
   *  stored in it.
   * @param connection the connection to the database.
   */
  void ensureMetaTableTable(std::string metaName, std::vector<string> headers);

  /**
   * ensures that a table exist for a metaName that is either for arraymetadata or scalarmetadata. 
   * @param metaName the name of the metaData that should be stored into a seperate table.
   * @param connection the connection to the postgres database. 
   *
   */
  void ensureMetaNameTable(std::string metaName);
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

  /**
   * updates a standardMetaValue It is used for a try when the insert gives errors.
   * The idea behind is, is that when later modules know better than earlier modules. So that when two
   * modules add the same metadatatype. the most recent knows best.
   *
   *
   */
  void updateStandardMetaValue(ocfa::misc::MetaValue *value, std::string inName);
  
  /**
   * gets the content of inValue as a string. If inValue is a scalar, it is returned as Utf8, if inValue is 
   * and array it, a concatenation of all elements, seperated by spaces, is returned .
   *
   */
  std::string getStringValue(ocfa::misc::MetaValue &inValue);
  Dsm(const Dsm& dsm):
	  ocfa::facade::XMLAccessor(dsm),
	  mInterestingMeta(),
	  mConnection(0),
          mTransaction(false)
  {
       throw ocfa::misc::OcfaException("No copying allowed",this);
  }
  const Dsm& operator=(const Dsm&) {
       throw ocfa::misc::OcfaException("No assignment allowed",this);
       return *this;
  }
private:
  /**
   * helper to escape strings to be used in sql statements
   */
  std::string escape(std::string in);
  /**
   * map that stores which metadata should be put into seperate tables and which should not. 
   *
   */
  std::map<std::string, bool> mInterestingMeta;
  /**
   * connection to the database.
   */
  PGconn *mConnection;
  /**
    * bool indicating if we should make processevidence
    * into a single transaction for the database
    */
  bool mTransaction;
};

#endif
