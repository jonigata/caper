.SUFFIXES: .boo .cpg .exe


BOOC	= booc

%_parser.boo: ../grammar/%.cpg ../../caper
	../../caper -boo $< $@ --debug

%.exe: %_parser.boo %.boo
	$(BOOC) -o:$@ $^

all: calc0.exe

../../caper:
	cd ../..; $(MAKE)

calc0.exe: calc0_parser.boo calc0.boo

clean :
	rm -f *_parser.boo
	rm -f *.exe
	rm -f *.exe.mdb

test : calc2.o
	cd ../test; $(MAKE)

