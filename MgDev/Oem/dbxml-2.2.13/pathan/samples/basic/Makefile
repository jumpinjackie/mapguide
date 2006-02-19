#############################################################################
# Module:           BasicPathanUsage
# Module Desc:      Basic usage example code
# Project:          
# Project Version:  1
# Creation Date:    17:19, 2001/05/25
#############################################################################

###### Include macro definitions for samples

MAKEDEFS = ../../Makefile.defs

include $(MAKEDEFS)

BASICLIBS = ../../lib/libpathan.la $(XERCESLIB) -ldl

# Include only the public headers
BASICINCPATH = $(XERCESINCPATH) -I../../include

###### Files

SOURCES = basicPathanUsage.cpp ../common/XStr.cpp

###### Build 

eval: $(SOURCES) ../../lib/libpathan.la
	$(LIBTOOL) --mode=link $(CXX) -ggdb -o basicPathanUsage $(SOURCES) $(BASICINCPATH) $(LIBPATH) $(BASICLIBS) $(LIBS) $(LINKFLAGS)

all: eval

clean:
	$(RM) *.o core basicPathanUsage

###### Implicit Rules

%.o: %.cpp
	$(CXX) -c $(DEFINES) -g $(CXXFLAGS) $(BASICINCPATH) -o $@ $<
%.o: %.c
	$(CC) -c $(DEFINES) -g $(CFLAGS) $(BASICINCPATH) -o $@ $<
