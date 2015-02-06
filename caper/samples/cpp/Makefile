.SUFFIXES: .o .c .cpp .hpp .cpg .ipp

CPPFLAGS    = -g -Wall -DLINUX -std=c++11
CC	    = clang++

.cpp.o:
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.ipp : ../grammar/%.cpg ../../caper
	../../caper $< $@

all: hello0 hello1 hello2 calc0 calc1 calc2 recovery0 recovery1 rawlist0 rawlist1 rawlist2 rawoptional list0 list1 list2 optional 

../../caper:
	cd ../..; $(MAKE)

calc0: calc0.o
	$(CC) $(CPPFLAGS) -o $@ $^

calc0.o : calc0.cpp calc0.ipp

calc1: calc1.o
	$(CC) $(CPPFLAGS) -o $@ $^

calc1.o : calc1.cpp calc1.ipp

calc2: calc2.o
	$(CC) $(CPPFLAGS) -o $@ $^

calc2.o : calc2.cpp calc2.ipp

hello0: hello0.o
	$(CC) $(CPPFLAGS) -o $@ $^

hello0.o : hello0.cpp hello0.ipp

hello1: hello1.o
	$(CC) $(CPPFLAGS) -o $@ $^

hello1.o : hello1.cpp hello1.ipp

hello2: hello2.o
	$(CC) $(CPPFLAGS) -o $@ $^

hello2.o : hello2.cpp hello2.ipp

rawlist0: rawlist0.o
	$(CC) $(CPPFLAGS) -o $@ $^

rawlist0.o : rawlist0.cpp rawlist0.ipp

rawlist1: rawlist1.o
	$(CC) $(CPPFLAGS) -o $@ $^

rawlist1.o : rawlist1.cpp rawlist1.ipp

rawlist2: rawlist2.o
	$(CC) $(CPPFLAGS) -o $@ $^

rawlist2.o : rawlist2.cpp rawlist2.ipp

rawoptional: rawoptional.o
	$(CC) $(CPPFLAGS) -o $@ $^

rawoptional.o : rawoptional.cpp rawoptional.ipp

list0: list0.o
	$(CC) $(CPPFLAGS) -o $@ $^

list0.o : list0.cpp list0.ipp

list1: list1.o
	$(CC) $(CPPFLAGS) -o $@ $^

list1.o : list1.cpp list1.ipp

list2: list2.o
	$(CC) $(CPPFLAGS) -o $@ $^

list2.o : list2.cpp list2.ipp

optional: optional.o
	$(CC) $(CPPFLAGS) -o $@ $^

optional.o : optional.cpp optional.ipp

recovery0: recovery0.o
	$(CC) $(CPPFLAGS) -o $@ $^

recovery0.o : recovery0.cpp recovery0.ipp

recovery1: recovery1.o
	$(CC) $(CPPFLAGS) -o $@ $^

recovery1.o : recovery1.cpp recovery1.ipp

clean :
	rm -f *.o 
	rm -f *.ipp
	rm -f hello0 hello1 hello2 calc0 calc1 calc2 recovery0 recovery1 rawlist0 rawlist1 rawlist2 rawoptional list0 list1 list2 optional

test : calc2
	cd ../test; $(MAKE)

