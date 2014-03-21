.SUFFIXES: .o .c .cpp .hpp .cpg

CPPFLAGS = -g -Wall -DLINUX 

.cpp.o:
	g++ $(CPPFLAGS) -c -o $@ $<

all: hello0 hello1 hello2 calc0 calc1 calc2 recovery recovery2

recovery: recovery.o
	g++ $(CPPFLAGS) -o $@ $^

recovery.o : recovery.cpp recovery.hpp

recovery.hpp : recovery.cpg ../caper
	../caper $< $@

recovery2: recovery2.o
	g++ $(CPPFLAGS) -o $@ $^

recovery2.o : recovery2.cpp recovery2.hpp

recovery2.hpp : recovery2.cpg ../caper
	../caper $< $@

calc0: calc0.o
	g++ $(CPPFLAGS) -o $@ $^

calc0.o : calc0.cpp calc0.hpp

calc0.hpp : calc0.cpg ../caper
	../caper $< $@

calc1: calc1.o
	g++ $(CPPFLAGS) -o $@ $^

calc1.o : calc1.cpp calc1_parser.hpp

calc1_parser.hpp : calc1_parser.cpg ../caper
	../caper $< $@

calc2: calc2.o
	g++ $(CPPFLAGS) -o $@ $^

calc2.o : calc2.cpp calc2_parser.hpp

calc2_parser.hpp : calc2_parser.cpg ../caper
	../caper $< $@

hello0: hello0.o
	g++ $(CPPFLAGS) -o $@ $^

hello0.o : hello0.cpp hello0.hpp

hello0.hpp : hello0.cpg ../caper
	../caper $< $@

hello1: hello1.o
	g++ $(CPPFLAGS) -o $@ $^

hello1.o : hello1.cpp hello1.hpp

hello1.hpp : hello1.cpg ../caper
	../caper $< $@

hello2: hello2.o
	g++ $(CPPFLAGS) -o $@ $^

hello2.o : hello2.cpp hello2.hpp

hello2.hpp : hello2.cpg ../caper
	../caper $< $@

clean :
	rm -f *.o hello0.o hello1.o hello2.o calc0.o calc1.o calc2.o recovery.o recovery2.o
	rm -f hello0 hello1 hello2 calc0 calc1 calc2 recovery recovery2
	rm -f hello0.hpp hello1.hpp hello2.hpp calc0.hpp calc1_parser.hpp calc2.o calc2_parser.hpp recovery.hpp recovery2.hpp
	rm -f conflict0.hpp conflict1.hpp samerule.hpp

test : calc2.o
	./calc2 < calc2.input | diff calc2.expected -

