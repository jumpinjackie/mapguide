#############################################################################
# Project:          Pathan, Open Source X-Path module for Xerces C++
# Project Version:  1
# tmake Template:   pathanMaindir
# Creation Date:    14:44, 2001/09/28
#############################################################################

###### Include macro definitions

MAKEDEFS    = Makefile.defs
include $(MAKEDEFS)

###### Files

TARGET      = pathan
LIBNAME     = $(TARGET)
KERNEL_OBJS  = $(srcdir)/objs/*.lo

VPATH = $(srcdir)/objs

LIBTOOL_VERSION=3:1:0

.PHONY: samples

###### Build 

$(TARGET): subdirs localinstall

all: $(TARGET) 

clean: localuninstall
	$(RM) $(srcdir)/core
	$(RM) $(srcdir)/objs/*.o $(srcdir)/objs/*.lo $(srcdir)/objs/*.d $(srcdir)/objs/Pathan.hpp

distclean: clean
	$(RM) $(srcdir)/pathan/pathan_config_unix.h
	$(RM) $(srcdir)/libpathan.la
	$(RM) -r $(srcdir)/.libs/ $(srcdir)/objs/.libs
	$(RM) $(srcdir)/lib/*.a $(srcdir)/lib/*.la $(srcdir)/*.so*
	$(RM) Makefile.defs config.status config.cache config.log libtool

completeclean: lexparseclean distclean

lexparseclean:
	$(MAKE) -C $(srcdir)/src/parser lexparseclean
	$(MAKE) -C $(srcdir)/src/lexer lexparseclean

localinstall: subdirs
	@$(LIBTOOL) --mode=link $(CXX) -o $(srclibdir)/libpathan.la -rpath $(libdir) $(KERNEL_OBJS) -version-info $(LIBTOOL_VERSION)

localuninstall:
	@$(LIBTOOL) --mode=uninstall rm -f $(srclibdir)/libpathan.la

install:
	@for dir in `find include -type d | grep -v 'CVS' | grep -v '.svn'`; do $(srcdir)/util/mkinstalldirs $(incdir)/../$$dir ; $(LIBTOOL) --mode=install $(INSTALL) $(srcincdir)/../$$dir/*.h* $(incdir)/../$$dir 2>/dev/null >/dev/null; done
	$(srcdir)/util/mkinstalldirs $(libdir)
	@$(LIBTOOL) --mode=install $(INSTALL) $(srclibdir)/libpathan.la $(libdir) 2> /dev/null
	@$(LIBTOOL) --mode=finish $(libdir)

uninstall:
	$(RM) -r $(incdir)/pathan
	$(LIBTOOL) --mode=uninstall rm -f $(libdir)/libpathan.la

subdirs:
	@$(MKDIR) $(srcdir)/objs 2> /dev/null
	@$(MKDIR) $(srcdir)/lib 2> /dev/null
	@$(MAKE) -C $(srcdir)/src/parser objects
	@$(MAKE) -C $(srcdir)/src/DOMutils objects
	@$(MAKE) -C $(srcdir)/src/sequence objects
	@$(MAKE) -C $(srcdir)/src/dataItem objects
	@$(MAKE) -C $(srcdir)/src/factory objects
	@$(MAKE) -C $(srcdir)/src/functionAPI objects
	@$(MAKE) -C $(srcdir)/src/operators objects
	@$(MAKE) -C $(srcdir)/src/lexer objects
	@$(MAKE) -C $(srcdir)/src/navigation objects
	@$(MAKE) -C $(srcdir)/src/simpleVariables objects
	@$(MAKE) -C $(srcdir)/src/utils objects
	@$(MAKE) -C $(srcdir)/src/context objects
	@$(MAKE) -C $(srcdir)/src/exceptions objects
	@$(MAKE) -C $(srcdir)/src/memoryManager objects
	@$(MAKE) -C $(srcdir)/src/schema objects
	@$(MAKE) -C $(srcdir)/src/collations objects
	@$(MAKE) -C $(srcdir)/src/mapm objects
	@$(MAKE) -C $(srcdir)/src/functions objects
	@$(MAKE) -C $(srcdir)/src/functions/dateTimeFunctions objects
	@$(MAKE) -C $(srcdir)/src/items/ objects
	@$(MAKE) -C $(srcdir)/src/items/impl/ objects
	@$(MAKE) -C $(srcdir)/src/validators/ objects
	@$(MAKE) -C $(srcdir)/src/dom-extensions/ objects
	@$(MAKE) -C $(srcdir)/src/dom-extensions/impl objects


samples:
	@$(MAKE) -C $(srcdir)/samples


###### Implicit Rules

%.o: %.cpp
	@$(LIBTOOL) --mode=compile $(CXX) -c $(DEFINES) $(CXXFLAGS) $(INCPATH) -o $@ $<

%.o: %.c
	@$(LIBTOOL) --mode=compile $(CC) -c $(DEFINES) $(CFLAGS) $(INCPATH) -o $@ $<

###### Makefile rules for rebuilding Makefile.defs

Makefile.defs: Makefile.defs.in config.status
	@./config.status

config.status: configure
	@./config.status --recheck || (echo You need to run the "configure" or "runConfigure" script && false)

configure: configure.in
	@autoconf
