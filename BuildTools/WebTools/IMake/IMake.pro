TEMPLATE = app 
CONFIG += warn_on 

SOURCES += IMake.cpp \
           SimpleXmlParser.cpp
           
HEADERS += SimpleXmlParser.h \
           stdafx.h

#########################################
#                                       #
#      Debug specific configuration     #
#                                       #
#########################################
debug {
DESTDIR = ./Linux
TARGET = IMake
OBJECTS_DIR = ./Linux/obj/debug 
DEFINES = _DEBUG
}

#########################################
#                                       #
#    Release specific configuration     #
#                                       #
#########################################
release {
DESTDIR = ./Linux
TARGET = IMake
OBJECTS_DIR = ./Linux/obj/debug 
DEFINES = NDEBUG
}
