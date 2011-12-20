
CXXFLAGS += -ggdb -Wall


validator: main.o Parser.o Lexer.o Expression.o 
	g++ -o validator main.o Parser.o Lexer.o Expression.o $(CXXFLAGS)

test: validator rulelists/*
	./test.sh

clean:
	rm validator *.o
