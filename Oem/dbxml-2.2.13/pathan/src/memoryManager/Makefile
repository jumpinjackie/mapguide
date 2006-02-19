#############################################################################
# Module:           Memory manager
# Module Desc:      Memory manager
# Project:          Pathan, Open Source X-Path module for Xerces C++
# Project Version:  2
# tmake Template:   pathanSubdir
# Creation Date:    14:44, 2001/09/28
#############################################################################

###### Include macro definitions

MAKEDEFS = ../../Makefile.defs
include $(MAKEDEFS)

###### Files

SOURCES     = XPath2MemoryManagerImpl.cpp \
	ProxyMemoryManager.cpp \
	StringPool.cpp \
	BaseMemoryManager.cpp

###### Build 

objects: $(PARSERCPP) ${SOURCES:.cpp=.lo}

all: objects

###### Include rules

RULES = ../../Makefile.rules
include $(RULES)
