# $Id: Makefile,v 1.39 2005/12/08 22:38:13 gmf Exp $

##################################################
# Include macro definitions
##################################################

MAKEDEFS= Makefile.defs
include $(MAKEDEFS)

##################################################
# Object and utility lists.
##################################################
CORE_OBJS= $(DBXML_OBJECTS) $(DBXML_NODESTORE_OBJECTS) $(DBXML_DATAITEM_OBJECTS) $(DBXML_OPTIMIZER_OBJECTS)

JAVA_OBJS=\
	dbxml_java_wrap$(OBJECT_POSTFIX)

JAVA_DBXMLSRCS=\
	$(JAVA_DBXMLDIR)/debug/XmlConstants.java \
	$(JAVA_DBXMLDIR)/XmlQueryExpression.java \
	$(JAVA_DBXMLDIR)/XmlContainer.java \
	$(JAVA_DBXMLDIR)/XmlDocument.java \
	$(JAVA_DBXMLDIR)/XmlException.java \
	$(JAVA_DBXMLDIR)/XmlIndexDeclaration.java \
	$(JAVA_DBXMLDIR)/XmlIndexSpecification.java \
	$(JAVA_DBXMLDIR)/XmlQueryContext.java \
	$(JAVA_DBXMLDIR)/XmlResults.java \
	$(JAVA_DBXMLDIR)/XmlUpdateContext.java \
	$(JAVA_DBXMLDIR)/XmlValue.java \
	$(JAVA_DBXMLDIR)/XmlModify.java \
	$(JAVA_DBXMLDIR)/XmlManager.java \
	$(JAVA_DBXMLDIR)/XmlTransaction.java \
	$(JAVA_DBXMLDIR)/XmlInputStream.java \
	$(JAVA_DBXMLDIR)/XmlIndexLookup.java \
	$(JAVA_DBXMLDIR)/XmlStatistics.java \
	$(JAVA_DBXMLDIR)/XmlMetaDataIterator.java \
	$(JAVA_DBXMLDIR)/XmlMetaData.java \
	$(JAVA_DBXMLDIR)/XmlResolver.java \
	$(JAVA_DBXMLDIR)/XmlManagerConfig.java \
	$(JAVA_DBXMLDIR)/XmlContainerConfig.java \
	$(JAVA_DBXMLDIR)/XmlDocumentConfig.java \
	$(JAVA_DBXMLDIR)/dbxml_java.java \
	$(JAVA_DBXMLDIR)/dbxml_javaConstants.java \
	$(JAVA_DBXMLDIR)/dbxml_javaJNI.java \
	$(JAVA_DBXMLDIR)/NativeXmlInputStream.java \
	$(JAVA_DBXMLDIR)/XmlInputStreamWrap.java \
	$(JAVA_DBDIR)/XmlHelper.java

JAVA_EXSRCS= \
	$(JAVA_EXDIR)/addIndex.java \
	$(JAVA_EXDIR)/buildDB.java \
	$(JAVA_EXDIR)/deleteDocument.java \
	$(JAVA_EXDIR)/deleteIndex.java \
	$(JAVA_EXDIR)/exampleLoadContainer.java \
	$(JAVA_EXDIR)/mdConst.java \
	$(JAVA_EXDIR)/modifyDocument.java \
	$(JAVA_EXDIR)/myDb.java \
	$(JAVA_EXDIR)/myDbEnv.java \
	$(JAVA_EXDIR)/queryForDocumentValue.java \
	$(JAVA_EXDIR)/queryForMetaData.java \
	$(JAVA_EXDIR)/queryWithContext.java \
	$(JAVA_EXDIR)/queryWithDocumentNames.java \
	$(JAVA_EXDIR)/replaceIndex.java \
	$(JAVA_EXDIR)/retrieveDB.java \
	$(JAVA_EXDIR)/simpleAdd.java \
	$(JAVA_EXDIR)/simpleContainerInEnv.java \
	$(JAVA_EXDIR)/simpleQuery.java \
	$(JAVA_EXDIR)/updateDocument.java

JAVA_PERFSRCS=\
	$(JAVA_PERFDIR)/PerfTest.java

TCL_OBJS=\
	dbxml_tcl_wrap$(OBJECT_POSTFIX)

EXAMPLES = exampleLoadContainer addIndex \
	buildDB deleteDocument deleteIndex \
	simpleAdd \
	simpleContainer simpleContainerInEnv \
	simpleQuery queryForDocumentValue \
	queryForMetaData queryWithContext \
	queryWithDocumentNames replaceIndex \
	retrieveDB updateDocument modifyDocument


UTIL_PROGS=	 dbxml_dump dbxml_load query_runner \
	dbxml_load_container dbxml

BENCHMARK_PROGS= dbxml_benchmark

PERF_PROGS= dbxml_perf

UNIT_TESTS=	dbxml_test_driver dbxml_test_databases dbxml_test_query_processor_api dbxml_test_input_streams

##################################################
# List of files installed into the library directory.
##################################################
LIB_INSTALL_FILE_LIST=\
	$(libcxx) \
	$(libxso) \
	$(libxso_default) \
	$(libxso_major) \
	$(libxso_static) \
	$(libxso_target) \
	$(libtso) \
	$(libtso_default) \
	$(libtso_major) \
	$(libtso_static) \
	$(libtso_target) \
	$(libjso) \
	$(libjso_default) \
	$(libjso_g) \
	$(libjso_major) \
	$(libjso_static) \
	$(libjso_target) \
	$(libjso_base) \
	$(libj_exjarfile) \
	$(libj_jarfile)

##################################################
# Note: "all" must be the first target in the Makefile.
##################################################

all: library_build util_progs examples $(build_test)

install-strip install: all library_install

##################################################
# Library and standard utilities build.
##################################################
.PHONY: library_build test_build  examples util_progs unit_tests default_lib test_lib test_progs test benchmark perf

library_build: core_subdirs $(libxso_target) $(build_tcl) $(build_java)

tcl_build: $(libtso_target)

test_build: tcl_build test_lib test_progs

examples: default_lib example_subdir $(EXAMPLES)

util_progs: default_lib common_util_subdir dbxml_dump_subdir dbxml_load_subdir load_container_subdir query_runner_subdir shell_subdir $(UTIL_PROGS)

benchmark: default_lib benchmark_subdir $(BENCHMARK_PROGS)

perf: default_lib perf_subdir $(PERF_PROGS)

shell: default_lib common_util_subdir shell_subdir dbxml

unit_tests: unit_test_util_subdir unit_test_subdir unit_test_database_subdir unit_test_qpapi_subdir unit_test_input_streams_subdir default_lib $(UNIT_TESTS)

default_lib: core_subdirs $(DEF_LIB)

test_lib: core_subdirs tcl_build

test_progs: test_lib default_lib dbxml_dump_subdir dbxml_load_subdir common_util_subdir dbxml_load dbxml_dump unit_tests

test: test_progs
	tclsh ../test/run_tests.tcl

$(libcxx): $(CORE_OBJS)
	$(ar) cr $@ $(CORE_OBJS)
	test ! -f $(ranlib) || $(ranlib) $@

$(libjso_target): $(CORE_OBJS) $(JAVA_OBJS)
	$(XSOLINK) -shrext $(libjso_suffix) $(SOFLAGS) $(LDFLAGS) -o $@ \
	    $(CORE_OBJS) $(JAVA_OBJS) $(LIBJSO_LIBS)
	$(LIBJSO_LN_BUILD)

$(libtso_target): $(CORE_OBJS) $(TCL_OBJS)
	$(XSOLINK) $(LIBTSO_MODULE) $(SOFLAGS) $(LDFLAGS) -o $@ \
		$(CORE_OBJS) $(TCL_OBJS) $(LIBTSO_LIBS)

$(libxso_target): $(CORE_OBJS)
	$(XSOLINK) $(SOFLAGS) $(LDFLAGS) -o $@ \
	    $(CORE_OBJS) $(LIBXSO_LIBS)

##################################################
# Creating individual dependencies and actions for building class
# files is possible, but it is very messy and error prone.
##################################################
java:	core_subdirs $(libjso_target) $(libj_jarfile) $(libj_exjarfile)

$(libj_jarfile): $(JAVA_DBXMLSRCS)
	@test -d $(JAVA_CLASSTOP) || \
	   ($(mkdir) -p $(JAVA_CLASSTOP) && $(chmod) $(dmode) $(JAVA_CLASSTOP))
	$(JAVAC) -d $(JAVA_CLASSTOP) $(JAVACFLAGS) $(JAVA_DBXMLSRCS)
	cd $(JAVA_CLASSTOP) && $(JAR) cf ../$@ $(JAVA_DBXMLREL) $(JAVA_DBREL)

$(libj_exjarfile): $(libj_jarfile) $(JAVA_EXSRCS)
	@test -d $(JAVA_CLASSTOP) || \
	   ($(mkdir) -p $(JAVA_CLASSTOP) && $(chmod) $(dmode) $(JAVA_CLASSTOP))
	$(JAVAC) -d $(JAVA_CLASSTOP) $(JAVACFLAGS) $(JAVA_EXSRCS)
	cd $(JAVA_CLASSTOP) && $(JAR) cf ../$@ $(JAVA_EXREL)

$(libj_perfjarfile): $(libj_jarfile) $(JAVA_PERFSRCS)
	@test -d $(JAVA_CLASSTOP) || \
	   ($(mkdir) -p $(JAVA_CLASSTOP) && $(chmod) $(dmode) $(JAVA_CLASSTOP))
	$(JAVAC) -d $(JAVA_CLASSTOP) $(JAVACFLAGS) $(JAVA_PERFSRCS)
	cd $(JAVA_CLASSTOP) && $(JAR) cf ../$@ $(JAVA_PERFREL)

##################################################
# Examples
##################################################
addIndex: addIndex$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) addIndex$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

buildDB: buildDB$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) buildDB$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

deleteDocument: deleteDocument$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) deleteDocument$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

deleteIndex: deleteIndex$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) deleteIndex$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

exampleLoadContainer: exampleLoadContainer$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) exampleLoadContainer$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

simpleAdd: simpleAdd$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) simpleAdd$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

simpleContainer: simpleContainer$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) simpleContainer$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

simpleContainerInEnv: simpleContainerInEnv$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) simpleContainerInEnv$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

simpleQuery: simpleQuery$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) simpleQuery$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

queryForDocumentValue: queryForDocumentValue$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) queryForDocumentValue$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

queryForMetaData: queryForMetaData$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) queryForMetaData$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

queryWithContext: queryWithContext$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) queryWithContext$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

queryWithDocumentNames: queryWithDocumentNames$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) queryWithDocumentNames$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

replaceIndex: replaceIndex$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) replaceIndex$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

retrieveDB: retrieveDB$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) retrieveDB$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

updateDocument: updateDocument$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) updateDocument$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

modifyDocument: modifyDocument$(OBJECT_POSTFIX) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) modifyDocument$(OBJECT_POSTFIX) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

##################################################
# Unit tests
##################################################
dbxml_test_driver: $(UNIT_TEST_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(UNIT_TEST_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

dbxml_test_databases: databaseManagement$(OBJECT_POSTFIX) $(UNIT_TEST_DATABASE_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(UNIT_TEST_DATABASE_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

dbxml_test_query_processor_api: queryProcessorAPI$(OBJECT_POSTFIX) $(UNIT_TEST_QPAPI_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(UNIT_TEST_QPAPI_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

dbxml_test_input_streams: inputStreams$(OBJECT_POSTFIX) $(UNIT_TEST_INPUTSTREAMS_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(UNIT_TEST_INPUTSTREAMS_OBJECTS) $(UNIT_TEST_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@


##################################################
# Library and standard utilities install.
##################################################
library_install: install_setup
library_install: install_include install_lib install_utilities install_docs

uninstall: uninstall_include uninstall_lib uninstall_utilities uninstall_docs

install_setup:
	@test -d $(prefix) || \
	    ($(mkdir) -p $(prefix) && $(chmod) $(dmode) $(prefix))

INCINC= DbXml.hpp DbXmlFwd.hpp XmlQueryExpression.hpp XmlContainer.hpp \
	XmlDocument.hpp XmlException.hpp XmlIndexSpecification.hpp \
	XmlNamespace.hpp XmlPortability.hpp XmlQueryContext.hpp \
	XmlResults.hpp XmlUpdateContext.hpp XmlValue.hpp XmlResolver.hpp \
	XmlModify.hpp XmlManager.hpp XmlData.hpp XmlTransaction.hpp \
	XmlInputStream.hpp XmlMetaDataIterator.hpp XmlStatistics.hpp \
	XmlIndexLookup.hpp

install_include:
	@echo "Installing DB XML include files: $(includedir) ..."
	@test -d $(includedir) || \
	    ($(mkdir) -p $(includedir) && $(chmod) $(dmode) $(includedir))
	@cd $(srcdir)/include/dbxml/ && $(cp) -pf $(INCINC) $(includedir)
	@cd $(includedir) && $(chmod) $(fmode) $(INCINC)

uninstall_include:
	@cd $(includedir) && $(rm) -f $(INCINC)

install_lib:
	@echo "Installing DB XML library: $(libdir) ..."
	@test -d $(libdir) || \
	    ($(mkdir) -p $(libdir) && $(chmod) $(dmode) $(libdir))
	@cd $(libdir) && $(rm) -f $(LIB_INSTALL_FILE_LIST)
	@$(INSTALLER) $(libxso_target) $(install_tcl) $(install_java) $(libdir)
	@(cd $(libdir) && \
	    test -f $(libxso) && $(ln) -s $(libxso) $(libxso_default); \
	    test -f $(libxso) && $(ln) -s $(libxso) $(libxso_major); \
	    test -f $(libxso_static) && $(ln) -s $(libxso_static) $(libcxx); \
	    test -f $(libtso) && $(ln) -s $(libtso) $(libtso_default); \
	    test -f $(libtso) && $(ln) -s $(libtso) $(libtso_major); \
	    test -f $(libjso) && $(ln) -s $(libjso) $(libjso_default); \
	    test -f $(libjso) && $(ln) -s $(libjso) $(libjso_major); \
	    test -f $(libjso) && $(ln) -s $(libjso) $(libjso_g); \
	    $(LIBJSO_LN_INSTALL)) || true
	@(test -f $(libj_jarfile) && \
	    $(cp) $(libj_jarfile) $(libdir) && \
	    $(chmod) $(fmode) $(libdir)/$(libj_jarfile)) || true

uninstall_lib:
	@cd $(libdir) && $(rm) -f $(LIB_INSTALL_FILE_LIST)

install_utilities:
	echo "Installing DB XML utilities: $(bindir) ..."
	@test -d $(bindir) || \
	    ($(mkdir) -p $(bindir) && $(chmod) $(dmode) $(bindir))
	@for i in $(UTIL_PROGS); do \
		$(rm) -f $(bindir)/$$i $(bindir)/$$i.exe; \
		test -f $$i.exe && i=$$i.exe || true; \
		$(INSTALLER) $$i $(bindir)/$$i; \
		test -f $(strip) && $(strip) $(bindir)/$$i || true; \
		$(chmod) $(emode) $(bindir)/$$i; \
	done

uninstall_utilities:
	@(cd $(bindir); for i in $(UTIL_PROGS); do \
		$(rm) -f $$i $$i.exe; \
	done)

DOCLIST=\
	api_c api_cxx api_tcl collections gsg gsg_xml images index.html \
	java ref ref_xml sleepycat utility
install_docs:
	@echo "Installing documentation: $(docdir) ..."
	@test -d $(docdir) || \
	    ($(mkdir) -p $(docdir) && $(chmod) $(dmode) $(docdir))
	@cd $(docdir) && $(rm) -rf $(DOCLIST)
	@cd $(srcdir)/docs && $(cp) -pr $(DOCLIST) $(docdir)/

uninstall_docs:
	@cd $(docdir) && $(rm) -rf $(DOCLIST)

##################################################
# Remaining standard Makefile targets.
##################################################
CLEAN_LIST=$(EXAMPLES) $(UTIL_PROGS) $(UNIT_TESTS) $(BENCHMARK_PROGS) $(PERF_PROGS)

mostly-clean clean:
	$(rm) -rf $(CORE_OBJS)
	$(rm) -rf ${CORE_OBJS:%$(OBJECT_POSTFIX)=%.d}
	$(rm) -rf $(JAVA_OBJS) $(TCL_OBJS)
	$(rm) -rf *.exe $(CLEAN_LIST)
	$(rm) -rf $(JAVA_CLASSTOP)
	$(rm) -rf tags *.lo *.o .libs/*.o *.o.lock *.d core *.core test_*
	$(rm) -rf ALL.OUT.* TESTDIR .libs $(LIB_INSTALL_FILE_LIST)

REALCLEAN_LIST=\
	Makefile.defs config.cache config.log config.status configure.lineno \
	dbxml_config.h include.tcl

distclean maintainer-clean realclean: clean
	$(rm) -rf $(REALCLEAN_LIST)
	$(rm) -rf libtool

check depend dvi info obj TAGS:
	@echo "$@: make target not supported" && true

dist:
	@echo "$@: make target not supported" && false


##################################################
# Sub-directory build rules
##################################################
SUBDIRS_TARGETS= dbxml_subdir dbxml_nodestore_subdir \
	dbxml_dataitem_subdir dbxml_optimizer_subdir \
	example_subdir unit_test_subdir

.PHONY: core_subdirs subdirs $(SUBDIRS_TARGETS)

core_subdirs: dbxml_subdir dbxml_nodestore_subdir \
	dbxml_dataitem_subdir dbxml_optimizer_subdir

subdirs: $(SUBDIRS_TARGETS)

dbxml_subdir:
	@$(MAKE) -C $(DBXML_PATH)

dbxml_nodestore_subdir:
	@$(MAKE) -C $(DBXML_NODESTORE_PATH)

dbxml_dataitem_subdir:
	@$(MAKE) -C $(DBXML_DATAITEM_PATH)

dbxml_optimizer_subdir:
	@$(MAKE) -C $(DBXML_OPTIMIZER_PATH)

example_subdir:
	@$(MAKE) -C $(EXAMPLE_PATH)

common_util_subdir:
	@$(MAKE) -C $(COMMON_UTIL_PATH)

dbxml_dump_subdir:
	@$(MAKE) -C $(DBXML_DUMP_PATH)

dbxml_load_subdir:
	@$(MAKE) -C $(DBXML_LOAD_PATH)

shell_subdir:
	@$(MAKE) -C $(SHELL_PATH)

query_runner_subdir:
	@$(MAKE) -C $(QUERY_RUNNER_PATH)

load_container_subdir:
	@$(MAKE) -C $(LOAD_CONTAINER_PATH)

unit_test_util_subdir:
	@$(MAKE) -C $(UNIT_TEST_UTIL_PATH)

unit_test_subdir:
	@$(MAKE) -C $(UNIT_TEST_PATH)

unit_test_database_subdir:
	@$(MAKE) -C $(UNIT_TEST_DATABASE_PATH)

unit_test_qpapi_subdir:
	@$(MAKE) -C $(UNIT_TEST_QPAPI_PATH)

unit_test_input_streams_subdir:
	@$(MAKE) -C $(UNIT_TEST_INPUTSTREAMS_PATH)

benchmark_subdir:
	@$(MAKE) -C $(BENCHMARK_PATH)

perf_subdir:
	@$(MAKE) -C $(PERF_PATH)

##################################################
# Java build rules.
##################################################
dbxml_java_wrap$(OBJECT_POSTFIX) : $(srcdir)/src/java/dbxml_java_wrap.cpp
	$(CXX) $(CXXFLAGS) $?

##################################################
# Tcl build rules.
##################################################
dbxml_tcl_wrap$(OBJECT_POSTFIX): $(srcdir)/src/tcl/dbxml_tcl_wrap.cpp
	$(CXX) $(CXXFLAGS) $(TCL_INCLUDE_SPEC) $?

##################################################
# Utility build rules.
##################################################
dbxml_dump: $(DBXML_DUMP_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(DBXML_DUMP_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@
dbxml_load: $(DBXML_LOAD_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(DBXML_LOAD_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@
dbxml_load_container: $(LOAD_CONTAINER_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(LOAD_CONTAINER_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@
query_runner: $(QUERY_RUNNER_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(QUERY_RUNNER_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@
dbxml: $(SHELL_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(SHELL_OBJECTS) $(COMMON_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

##################################################
# Benchmark build rules.
##################################################
dbxml_benchmark: $(BENCHMARK_OBJECTS) $(DEF_LIB)  $(COMMON_UTIL_OBJECTS)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(BENCHMARK_OBJECTS)  $(COMMON_UTIL_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

##################################################
# Perf build rules.
##################################################
dbxml_perf: $(PERF_OBJECTS) $(DEF_LIB)
	@$(CXXLINK) -o $@ $(LDFLAGS) $(PERF_OBJECTS) $(DEF_LIB) $(LIBS)
	@$(POSTLINK) $@

##################################################
# Makefile rules for rebuilding Makefile.defs
##################################################
Makefile.defs: ../dist/Makefile.in config.status
	@./config.status

config.status: ../dist/configure
	@./config.status --recheck || (echo You need to run the ../dist/configure script && false)

CONFIGURE_LIST=\
	../dist/configure.ac ../dist/aclocal/config.ac ../dist/aclocal/libtool.ac ../dist/aclocal/options.ac \
	../dist/aclocal/programs.ac ../dist/aclocal/sosuffix.ac ../dist/aclocal/tcl.ac ../dist/s_config

../dist/configure: $(CONFIGURE_LIST)
	@cd ../dist/ && ./s_config

##################################################
# Default rule
##################################################
.DEFAULT:
	@echo Can\'t make $@

