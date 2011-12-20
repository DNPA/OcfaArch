// This file may be possably deleted. It is not currently used.
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
 * Module class header file
 **/
 #ifndef __MODULE_HPP__
 #define __MODULE_HPP__
 
 /**** Includes *************************************************************/
 #include <set>
 #include <status.hpp>
 
 
  
 /**** type definitions *****************************************************/
 
 typedef string module_type;
 
  
 /**
  * A class containing both needed module types and available module instances.
  * Supports methods to add needed types and available instances.
  **/
  class Module 
  {
  
    public:
      /**
       * Constructor
       */
      Module();
      /**
       * Destructor
       */
      ~Module();
      /**
       * Method AddAvailable adds a module instance to a list. 
       * The list of available module instances is used for routing purposes.
       * As a side effect the type of the given module instance will be deleted
       * from de Needed module list of types, if still present.
       * available module instances.
       * @param Instance a pointer to the ModuleInstance
       */  
      void AddAvailable(ModuleInstance *Instance);
      /**
       * Method AddNeeded adds a module type to a list of for routing needed
       * module types
       * @param ModuleType a string value describing the name of the Module type
       */
      void AddNeeded(string ModuleType);
      /**
       * Get target module instance
       * @param ModuleType A string describing the target  module type
       * @return The pointer to the module instance
       */
      ModuleInstance *GetTargetInstance(module_type ModuleType);
      /**
       * Clear available odule instnce list
       */
      void ClearAllAvailable();
      /**
       * Clear neede module type list
       * When a new rulelist is loaded, the needed module types should be reset
       * before loading a new rulelist. Under normal circumstances the list
       * should be empty
       */
      void ClearAllNeeded();
    
    private:
      set<module_type> mNeededModuleTypes;
      map<ModuleInstance, Status> mAvailableModuleInstances;
    
  } //end class module

/***************************************************************************/
class ModInstList  : public ocfa::Ocfa
{
  public:
    /**** constructor ****/
    ModInstList(Router *routerComponent) ;
    /**** destructor ****/
    ~ModInstList();

    /**** methods ****/
    void Add(ModuleInstance *Instance) ;
    bool getFirstByType(module_type ModType, ModuleInstance **Instance);

  private:
    Router		*_router;
    vector<ModuleInstance *> _vList;
    bool _Empty;
};

#endif
