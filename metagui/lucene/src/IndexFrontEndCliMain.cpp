
#include <misc/OcfaException.hpp>
#include <IndexCliFrontEnd.hpp>
int main(int argc, char *argv[]){

  if (argc < 2){

    cout << "You should at least specify a query " << endl;

  }
  else {

    try {
      IndexCliFrontEnd frontEnd(argc, argv);
      frontEnd.doSearch();
    } catch (ocfa::misc::OcfaException &e){

      cout << e.what();
    }
  }
}
