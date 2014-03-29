.SUFFIXES: .js .cpg

%.ijs : ../grammar/%.cpg ../../caper
	../../caper $< $@ --debug -js

all: hello0 hello1 hello2 calc0 calc1 calc2 recovery recovery2 rawlist0 rawlist1 rawlist2 rawoptional list0 list1 list2 optional 

../../caper:
	cd ../..; $(MAKE)

hello0: hello0.ijs

hello1: hello1.ijs

hello2: hello2.ijs

calc0: calc0.ijs

calc1: calc1.ijs

calc2: calc2.ijs

rawlist0: rawlist0.ijs

rawlist1: rawlist1.ijs

rawlist2: rawlist2.ijs

rawoptional: rawoptional.ijs

list0: list0.ijs

list1: list1.ijs

list2: list2.ijs

optional: optional.ijs

recovery: recovery.ijs

recovery2: recovery2.ijs

clean :
	rm -f *.ijs
	rm -f hello0 hello1 hello2 calc0 calc1 calc2 recovery recovery2 rawlist0 rawlist1 rawlist2 rawoptional list0 list1 list2 optional

test : calc2.o
	cd ../test; $(MAKE)

