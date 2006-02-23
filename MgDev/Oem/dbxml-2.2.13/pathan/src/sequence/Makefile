

#############################################################################
# Module:           Sequence
# Module Desc:      Sequence Module
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

SOURCES     = Sequence.cpp

###### Build 

objects: ${SOURCES:.cpp=.lo}

all: objects

###### Include dependences

-include $(patsubst %.cpp,$(srcdir)/objs/%.d,$(filter %.cpp,$(SOURCES)))

###### Include Implicit Rules

MAKEIMPL = ../../Makefile.implicit
include $(MAKEIMPL)
