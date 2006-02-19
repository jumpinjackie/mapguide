TEMPLATE = lib
CONFIG += warn_off thread exceptions rtti dll

TargetSwig.target = SQLitePhpApi_wrap.cpp
TargetSwig.depends = TargetIMake
TargetSwig.commands = ../../../../Oem/SWIGEx/Linux/swig -c++ -php -baseexception SqliteException -clsidcode getclassid.code -clsiddata m_cls_id -dispose "((SqliteDisposable*)arg1)->Release()" -rethrow "e->Raise();" -nodefault -o SQLitePhpApi_wrap.cpp -lib ../../../../Oem/SWIGEx/Lib SQLitePhpApi.i

TargetIMake.depends = TargetLanguage
TargetIMake.commands = ../../../../BuildTools/WebTools/IMake/Linux/IMake SQLiteApiGen.xml

TargetLanguage.commands = cp -f php.i language.i

QMAKE_EXTRA_UNIX_TARGETS += TargetSwig TargetIMake TargetLanguage

#SOURCES += SQLitePhpApi_wrap.cpp
SOURCES += SwigWrappers.cpp

DEFINES += PHP

INCLUDEPATH = ../Exceptions \
              ../SQLite3 \
              ../SqliteVM \
              ../../../../Oem/php/TSRM \
              ../../../../Oem/php/Zend \
              ../../../../Oem/php/main \
              ../../../../Oem/php \
              ../../../../Common/ServiceAPI/src/Common/Util

#########################################
#                                       #
#      Debug specific configuration     #
#                                       #
#########################################
debug {
DESTDIR = ../../bin/linux/debug
TARGET = php_SQLitePhpApid
OBJECTS_DIR = ../../obj/linux/debug/

LIBS += -L$(DevRoot)/Common/lib/debug \
        -L$(DevRoot)/UnitTest/SQLite/lib/linux/debug \
        -lSqliteVMd -lCommonLibd

}

#########################################
#                                       #
#      Release specific configuration   #
#                                       #
#########################################
release {
DESTDIR = ../../bin/linux/release
TARGET = php_SQLitePhpApi
OBJECTS_DIR = ../../obj/linux/release/

LIBS += -L$(DevRoot)/Common/lib/release \
        -L$(DevRoot)/UnitTest/SQLite/lib/linux/release \
        -lSqliteVM -lCommonLib

}
