#ifndef __PACKET_TYPE_HPP_
#define __PACKET_TYPE_HPP_
typedef struct {
	//string  Table; //RJM:CODEREVIEW This filed should not be here
	string  NameSpace;
	string  Type;
	string  Value;
	string  LastMod;
} packet_type;

//const packet_type  EmptyPacket = {"EMPTY","EMPTY","EMPTY","EMPTY","EMPTY"};
const packet_type  EmptyPacket = {"EMPTY","EMPTY","EMPTY","EMPTY"}; //RJM:CODEREVIEW no Table field
typedef vector<packet_type> packet_vector_type;
#endif
