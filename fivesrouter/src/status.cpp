// RJM:CODEREVIEW This file may be deleted, it is not used.
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

#include "../inc/status.hpp"

// Constructor
Status::Status(int Interval = STATUS_DEFAULT_INTERVAL) 
                          : _Status(STS_NOTINIT),
                            _SubscriptionTime(0),
		            _LastUpdateTime(0) 
{
    _maxTimeInterval = (time_t)Interval;
};
//Default constructor
Status::Status()
                          : _Status(STS_NOTINIT)
			    _SubscriptionTime(0),
		            _LastUpdateTime(0)
{
    _maxTimeInterval = (time_t)STATUS_DEFAULT_INTERVAL;
};			    

// Destructor
Status::~Status()
{
   //Nothing to do?
};


void Status::Update()
{
           //Update the last time 
           (void)time(&_LastUpdateTime);
};


void Status::Register()
{
     if(_SubscriptionTime==0) {
           (void)time(&_SubscriptionTime);
     }   
};

status_type Status::getStatus()
{
  time_t nu;
  (void)time(&nu);
    if(nu - _LastUpdateTime > _maxTimeInterval) {
      _Status = STS_NOTRESPONDING;
    }
    else {
      //instance has responded within the last timeinterval
      
    }
    
    return _Status;
    
};
