.SUFFIXES: .cpg .id

TARGET  = list0 list1 list2 optional hello0 hello1 hello2 calc0 list0 list1 list2 optional # calc1 calc2 recovery recovery2 rawlist0 rawlist1 rawlist2 rawoptional list0 list1 list2 optional 

%.ipp : ../grammar/%.cpg ../../caper
	../../caper $< $@ --debug -d

all: $(TARGET)

../../caper:
	cd ../..; $(MAKE)

hello0: hello0.d hello0_parser.d
	gdc -o $@ $^

hello0_parser.d : ../grammar/hello0.cpg ../../caper
	../../caper $< $@ --debug -d

hello1: hello1.d hello1_parser.d
	gdc -o $@ $^

hello1_parser.d : ../grammar/hello1.cpg ../../caper
	../../caper $< $@ --debug -d

hello2: hello2.d hello2_parser.d
	gdc -o $@ $^

hello2_parser.d : ../grammar/hello2.cpg ../../caper
	../../caper $< $@ --debug -d

calc0: calc0.d calc0_parser.d
	gdc -o $@ $^

calc0_parser.d : ../grammar/calc0.cpg ../../caper
	../../caper $< $@ --debug -d

calc1: calc1.d calc1_parser.d
	gdc -o $@ $^

calc1_parser.d : ../grammar/calc1.cpg ../../caper
	../../caper $< $@ --debug -d

calc2: calc2.d calc2_parser.d
	gdc -o $@ $^

calc2_parser.d : ../grammar/calc2.cpg ../../caper
	../../caper $< $@ --debug -d

list0: list0.d list0_parser.d
	gdc -o $@ $^

list0_parser.d : ../grammar/list0.cpg ../../caper
	../../caper $< $@ --debug -d

list1: list1.d list1_parser.d
	gdc -o $@ $^

list1_parser.d : ../grammar/list1.cpg ../../caper
	../../caper $< $@ --debug -d

list2: list2.d list2_parser.d
	gdc -o $@ $^

list2_parser.d : ../grammar/list2.cpg ../../caper
	../../caper $< $@ --debug -d

optional: optional.d optional_parser.d
	gdc -o $@ $^

optional_parser.d : ../grammar/optional.cpg ../../caper
	../../caper $< $@ --debug -d


clean :
	rm -f $(TARGET)
	rm -f hello0_parser.d hello1_parser.d hello2_parser.d calc0_parser.d calc1_parser.d calc2_parser.d recovery_parser.d recovery2_parser.d rawlist0_parser.d rawlist1_parser.d rawlist2_parser.d rawoptional_parser.d list0_parser.d list1_parser.d list2_parser.d optional_parser.d

test : calc2.o
	cd ../test; $(MAKE)

