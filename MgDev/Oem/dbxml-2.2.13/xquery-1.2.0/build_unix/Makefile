# $Id: Makefile,v 1.8 2005/04/20 18:31:48 bostic Exp $

##################################################
# Include macro definitions
##################################################

MAKEDEFS= Makefile.defs
include $(MAKEDEFS)

##################################################
# Object and utility lists.
##################################################
CORE_OBJS= $(XQUERY_OBJECTS)

EXAMPLES = eval

##################################################
# List of files installed into the library directory.
##################################################
LIB_INSTALL_FILE_LIST=\
	$(libcxx) \
	$(libxso) \
	$(libxso_default) \
	$(libxso_major) \
	$(libxso_static) \
	$(libxso_target)

##################################################
# Note: "all" must be the first target in the Makefile.
##################################################
all: default_lib examples

install-strip install: all library_install

##################################################
# Library and standard utilities build.
##################################################
.PHONY: library_build examples shell tester util_progs default_lib test_lib

library_build: core_subdirs $(libxso_target) examples

examples: eval_subdir default_lib $(EXAMPLES)

default_lib: core_subdirs $(DEF_LIB)

$(libcxx): $(CORE_OBJS)
	$(ar) cr $@ $(CORE_OBJS)
	test ! -f $(ranlib) || $(ranlib) $@

$(libxso_target): $(CORE_OBJS)
	$(XSOLINK) $(SOFLAGS) $(LDFLAGS) -o $@ \
	    $(CORE_OBJS) $(LIBXSO_LIBS)

##################################################
# Examples
##################################################
eval: $(EVAL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(EVAL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

##################################################
# Library and standard utilities install.
##################################################
library_install: install_setup
library_install: install_include install_lib install_docs

uninstall: uninstall_include uninstall_lib uninstall_docs

install_setup:
	@test -d $(prefix) || \
	    ($(mkdir) -p $(prefix) && $(chmod) $(dmode) $(prefix))

INC_DIRS = xquery/ xquery/dataItem/ xquery/functions/ xquery/dom-extensions/ xquery/context/ \
	xquery/utils/

install_include:
	@echo "Installing XQuery include files: $(includedir) ..."
	@test -d $(includedir) || \
	    ($(mkdir) -p $(includedir) && $(chmod) $(dmode) $(includedir))
	@for name in $(INC_DIRS); do \
		test -d $(includedir)/$$name || \
	    ($(mkdir) -p $(includedir)/$$name && $(chmod) $(dmode) $(includedir)/$$name); \
		cd $(srcdir)/include/$$name && $(cp) -pf *.hpp $(includedir)/$$name; \
		cd $(includedir) && ($(chmod) $(fmode) $$name/*.hpp) && \
			$(chmod) $(dmode) $$name/; \
		done

uninstall_include:
	@cd $(includedir) && $(rm) -rf xquery/

install_lib:
	@echo "Installing XQuery library: $(libdir) ..."
	@test -d $(libdir) || \
	    ($(mkdir) -p $(libdir) && $(chmod) $(dmode) $(libdir))
	@cd $(libdir) && $(rm) -f $(LIB_INSTALL_FILE_LIST)
	@$(INSTALLER) $(libxso_target) $(libdir)
	@(cd $(libdir) && \
	    test -f $(libxso) && $(ln) -s $(libxso) $(libxso_default); \
	    test -f $(libxso) && $(ln) -s $(libxso) $(libxso_major); \
	    test -f $(libxso_static) && $(ln) -s $(libxso_static) $(libcxx); \
	    ) || true

uninstall_lib:
	@cd $(libdir) && $(rm) -f $(LIB_INSTALL_FILE_LIST)

DOCLIST=\
	api_c api_cxx api_tcl images index.html java javaxml pdf_xml ref \
	ref_xml sleepycat utility
install_docs:
	@#echo "Installing documentation: $(docdir) ..."
	@#test -d $(docdir) || ($(mkdir) -p $(docdir) && $(chmod) $(dmode) $(docdir))
	@#cd $(docdir) && $(rm) -rf $(DOCLIST)
	@#cd $(srcdir)/docs && $(cp) -pr $(DOCLIST) $(docdir)/

uninstall_docs:
	@#cd $(docdir) && $(rm) -rf $(DOCLIST)

##################################################
# Remaining standard Makefile targets.
##################################################
CLEAN_LIST=$(EXAMPLES)

mostly-clean clean:
	$(rm) -rf $(CORE_OBJS)
	$(rm) -rf ${CORE_OBJS:%$(OBJECT_POSTFIX)=%.d}
	$(rm) -rf *.exe $(CLEAN_LIST)
	$(rm) -rf tags *.lo *.o *.o.lock *.d core *.core test_*
	$(rm) -rf ALL.OUT.* TESTDIR .libs $(LIB_INSTALL_FILE_LIST)

REALCLEAN_LIST=\
	Makefile.defs config.cache config.log config.status configure.lineno \
	xquery_config.h libtool include.tcl

distclean maintainer-clean realclean: clean
	$(rm) -rf $(REALCLEAN_LIST)

check depend dvi info obj TAGS:
	@echo "$@: make target not supported" && true

dist:
	@echo "$@: make target not supported" && false

##################################################
# Sub-directory build rules
##################################################
SUBDIRS_TARGETS= xquery_subdir eval_subdir

.PHONY: core_subdirs subdirs $(SUBDIRS_TARGETS)

core_subdirs: xquery_subdir

subdirs: $(SUBDIRS_TARGETS)

xquery_subdir:
	@$(MAKE) -C $(XQUERY_PATH)

eval_subdir:
	@$(MAKE) -C $(EVAL_PATH)

##################################################
# Makefile rules for rebuilding Makefile.defs
##################################################
Makefile.defs: ../dist/Makefile.in config.status
	@./config.status

config.status: ../dist/configure
	@./config.status --recheck || (echo You need to run the ../dist/configure script && false)

CONFIGURE_LIST=\
	../dist/configure.ac ../dist/aclocal/config.ac ../dist/aclocal/libtool.ac ../dist/aclocal/options.ac \
	../dist/aclocal/programs.ac ../dist/aclocal/sosuffix.ac ../dist/s_config

../dist/configure: $(CONFIGURE_LIST)
	@cd ../dist/ && ./s_config

##################################################
# Default rule
##################################################
.DEFAULT:
	@echo Can\'t make $@

