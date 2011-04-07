# DEBUG = y
MAKERULESDIR = ..
DESTDIR = ../bin.$(UNAME)
INCLUDES = -I../include -Iinclude
LIBDIR = -L../lib.$(UNAME) -Llib.$(UNAME)

include $(MAKERULESDIR)/Makerules

OPTSOURCES =	mesh_opt.cc

OPTFILES = $(addprefix $(OBJDIR)/,$(OPTSOURCES:.cc=.o))
OFILES = $(OPTFILES) 

OPTPROG = $(addsuffix $(EXE), $(addprefix $(DESTDIR)/, $(OPTSOURCES:.cc=)))
PROGS = $(OPTPROG)

default: $(PROGS)


LIBS += -ltrimesh $(CHOLMODLIBS)


$(OPTPROG) : $(DESTDIR)/%$(EXE) : $(OBJDIR)/%.o
	$(LINK)

$(PROGS) : ../lib.$(UNAME)/libtrimesh.a

clean :
	-rm -f $(OFILES) $(OBJDIR)/Makedepend $(OBJDIR)/*.d
	-rm -rf $(OBJDIR)/ii_files
	-rmdir $(OBJDIR)

spotless : clean
	-rm -f $(PROGS)
	-rmdir $(DESTDIR)

