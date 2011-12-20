#ifndef __PACKET_TYPE_HPP_
#define __PACKET_TYPE_HPP_

#include <string>
#include <vector>

typedef struct {
	//string  Table; //RJM:CODEREVIEW This filed should not be here
	std::string  NameSpace;
	std::string  Type;
	std::string  Value;
	std::string  LastMod;
} packet_type;

//const packet_type  EmptyPacket = {"EMPTY","EMPTY","EMPTY","EMPTY","EMPTY"};
const packet_type  EmptyPacket = {"EMPTY","EMPTY","EMPTY","EMPTY"}; //RJM:CODEREVIEW no Table field
typedef std::vector<packet_type> packet_vector_type;
#endif
