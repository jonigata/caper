.SUFFIXES: .o .c .cpp .d
CC		= g++
CPPFLAGS	= -O3 --input-charset=cp932
TARGET		= caper
OBJS		= $(TARGET).o caper_cpg.o caper_tgt.o caper_generate_cpp.o caper_generate_d.o caper_generate_csharp.o caper_generate_js.o
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
	g++ -g -Wall -c -DLINUX $(CPPFLAGS) -o $@ $<

.c.o:
	gcc -g -Wall -c -DLINUX $(CPPFLAGS) -o $@ $<

all:
	mkdir -p depend
	make depend -s
	make $(TARGET)

depend: $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ -lboost_filesystem

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(DEPENDDIR)

publish:
	rm -rf /tmp/caper /tmp/caper-*
	svn export . /tmp/caper
	rm -f ~/caper-`date +%Y-%m-%d`.zip
	cd /tmp; zip -r9 /home/naoyuki/caper/caper/site/caper-`date +%Y-%m-%d`.zip caper

-include $(addprefix $(DEPENDDIR)/,$(OBJS:.o=.d))
