#include <stdlib.h>
#include <iostream>

int main(int argc,char **argv) {
   char *bigstuff=(char *) calloc(1,543210123);
   bigstuff[543210120]='a';
   bigstuff[0]='b';
   std::cout << bigstuff << bigstuff+543210120 << std::endl; 
}
