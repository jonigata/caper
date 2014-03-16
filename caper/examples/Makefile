.SUFFIXES: .o .c .cpp .hpp .cpg

CPPFLAGS = -g -Wall -DLINUX 

.cpg.hpp:
	../caper $< $@

.cpp.o:
	g++ $(CPPFLAGS) -c -o $@ $<

all: recovery hello0 hello1 hello2 calc0 calc1 calc2

recovery: recovery.o 
	g++ $(CPPFLAGS) -o $@ $^

recovery.o : recovery.cpp recovery.hpp 

calc0: calc0.o
	g++ $(CPPFLAGS) -o $@ $^

calc0.o : calc0.cpp calc0.hpp 

calc1: calc1.o
	g++ $(CPPFLAGS) -o $@ $^

calc1.o : calc1.cpp calc1_parser.hpp 

calc2: calc2.o
	g++ $(CPPFLAGS) -o $@ $^

calc2.o : calc2.cpp calc2_parser.hpp 

hello0: hello0.o
	g++ $(CPPFLAGS) -o $@ $^

hello0.o : hello0.cpp hello0.hpp

hello1: hello1.o
	g++ $(CPPFLAGS) -o $@ $^

hello1.o : hello1.cpp hello1.hpp

hello2: hello2.o
	g++ $(CPPFLAGS) -o $@ $^

hello2.o : hello2.cpp hello2.hpp

clean :
	rm -f *.o hello0.o hello1.o hello2.o calc0.o calc1.o calc2.o 
	rm -f hello0 hello1 hello2 calc0 calc1 calc2
	rm -f hello0.hpp hello1.hpp hello2.hpp calc0.hpp calc1_parser.hpp calc2.o calc2_parser.hpp
	rm -f conflict0.hpp conflict1.hpp samerule.hpp
