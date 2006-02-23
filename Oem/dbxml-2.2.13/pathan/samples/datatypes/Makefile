#############################################################################
# Module:           test-datatypes
# Module Desc:      Unit testing of the new datatyping system
# Project:          
# Project Version:  1
# Creation Date:    17:19, 2001/05/25
#############################################################################

###### Include macro definitions for samples

MAKEDEFS = ../../Makefile.defs

include $(MAKEDEFS)

DTLIBS = ../../lib/libpathan.la $(XERCESLIB) -ldl

DTINCPATH = $(INCPATH) -I../../samples/xgrep

###### Files

OBJS    = test-datatypes.o \
          DatatypeTester.o \
          ATAnyURITester.o \
          ATBase64BinaryTester.o \
          ATBooleanTester.o \
          ATDateTester.o \
          ATDateTimeTester.o \
          ATDecimalTester.o \
          ATDoubleTester.o \
          ATDurationTester.o \
          ATFloatTester.o \
          ATGDayTester.o \
          ATGMonthDayTester.o \
          ATGMonthTester.o \
          ATGYearMonthTester.o \
          ATGYearTester.o \
          ATHexBinaryTester.o \
          ATQNameTester.o \
          ATStringTester.o \
          ATTimeTester.o \
          ATUntypedAtomicTester.o \
          CastAsTester.o \
          TypePromotionTester.o \
          ../../samples/common/DOMTreeErrorReporter.o 

###### Build 

test-datatypes: $(OBJS) ../../lib/libpathan.la
	$(LIBTOOL) --mode=link $(CXX) -ggdb -o test-datatypes $(OBJS) $(DTINCPATH) $(LIBPATH) $(DTLIBS) $(LIBS) $(LINKFLAGS)

all: test-datatypes

clean:
	$(RM) $(OBJS) core test-datatypes

###### Implicit Rules

%.o: %.cpp
	$(CXX) -c $(DEFINES) -g $(CXXFLAGS) $(DTINCPATH) -o $@ $<
%.o: %.c
	$(CC) -c $(DEFINES) -g $(CFLAGS) $(DTINCPATH) -o $@ $<
