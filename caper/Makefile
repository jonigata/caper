.SUFFIXES: .o .c .cpp .d
#CC		= g++
#CPPFLAGS	= -O9 --input-charset=cp932 -std=c++11 -pg -fpermissive
CC		= clang++
CPPFLAGS	= -O3 -std=c++11
TARGET		= caper
OBJS		= $(TARGET).o caper_cpg.o caper_tgt.o caper_generate_cpp.o caper_generate_d.o \
	caper_generate_csharp.o caper_generate_js.o caper_generate_java.o caper_generate_boo.o \
	caper_generate_ruby.o caper_generate_php.o caper_generate_haxe.o caper_stencil.o
#TARGET		= grammar_test
#OBJS		= grammar_test.o
DEPENDDIR	= ./depend

VPATH = ./depend

.c.d:
	@$(CC) $(CDBFLG) $(CPPFLAGS) $(INC) $(MACHINE) $(ENVIRON) -MM $< > $(DEPENDDIR)/$*.d.t
	@cp $(DEPENDDIR)/$*.d.t $(DEPENDDIR)/$*.d
	@sed -e 's/$*\.o/$*\.d/g' $(DEPENDDIR)/$*.d.t >> $(DEPENDDIR)/$*.d
	@rm $(DEPENDDIR)/$*.d.t
	@echo '$*.o : $*.d' >> $(DEPENDDIR)/$*.d	

.cpp.d:
	@$(CC) $(CDBFLG) $(CPPFLAGS) $(INC) $(MACHINE) $(ENVIRON) -MM $< > $(DEPENDDIR)/$*.d.t
	@cp $(DEPENDDIR)/$*.d.t $(DEPENDDIR)/$*.d
	@sed -e 's/$*\.o/$*\.d/g' $(DEPENDDIR)/$*.d.t >> $(DEPENDDIR)/$*.d
	@rm $(DEPENDDIR)/$*.d.t
	@echo '$*.o : $*.d' >> $(DEPENDDIR)/$*.d	

.cpp.o:
	$(CC) -g -Wall -c -DLINUX $(CPPFLAGS) -o $@ $<

.c.o:
	gcc -g -Wall -c -DLINUX $(CPPFLAGS) -o $@ $<

all:
	mkdir -p depend
	$(MAKE) depend -s
	$(MAKE) $(TARGET)

depend: $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ -lboost_system -lboost_filesystem

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(DEPENDDIR)

publish:
	rm -rf /tmp/caper /tmp/caper-*
	svn export . /tmp/caper
	rm -f ~/caper-`date +%Y-%m-%d`.zip
	cd /tmp; zip -r9 /home/naoyuki/caper/caper/site/caper-`date +%Y-%m-%d`.zip caper

test:
	cd examples; make test

-include $(addprefix $(DEPENDDIR)/,$(OBJS:.o=.d))
