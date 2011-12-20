#ifndef __SETMETA_FACET__
#define __SETMETA_FACET__

//#include "router.hpp" Use class forward declaration instead, preventing include loops


namespace router {

   class Router;  /* class forward declaration. Make usage of Router class possible,
                      without including router.h */
   
   class SetMetaFacet {
      private:
         Router *mRouter;
      public:
         SetMetaFacet(Router *r) : mRouter(r) {
         };
         void operator() (std::string n, ocfa::misc::Scalar s);
   };
   
} //end namespace router

#endif
