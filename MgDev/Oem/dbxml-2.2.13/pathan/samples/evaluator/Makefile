#############################################################################
# Module:           xgrep
# Module Desc:      The X-Grep command line xml grep utility
# Project:          xgrep
# Project Version:  1
# Creation Date:    17:19, 2001/05/25
#############################################################################

###### Include macro definitions for samples

MAKEDEFS = ../../Makefile.defs

include $(MAKEDEFS)

XGREPLIBS = ../../lib/libpathan.la $(XERCESLIB) -ldl

# Include only the public headers
EVALINCPATH = $(XERCESINCPATH) -I../../include

###### Files

SOURCES = eval.cpp ../common/DOMTreeErrorReporter.cpp ../common/XStr.cpp

###### Build 

eval: $(SOURCES) ../../lib/libpathan.la
	$(LIBTOOL) --mode=link $(CXX) -ggdb -o eval $(SOURCES) $(EVALINCPATH) $(LIBPATH) $(XGREPLIBS) $(LIBS) $(LINKFLAGS)

all: eval

clean:
	$(RM) *.o core eval

###### Implicit Rules

%.o: %.cpp
	$(CXX) -c $(DEFINES) -g $(CXXFLAGS) $(EVALINCPATH) -o $@ $<
%.o: %.c
	$(CC) -c $(DEFINES) -g $(CFLAGS) $(EVALINCPATH) -o $@ $<
