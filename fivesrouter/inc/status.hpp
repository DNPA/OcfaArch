//RJM:CODEREVIEW This file may be deleted. It is not used.
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

#include <ctime> 


 /**
 * Status class header file
 **/
 #ifndef __STATUS_HPP__
 #define __STATUS_HPP__
 
 /**
  * The default value for the interval time in seconds
  */
 #define STATUS_DEFAULT_INTERVAL 120

 /**
  * A class containing the status of a Module Instance
  */
 class Status {
    public:
       /**
        * status_type enum
        * Defines the different status of a Module Instance
        */
       typedef enum status_type {
           STS_NOTINIT=0,            /**< enum value NotInit */
           STS_IDLE,                 /**< enum value Idle */
           STS_RUNNING,              /**< enum value Running */
           STS_OVERLOADED,           /**< enum value Overloaded */
           STS_NOTRESPONDING         /**< enum value NotResponding */ 
       };
    
       /**
        * Constructor
	* @param Interval Is the time interval in seconds. Default = 60 sec.
	*/
       Status(int Interval);
       /**
        * Default constructor
	*/
       Status();
       /**
        * Destructor
	*/
       ~Status();
       /**
        * Update the status
	*/       
       void Update();
       /**
        * Register a new status instance
	*/
       void Register();
       /**
        * get de status
	* @return The current status of de Status object
	*/
       status_type getStatus();
       
       
    /**** Private parts of class ********************************************/   
    private:
       time_t        _LastUpdateTime;
       time_t        _SubscriptionTime;
       time_t        _maxTimeInterval; /**< The maximum time that a module instance may be idle */
       status_type   _Status;
       int           _NumberOfJobsInBuffer;
 };
 
 #endif
