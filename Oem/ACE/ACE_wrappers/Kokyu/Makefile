# Makefile,v 1.5 2003/08/11 06:34:35 jwillemsen Exp

#----------------------------------------------------------------------------
#       GNU Workspace
#----------------------------------------------------------------------------

include $(ACE_ROOT)/include/makeinclude/macros.GNU
TARGETS_NESTED := $(TARGETS_NESTED:.nested=)

$(TARGETS_NESTED):
	@$(MAKE) -f Makefile.Kokyu -C . $(@);
