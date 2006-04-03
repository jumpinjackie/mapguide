# $Id: Makefile,v 1.4 2004/08/19 14:00:14 jsnelson Exp $

###### Include macro definitions

MAKEDEFS = ../build_unix/Makefile.defs
include $(MAKEDEFS)

###### Files

SOURCES= $(XQUERY_SOURCES)

###### Include rules

RULES = ../build_unix/Makefile.rules
include $(RULES)

###### Parser and Lexer rules

Grammar.cpp Grammar.hpp: xquery.y
	$(bison) -d -v -o Grammar.cpp xquery.y

Scanner.cpp: xquery.l flex.skl
	$(flex) -BUs -Sflex.skl xquery.l # This can take a long time
