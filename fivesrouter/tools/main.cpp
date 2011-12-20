
#include "Parser.hpp"


int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Usage: \n %s rulelist\n", argv[0]);
		return 1;
	}
	int rc = 1;
	
	try{

		Parser p(argv[1]);
	
		int version = p.identifyVersion(argv[1]);
//		printf("Rulelist version: %d\n", version);
		if (version != 2){
			printf("I can only validate version 2 rulelists. Aborting\n");
			return 1;
		}

		rc = p.parse();

		if (!rc){
			printf("The rulelist looks ok. Look for warnings though.\n");
		} else {
			printf("The rulelist has errors in it\n");
		}
	
	} catch (const char* e){
		printf("Error: %s\n", e);
	}

	return rc;
}


