.SUFFIXES: .o .c .cpp .d .hpp

CC		= clang++
CPPFLAGS	= -g -Wall -DLINUX -std=c++11
TARGET		= capella
OBJS		= $(TARGET).o capella_dependency.o capella_generate_boost.o capella_generate_cpp.o capella_generate_dot.o capella_generate_stub.o capella_generate_cs.o
DEPENDDIR	= ./depend

VPATH = ./depend

.c.d:
	$(CC) $(CDBFLG) $(CXXFLAGS) $(INC) $(MACHINE) $(ENVIRON) -MM $< > $(DEPENDDIR)/$*.d.t
	cp $(DEPENDDIR)/$*.d.t $(DEPENDDIR)/$*.d
	sed -e 's/$*\.o/$*\.d/g' $(DEPENDDIR)/$*.d.t >> $(DEPENDDIR)/$*.d
	rm $(DEPENDDIR)/$*.d.t
	echo '$*.o : $*.d' >> $(DEPENDDIR)/$*.d	

.cpp.d:
	$(CC) $(CDBFLG) $(CXXFLAGS) $(INC) $(MACHINE) $(ENVIRON) -MM $< > $(DEPENDDIR)/$*.d.t
	cp $(DEPENDDIR)/$*.d.t $(DEPENDDIR)/$*.d
	sed -e 's/$*\.o/$*\.d/g' $(DEPENDDIR)/$*.d.t >> $(DEPENDDIR)/$*.d
	rm $(DEPENDDIR)/$*.d.t
	echo '$*.o : $*.d' >> $(DEPENDDIR)/$*.d	

.cpp.o:
	$(CC) $(CPPFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CPPFLAGS) -c -o $@ $<

all:
	mkdir -p depend
	$(MAKE) depend -s
	$(MAKE) $(TARGET)

depend: $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^

capella.d : capella.hpp

capella.hpp : capella.cpg
	../caper/caper capella.cpg capella.hpp

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f $(DEPENDDIR)/*.d

-include $(addprefix $(DEPENDDIR)/,$(OBJS:.o=.d))
