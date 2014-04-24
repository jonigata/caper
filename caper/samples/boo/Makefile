.SUFFIXES: .boo .cpg .exe


BOOC	= booc

%_parser.boo: ../grammar/%.cpg ../../caper
	../../caper -boo $< $@ --debug

%_parser.cs: ../grammar/%.cpg ../../caper
	../../caper -cs $< $@ --debug

%.exe: %_parser.boo %.boo
	$(BOOC) -o:$@ $^

all: calc0.exe calc0_parser.cs

../../caper:
	cd ../..; $(MAKE)

calc0.exe: calc0_parser.boo calc0.boo


clean :
	rm -f *.o 
	rm -f *.ipp
	rm -f hello0 hello1 hello2 calc0 calc1 calc2 recovery0 recovery1 rawlist0 rawlist1 rawlist2 rawoptional list0 list1 list2 optional

test : calc2.o
	cd ../test; $(MAKE)

