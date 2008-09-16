.SUFFIXES: .o .c .cpp .d .hpp
CPPFLAGS	= -D__STDC_LIMIT_MACROS --input-charset=cp932
TARGET		= leaf
OBJS		= $(TARGET).o leaf_type.o leaf_compile.o leaf_node.o
DEPENDDIR	= ./depend
CC = g++

VPATH = ./depend

.c.d:
	$(CC) $(CDBFLG) $(CPPFLAGS) $(MACHINE) $(ENVIRON) -MM $< > $(DEPENDDIR)/$*.d.t
	cp $(DEPENDDIR)/$*.d.t $(DEPENDDIR)/$*.d
	sed -e 's/$*\.o/$*\.d/g' $(DEPENDDIR)/$*.d.t >> $(DEPENDDIR)/$*.d
	rm $(DEPENDDIR)/$*.d.t
	echo '$*.o : $*.d' >> $(DEPENDDIR)/$*.d	

.cpp.d:
	$(CC) $(CDBFLG) $(CPPFLAGS) $(MACHINE) $(ENVIRON) -MM $< > $(DEPENDDIR)/$*.d.t
	cp $(DEPENDDIR)/$*.d.t $(DEPENDDIR)/$*.d
	sed -e 's/$*\.o/$*\.d/g' $(DEPENDDIR)/$*.d.t >> $(DEPENDDIR)/$*.d
	rm $(DEPENDDIR)/$*.d.t
	echo '$*.o : $*.d' >> $(DEPENDDIR)/$*.d	

.cpp.o:
	g++ $(CPPFLAGS) -g -Wall -c -DLINUX -o $@ $<

.c.o:
	gcc $(CPPFLAGS) -g -Wall -c -DLINUX -o $@ $<

all:
	mkdir -p depend
	make depend
	make $(TARGET)
	make rtl

leaf.d : leaf_grammar.hpp leaf_ast.hpp

leaf_grammar.hpp : leaf_grammar.cpg
		../caper/caper -c++ leaf_grammar.cpg leaf_grammar.hpp

leaf_ast.hpp : leaf_ast.cap
		../capella/capella leaf_ast.cap leaf_ast.hpp

rtl: rtl/leaf_rt.bc

rtl/leaf_rt.bc : rtl/leaf_rt.ll
		llvm-as -f rtl/leaf_rt.ll

depend: $(OBJS:.o=.d)

$(TARGET): $(OBJS) leaf_grammar.hpp leaf_ast.hpp
	$(CC) $(CPPFLAGS) -g -o $@ $(OBJS) -lLLVMCore -lLLVMBitWriter -lLLVMSupport -lLLVMSystem -ldl
clean:
	rm -f $(TARGET) $(OBJS) *.bc
	rm -fr $(DEPENDDIR)
	rm -f leaf_grammar.hpp leaf_ast.hpp
	rm -f rtl/*.bc
	rm -f tsuite/*.ll tsuite/*.bc tsuite/*.result tsuite/*.error

clean_tsuite:
	rm -f tsuite/*.result tsuite/*.ll tsuite/*.bc

test : leaf rtl/leaf_rt.bc
		chmod 755 succtest
		chmod 755 failtest
		./succtest foo0000
		./succtest foo0001
		./succtest foo0002
		./succtest foo0003
		./succtest foo0004
		./succtest foo0005
		./succtest foo0006
		./succtest foo0007
		./succtest foo0008
		./succtest foo0009
		./succtest foo0010
#		./succtest foo0011
#		./succtest foo0012
		./failtest bar0000
		./failtest bar0001
		./failtest bar0002
		./failtest bar0003
		./failtest bar0004
		./failtest bar0005

try : leaf rtl/leaf_rt.bc
		chmod 755 succtest
		./succtest foo0013

-include $(addprefix $(DEPENDDIR)/,$(OBJS:.o=.d))
