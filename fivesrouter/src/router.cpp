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

#include <router.hpp>
#include <csv/RuleList.hpp>

using namespace ocfa;
using namespace evidence;
using namespace message;
using namespace router;

/**
 * Globals
 */
 


/**
 * Constructor Router class
 */
Router::Router()
                : TargetAccessor("router", "core"), mRuleList(NULL), mGlobalNamespace("klpd") {
  
  /* Get singleton instance of RuleList, this will also load the rulelist file from
   * the location specified in de configuration file */
  mRuleList = RuleList::getInstance();
  SetMetaFacet smf(this);
  mRuleList->initSetMetaFacet(smf);
  std::string ns=OcfaConfig::Instance()->getValue("defaultnamespace"); //RJM:CODEREVIEW we dont want to overwrite if result is empty
  if (ns != "")                                                        //RJM:CODEREVIEW
	  mGlobalNamespace = ns;					//RJM:CODEREVIEW
  mDoIntegerVals = (OcfaConfig::Instance()->getValue("routeonint") == "true");
  mDoIntegerNames = mDoIntegerVals ||  (OcfaConfig::Instance()->getValue("routeonint") == "name");
  mDoFloatVals = (OcfaConfig::Instance()->getValue("routeonfloat") == "true");
  mDoFloatNames = mDoFloatVals || (OcfaConfig::Instance()->getValue("routeonfloat") == "name");
  mDoDateVals = (OcfaConfig::Instance()->getValue("routeondate") == "true");
  mDoDateNames = mDoDateVals || (OcfaConfig::Instance()->getValue("routeondate") == "name");
  mDoArrayNames = (OcfaConfig::Instance()->getValue("routeonarray") != "false");
  mDoTableNames =  (OcfaConfig::Instance()->getValue("routeontable") != "false");
  mDoExtendedEvidence = (OcfaConfig::Instance()->getValue("extendedevidence") == "true");
} //constructor

/**
 * Destructor
 */
Router::~Router() {
  /**** Destructor *********************************************************/
  delete mRuleList;
}

/**
 * Process Evidence
 * Implements only the narrow algorithm: Looks at all the metadata only of non 'done' jobs.
 * The code in this method is almost identical to the old 1.0 router
 */
void Router::processEvidence() {
 MetaIterator 		*metaIter=0;
 ModuleInstance 	*moduleInstance=0;
 JobIterator		*jobiterator=0;
 packet_type 		Packet=EmptyPacket;
 //bool                   ContainerEmpty=true; // RJM:CODEREVIEW more general name
 vector<packet_type>    packetcontainer; // RJM:CODEREVIEW This realy should not be a vector. Please look at the proposed
                                         // PacketContainer class interface for a simple container class that should abstact
					 // away most of the gorry details that realy should not be here at the router level.
 ExtendableEvidence *extendableEvidence = dynamic_cast<ExtendableEvidence *>(getEvidence());
 
 if(mDoExtendedEvidence) {
    extendableEvidence->setGlobalMetaFacades(true);
    //getLogStream(LOG_DEBUG) << "GlobalMetaFacades = TRUE" << endl;
 } else {
    extendableEvidence->setGlobalMetaFacades(false);
    //getLogStream(LOG_DEBUG) << "GlobalMetaFacades = FALSE" << endl;
 }
 
 jobiterator= extendableEvidence->getJobIterator();

  if(jobiterator == 0) {
    getLogStream(LOG_CRIT) << "No job iterator found for given evidence in process evidence function" << endl;
    throw OcfaException("Unable to fetch job iterator from active evidence",this);
    //delete xml;
    return ;
  }

  do { /*while jobiterator->next() */
    /*RJM:CODEREVIEW switching the isDone() and the moduleInstance != 0 test 
      would make adding the shalow vector more natural. */
/* FIVES-PH: Rulelist version 2 ignores isDone because all meta data may be used for routing at all times */
    if(!jobiterator->isDone() || RuleList::getInstance()->getRuleListVersion() == 2) {  //for all jobs not done (new or processed)
      //for all processed jobs
      //get ModuleInstance
      moduleInstance = jobiterator->getModuleInstance();
      if(moduleInstance != 0) {
        Packet=EmptyPacket;
	Packet.NameSpace = moduleInstance->getNameSpace();
        Packet.LastMod = moduleInstance->getModuleName();
        Packet.Type = "ModuleInstance";
	Packet.Value= moduleInstance->getModuleName();
        packetcontainer.push_back(Packet);
        //get Metadata iterator
        metaIter =  jobiterator->getMetaIterator();
	if(metaIter != 0) {
          do{ //while metaData->next == TRUE
            //for all metaData items
            Packet.Type = metaIter->getName();
	    Packet.Value="EMPTY";
	    switch (metaIter->getMetaVal()->getType()) {
	      case META_SCALAR:
	        {
	          ScalarMetaValue *scalarValue = 
		      dynamic_cast<ScalarMetaValue *>(metaIter->getMetaVal());
	          if (scalarValue == 0){
		     throw OcfaException("cannot cast metascalarvalue to metascalar value", this);
	          }
		  switch (scalarValue->asScalar().getType()) {
		    case ocfa::misc::Scalar::SCL_STRING : 
		      {
	                //Only process Metadata of type string
                        Packet.Value = scalarValue->asScalar().asUTF8();
			packetcontainer.push_back(Packet); //fill vector with found packet
	              }
		      break;
		    case ocfa::misc::Scalar::SCL_INT:
		      if (mDoIntegerNames) {
                         if (mDoIntegerVals) {
                            Packet.Value = scalarValue->asScalar().asASCII();
			 } else {
                            Packet.Value = "EMPTY";
			 }
			 packetcontainer.push_back(Packet); //fill vector with found packet
		      }
		      break;
		    case ocfa::misc::Scalar::SCL_FLOAT:
		       if (mDoFloatNames) {
                           if (mDoFloatVals) {
                              Packet.Value = scalarValue->asScalar().asASCII();
			   } else {
                              Packet.Value = "EMPTY";
			   }
		           packetcontainer.push_back(Packet); //fill vector with found packet
		       }
		       break;
		    case ocfa::misc::Scalar::SCL_DATETIME:
		       if (mDoDateNames) {
		           if (mDoDateVals) {
                               Packet.Value = scalarValue->asScalar().asASCII();
			   } else {
                               Packet.Value = "EMPTY";
			   }
                           packetcontainer.push_back(Packet); //fill vector with found packet
		       }
		       break;
		    case ocfa::misc::Scalar::SCL_INVALID:
		      getLogStream(LOG_DEBUG) << "Scalar is SCL_INVALID";
		    default:
		      getLogStream(LOG_DEBUG) << "Scalar '" << metaIter->getName() << "' is not a known type. No processing";
		      break;
		  }    
                } //META_SCALAR type
		break;
              case META_ARRAY:
                if (mDoArrayNames) 
		   packetcontainer.push_back(Packet);
	        break;
	      case META_TABLE:
	        if (mDoTableNames)
		   packetcontainer.push_back(Packet);
		break;
	      default:
	        break;

	    }


          } while(metaIter->next());
	  //if(metaIter != 0) delete metaIter;
	} // metaIter != 0

	//delete moduleInstance;
      } //if moduleInstance != 0
      jobiterator->setDone();

    } // if jobiterator->isProcessed() == TRUE
    //RJM:CODEREVIEW sugestion, we may as well fill the shalow vector on the first pass
    // moduleInstance = jobiterator->getModuleInstance();     
    // if (moduleInstance) {
    //   Packet=EmptyPacket;
    //   Packet.NameSpace = moduleInstance->getNameSpace();
    //   Packet.LastMod = moduleInstance->getModuleName();
    // }
  } while (jobiterator->next());
  /*DEBUG: Print packet vector
  vector<packet_type>::iterator piPv;
  for(piPv = packetcontainer.begin() ; piPv != packetcontainer.end() ; ++piPv) {
    ocfa::misc::OcfaLogger::Instance()->syslog(LOG_INFO, "Router::processEvidence ") << "packetcontainer:: " << (*piPv).Table << "  " << (*piPv).NameSpace << "  " << (*piPv).Type << "  " << (*piPv).Value << "  " << (*piPv).LastMod << "\n";
  } */
  
  /* PerformRouting uses the final action thrown up by parsePacketVector. 
   * ParsePacketVector uses in 
   */
  /* RJM:CODEREVIEW 
   What kind of hubris is this, we should not get cut of from the Rule that 
   matches at this point. 
   parsePacketVector should probably return either:
   1) a single Rule (leaving the jumping to other tables on this level) 
   2) a vector of Rules where the last rule holds the final action.
   3) Both a final action and a vector of side actions.
   I would sugest either 1 or 2. */
  
  FinalAction *fa = 0;
  if (mRuleList->getRuleListVersion() == 2)
    fa= mRuleList->parsePacketVector(packetcontainer, extendableEvidence);
  else{
    fa= mRuleList->parsePacketVector(packetcontainer, "default");
    logEvidence(LOG_DEBUG,string("routing as a result of:") +mRuleList->getMatchPacketString("default"));
  }
  PerformRouting(fa);

} //end Router::processEvidence


void Router::PerformRouting(FinalAction *fa){

  FinalActionCommit                      *Commit;
  FinalActionForward                     *Forward;
  FinalActionSuspend                     *Suspend;
  
  
  /*RJM:CODEREVIEW if we dont call setTarget, the library will throw an exception. 
  Better to do it ourselves.  */
  //Look at final action type to decide what to do
  switch(fa->getType()) {
     case FinalAction::FACT_COMMIT:
       {
        JobInfo jobInfo(COMMIT_DEFAULT);
        addNewJob(&jobInfo);
	if((Commit = dynamic_cast<FinalActionCommit *>(fa)) == 0){
	   throw OcfaException("Dynamic cast exception on FinalAction", this);
	}

	getLogStream(LOG_INFO) << "setting target to " << mGlobalNamespace << ":" 
			       << Commit->getValue() << endl; 
        setTarget(Commit->getValue(),mGlobalNamespace);
       }
        break;
     case FinalAction::FACT_FORWARD:
       {
	if((Forward = dynamic_cast<FinalActionForward *>(fa)) == 0) {
	   throw OcfaException("Dynamic cast exception on FinalAction", this);
	}
        JobInfo jobInfo(FinalAction::FACT_FORWARD);
        addNewJob(&jobInfo);        
	setTarget(Forward->getValue(),mGlobalNamespace);
       }
        break;
     case FinalAction::FACT_JUMP:    //RJM:CODEREVIEW parsePacketVector should never return this !!
        //FIXME: not implemented yet
        break;
     case FinalAction::FACT_SUSPEND:
       {
	if((Suspend = dynamic_cast<FinalActionSuspend *>(fa)) == 0) {
	   throw OcfaException("Dynamic cast exception on FinalAction", this);
	}
        JobInfo jobInfo(COMMIT_SUSPEND);
        addNewJob(&jobInfo);
        setTarget(Suspend->getValue(),mGlobalNamespace);
       }
        break;
     case FinalAction::FACT_RETURN:  //RJM:CODEREVIEW parsePacketVector should never return this !!
     case FinalAction::FACT_DONOTHING: //RJM:CODEREVIEW parsePacketVector should never return this !!
     default:
        //To finish the story...
	getLogStream(LOG_NOTICE) << "Not supported or implemented Final Action handling";
        break;
  } //end switch final action type

} //end PerformRouting
