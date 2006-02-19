

#############################################################################
# Module:           Lexer
# Module Desc:      Lexer and associated files
# Project:          Pathan, Open Source X-Path module for Xerces C++
# Project Version:  1
# tmake Template:   pathanSubdir
# Creation Date:    14:44, 2001/09/28
#############################################################################

###### Include macro definitions

MAKEDEFS = ../../Makefile.defs
include $(MAKEDEFS)

VPATH = $(srcdir)/objs

###### Files

SOURCES     = LexerDOMStringStream.cpp \
		XPath2Lexer.cpp

XPLEXERFILE   = XPath2Lexer.cpp
XPLEXTEMPLATE   = XPath2Lexer.lex

###### Build 

objects: $(PARSERCPP) ${SOURCES:.cpp=.lo}

$(XPLEXERFILE) : $(XPLEXTEMPLATE)
#	${PERL} SyncLexer.pl XPath2Lexer.lex
	$(FLEX) -BUs -Pxx XPath2Lexer.lex
	${PERL} -pi -e 's/istream/LexerDOMStringStream/g' XPath2Lexer.cpp
	${PERL} -pi -e "s/yyin = \&cin/{\/\* Don't ever forget to declare yyin, or bad things will happen \*\/}/g" XPath2Lexer.cpp 
	${PERL} -pi -e "s/\#include \<FlexLexer\.h\>/\#include \"XPath2Lexer\.hpp\"/g" XPath2Lexer.cpp
	${PERL} -pi -e "s/\#include \<unistd\.h\>//g" XPath2Lexer.cpp
	${PERL} -pi -e "s/yyout->write\(\(unsigned char \*\)  buf/yyout->write\(\(const char \*\)buf/g" XPath2Lexer.cpp
	${PERL} -pi -e "s/delete yy_state_buf;/delete yy_state_buf; yy_flex_free\(yy_start_stack\);/g" XPath2Lexer.cpp

lexparseTest: all testLexerParser.o 
	$(RM) $(srcdir)/objs/testLexerParser.o
	$(CXX) $(CXXFLAGS) -ggdb $(INCPATH) $(LIBPATH) testLexerParser.cpp $(srcdir)/objs/*.o -o $@ $(XERCESLIB) $(LIBS)

lexTest: 
	$(RM) $(srcdir)/objs/testlexer.o
	$(CXX) $(CXXFLAGS) -ggdb $(DEFINES) $(INCPATH) -DXPATH_LEXER_DEBUG -c XPath2Lexer.cpp -o $(srcdir)/objs/XPath2Lexer.o
	$(CXX) $(CXXFLAGS) -ggdb $(DEFINES) $(INCPATH) $(LIBPATH) testlexer.cpp $(srcdir)/objs/*.o -o $@ $(XERCESLIB) $(LIBS)

all: objects

lexparseclean:
	$(RM) $(XPLEXERFILE)


###### Include dependences

-include $(patsubst %.cpp,$(srcdir)/objs/%.d,$(filter %.cpp,$(SOURCES)))

###### Include Implicit Rules

MAKEIMPL = ../../Makefile.implicit
include $(MAKEIMPL)
