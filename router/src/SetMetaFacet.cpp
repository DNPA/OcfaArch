
#include "router.hpp"

namespace router {

         void SetMetaFacet::operator() (std::string n, ocfa::misc::Scalar s) {
            return mRouter->setMeta(n,s);
         }

}
