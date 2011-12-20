//RJM: Not reviewed, for future use.
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

/**
 * File:          domrulelist.cpp
 * Project:       OCFA 2.0 / digiwash 2.0
 * Sub project:   Ocfa Architecture, Router
 * Author:        <jochen.van.der.wal@klpd.politie.nl
 * Organisation:  KLPD / NR
 */

#include "domrulelist.hpp"

using namespace router;

Match* DomRuleList::parseEvidence(JobIterator *ji, string TableName) {
      Match* m = new Match("DomAction", "DomTarget", "DomArgs");
      return m;
}

