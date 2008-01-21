## Makefile - Used to prepare Fusion distributions until its part of MGOS
##
## $Id: Makefile 659 2007-06-29 13:34:35Z pspencer $
##
## use with make, targets are
##
##  targz - builds a .tar.gz file
##  zip - builds a .zip file
##  all - builds the above :)
##
## Internal variables are:
## 
## BUILD			- the build date
## VERSION	  - the build version, update with each release
## FUSION		- the final base file name
## FUSION-DIRS   - the directories to include in the package
## JX-DIR  - the location of jx (from cvs)
##

BUILD	 = $(shell date +%Y%m%d)
VERSION = 1.0-preview
FUSION   = fusion-$(VERSION)-$(BUILD)
FUSION-DIRS  = config.xml common docs ext lib mapguide mapserver redist samples server widgets
JX-DIR	= ../jx/
CUR-DIR   =  $(shell pwd)

##
## You shouldn't need to edit anything past this point.
##

default: all

all: fusion zip targz

.PHONY: base
base:
	@if test ! -e builds; \
	then \
		echo "Making builds directory..."; \
		mkdir builds; \
	fi
	@echo "Creating builds/fusion temporary directory..."
	@if test -e builds/fusion; \
	then \
		echo "Removing previous temporary directory..."; \
		rm -rf builds/fusion; \
	fi

	@mkdir builds/fusion

.PHONY: jx
jx: base
	@echo "Making jx directory ..."
	@if test -e builds/fusion/jx; \
	then \
		echo "Removing existing jx from fusion ..."; \
		rm -rf builds/fusion/jx; \
	fi
	@if test ! -e builds/fusion/jx; \
	then \
		mkdir builds/fusion/jx; \
	fi
	@if test -e $(JX-DIR); \
	then \
		cd $(CUR-DIR); \
		cp -r $(JX-DIR) builds/fusion/jx; \
	fi

.PHONY: fusion
fusion: jx
	@echo " Adding fusion ..."
	@echo " compressing lib scripts into fusion-lib.js"
	if test -e lib/fusion-lib.js; \
	then \
	    rm lib/fusion-lib.js; \
	fi; \
	jsmin < lib/ConfigMgr.js >> lib/fusion-lib.js
	jsmin < lib/utils.js >> lib/fusion-lib.js
    jsmin < lib/EventMgr.js >> lib/fusion-lib.js
    jsmin < lib/GxCore.js >> lib/fusion-lib.js
    jsmin < lib/Error.js >> lib/fusion-lib.js
    jsmin < lib/ConfigMgr.js >> lib/fusion-lib.js
    jsmin < lib/WebCommand.js >> lib/fusion-lib.js
    jsmin < lib/WebLayout.js >> lib/fusion-lib.js
    jsmin < lib/MGBroker.js >> lib/fusion-lib.js
    jsmin < lib/Widget.js >> lib/fusion-lib.js
    jsmin < lib/ButtonBase.js >> lib/fusion-lib.js
    jsmin < lib/ButtonTool.js >> lib/fusion-lib.js
    jsmin < lib/CanvasTool.js >> lib/fusion-lib.js
    jsmin < lib/ClickTool.js >> lib/fusion-lib.js
    jsmin < lib/RectTool.js >> lib/fusion-lib.js
    jsmin < lib/GxMap.js >> lib/fusion-lib.js
    
	@for dir in $(FUSION-DIRS); do \
		if test -e $$dir; \
		then \
			cp -r $$dir builds/fusion; \
		fi; \
		if test ! -e $$dir -a ! -e builds/fusion/$$dir; \
		then \
			mkdir builds/fusion/$$dir; \
		fi; \
	done

.PHONY: archive
archive: fusion
	@if test -e builds/fusion; \
	then \
		echo "Removing CVS and backup files..." ; \
		find builds/fusion -name CVS -prune -exec rm -rf {} \; ; \
		find builds/fusion -name "*~" -exec rm -rf {} \; ; \
		find builds/fusion -name ".DS_Store" -exec rm -f {} \; ; \
	fi

.PHONEY: zip
zip: archive
	@echo "Building fusion ZIP..."
	@if test -e builds/$(FUSION).zip; \
	then \
		echo "Removing old archive file..."; \
		rm builds/$(FUSION).zip; \
	fi
	@if test -e builds/fusion; \
	then \
		echo "Archiving fusion to builds/$(FUSION).zip..."; \
		cd builds; \
		zip -r -q -9 $(FUSION).zip fusion ; \
		rm -rf fusion; \
		unzip -q $(FUSION).zip; \
		cd ..; \
	fi

.PHONEY: targz
targz: archive
	@echo "Building fusion tar.gz ..."
	@if test -e builds/$(FUSION).tar.gz; \
	then \
		echo "Removing old archive file..."; \
		rm builds/$(FUSION).tar.gz; \
	fi
	@if test -e builds/fusion; \
	then \
		echo "Archiving fusion to builds/$(FUSION).tar.gz..."; \
		cd builds; \
		tar czf $(FUSION).tar.gz fusion; \
		rm -rf fusion; \
		tar zxf $(FUSION).tar.gz; \
		cd ..; \
	fi
	