#############################################################################
# Module:           xgrep
# Module Desc:      The X-Grep command line xml grep utility
# Project:          xgrep
# Project Version:  1
# Creation Date:    17:19, 2001/05/25
#############################################################################

###### Include macro definitions for examples

MAKEDEFS = ../../Makefile.defs

include $(MAKEDEFS)

XGREPLIBS = ../../lib/libpathan.la $(XERCESLIB) -ldl

# Include only the public headers
XGREPINCPATH = $(XERCESINCPATH) -I../../include 

###### Files

SOURCES = xgrep.cpp ../common/DOMTreeErrorReporter.cpp ../common/XStr.cpp

###### Build 

xgrep: $(SOURCES) ../../lib/libpathan.la
	$(LIBTOOL) --mode=link $(CXX) -ggdb -o xgrep $(SOURCES) $(XGREPINCPATH) $(LIBPATH) $(XGREPLIBS) $(LIBS) $(LINKFLAGS)

all: xgrep

clean:
	$(RM) *.o core xgrep

###### Implicit Rules

%.o: %.cpp
	$(CXX) -c $(DEFINES) -g $(CXXFLAGS) $(XGREPINCPATH) -o $@ $<
%.o: %.c
	$(CC) -c $(DEFINES) -g $(CFLAGS) $(XGREPINCPATH) -o $@ $<
