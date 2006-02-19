

#############################################################################
# Module:           Parser
# Module Desc:      Parser and associated files
# Project:          Pathan, Open Source X-Path module for Xerces C++
# Project Version:  1
# tmake Template:   pathanSubdir
# Creation Date:    14:44, 2001/09/28
#############################################################################

###### Include macro definitions

MAKEDEFS = ../../Makefile.defs
include $(MAKEDEFS)

###### Files

SOURCES     = XPath2Parser.cpp \
		QName.cpp \
		VariableBinding.cpp

HEADERS     = XPathParserControl.hpp
HEADERCOPIES = ../../include/XPathParserControl.hpp

XPPARSERFILE  = XPath2Parser.y
XPPARSERCPP   = XPath2Parser.cpp
XPPARSERHPP   = XPath2Parser.hpp
XPPARSERCPPOUT = XPath2Parser.output

###### Build 

objects: $(PARSERCPP) ${SOURCES:.cpp=.lo}

$(XPPARSERCPP) $(XPPARSERHPP): $(XPPARSERFILE)
	$(BISON) -d -v -p XPathyy  -o $(XPPARSERCPP) $(XPPARSERFILE)
	${PERL} -pi -e "s/yyparse\(YYPARSE_PARAM_ARG\)/PATHAN_EXPORT yyparse(YYPARSE_PARAM_ARG)/g" $(XPPARSERCPP)
	${PERL} -pi -e "s/int yydebug/int PATHAN_EXPORT yydebug/g" $(XPPARSERCPP)

all: objects

lexparseclean:
	$(RM) $(XPPARSERCPP) $(XPPARSERHPP) $(XPPARSERCPPOUT)

###### Include dependences

-include $(patsubst %.cpp,$(srcdir)/objs/%.d,$(filter %.cpp,$(SOURCES)))

###### Include Implicit Rules

MAKEIMPL = ../../Makefile.implicit
include $(MAKEIMPL)
