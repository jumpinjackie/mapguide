#----------------------------------------------------------------------------
#       Makefile,v 1.2 2003/03/07 15:58:49 mesnier_p Exp
#
#       Makefile for soreduce utility
#----------------------------------------------------------------------------

BIN     = mkcsregdb
INSBIN  = $(ACE_ROOT)/bin

FILES   = mkcsregdb

SRC    = $(addsuffix .cpp,$(FILES))
OBJ	= $(addsuffix .o,$(FILES))

BUILD   = $(VBIN)

INSTALL = $(VBIN:%=$(INSBIN)/%$(EXEEXT))

#----------------------------------------------------------------------------
#       Include macros and targets
#----------------------------------------------------------------------------

include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include $(ACE_ROOT)/include/makeinclude/macros.GNU
include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
include $(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

#----------------------------------------------------------------------------
#       Dependencies
#----------------------------------------------------------------------------

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
