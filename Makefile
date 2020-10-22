CXX = /usr/local/bin/g++
EXEC = gcalc
OBJS = Gcalc.o Graph.o Auxiliaries.o Parser.o main.o
HFLS = Gcalc.h Graph.h Auxiliaries.h Parser.h GraphExeception.h
MSCFLS = test_in.txt test_out.txt Makefile design.pdf
CPPFLS = Graph.cpp  Auxiliaries.cpp Parser.cpp Gcalc.cpp main.cpp
COMP_FLAG = -std=c++11  -Wall -pedantic-errors -Werror -g -fPIC
DEBUG_FLAG = -DNDEBUG


$(EXEC): $(OBJS) Makefile
	$(CXX) $(DEBUG_FLAG) $(OBJS) -o $@


main.o: main.cpp Gcalc.h Graph.h GraphExeception.h
	$(CXX) -c $(COMP_FLAG) $(DEBUG_FLAG) $*.cpp

Parser.o: Parser.cpp


Auxiliaries.o: Auxiliaries.cpp Graph.h


Graph.o: Graph.cpp Graph.h GraphExeception.h Parser.h
	$(CXX) -c $(COMP_FLAG) $(DEBUG_FLAG) $*.cpp


Gcalc.o: Gcalc.cpp Gcalc.h Graph.h GraphExeception.h Parser.h Auxiliaries.h
	$(CXX) -c $(COMP_FLAG) $(DEBUG_FLAG) $*.cpp


tar:  $(HFLS) $(CPPFLS) $(MSCFLS)
	zip Gcalc.zip $(EXEC) $(HFLS) $(CPPFLS) $(MSCFLS)


clean:
	rm -f $(OBJS) $(EXEC)