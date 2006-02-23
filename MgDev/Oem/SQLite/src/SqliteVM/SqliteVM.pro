TEMPLATE = lib 
CONFIG += warn_on thread exceptions rtti staticlib

SOURCES += ../SQLite3/attach.c \
           ../SQLite3/auth.c \
           ../SQLite3/btree.c \
           ../SQLite3/build.c \
           ../SQLite3/date.c \
           ../SQLite3/delete.c \
           ../SQLite3/expr.c \
           ../SQLite3/func.c \
           ../SQLite3/hash.c \
           ../SQLite3/insert.c \
           ../SQLite3/legacy.c \
           ../SQLite3/main.c \
           ../SQLite3/opcodes.c \
           ../SQLite3/os_mac.c \
           ../SQLite3/os_unix.c \
           ../SQLite3/os_win.c \
           ../SQLite3/pager.c \
           ../SQLite3/parse.c \
           ../SQLite3/pragma.c \
           ../SQLite3/printf.c \
           ../SQLite3/random.c \
           ../SQLite3/select.c \
           ../SQLite3/shell.c \
           ../SQLite3/table.c \
           ../SQLite3/tokenize.c \
           ../SQLite3/trigger.c \
           ../SQLite3/update.c \
           ../SQLite3/utf.c \
           ../SQLite3/util.c \
           ../SQLite3/vacuum.c \
           ../SQLite3/vdbe.c \
           ../SQLite3/vdbeapi.c \
           ../SQLite3/vdbeaux.c \
           ../SQLite3/vdbemem.c \
           ../SQLite3/where.c \
           ../Exceptions/SqliteException.cpp \
           ../Exceptions/CantCloseDbException.cpp \
           ../Exceptions/CantOpenDbException.cpp \
           ../Exceptions/ColumnNotFoundException.cpp \
           ../Exceptions/ExecuteErrorException.cpp \
           ../Exceptions/NoDbConnectionException.cpp \
           ../Exceptions/SQLiteFinalizeException.cpp \
           ../Exceptions/SQLitePrepareException.cpp \
           ../Exceptions/SQLiteVMConstructException.cpp \
           ../Exceptions/TypeMismatchException.cpp \
           ../Exceptions/OutOfMemoryException.cpp \
           ../Exceptions/InvalidArgumentException.cpp \
           SqliteBlob.cpp \
           SqliteDB.cpp \
           SqliteDisposable.cpp \
           SqliteVM.cpp


HEADERS += ../SQLite3/btree.h \
           ../SQLite3/config.h \
           ../SQLite3/hash.h \
           ../SQLite3/opcodes.h \
           ../SQLite3/os.h \
           ../SQLite3/os_common.h \
           ../SQLite3/os_mac.h \
           ../SQLite3/os_unix.h \
           ../SQLite3/os_win.h \
           ../SQLite3/pager.h \
           ../SQLite3/parse.h \
           ../SQLite3/sqlite3.def \
           ../SQLite3/sqlite3.h \
           ../SQLite3/sqliteInt.h \
           ../SQLite3/vdbe.h \
           ../SQLite3/vdbeInt.h \
           ../Exceptions/SQLiteExceptions.h \
           ../Exceptions/SqliteException.h \
           ../Exceptions/CantCloseDbException.h \
           ../Exceptions/CantOpenDbException.h \
           ../Exceptions/ColumnNotFoundException.h \
           ../Exceptions/ExecuteErrorException.h \
           ../Exceptions/NoDbConnectionException.h \
           ../Exceptions/SQLiteFinalizeException.h \
           ../Exceptions/SQLitePrepareException.h \
           ../Exceptions/SQLiteVMConstructException.h \
           ../Exceptions/TypeMismatchException.h \
           ../Exceptions/OutOfMemoryException.h \
           ../Exceptions/InvalidArgumentException.h \
           ClassId.h \
           CommonDefs.h \
           SqliteBlob.h \
           SqliteDB.h \
           SQLiteDefs.h \
           SqliteDisposable.h \
           SqliteVM.h \
           SqliteWrap.h  \

INCLUDEPATH = ../Exceptions \
              ../SQLite3 \
              ../SqliteVM \
              ../../../../Common/ServiceAPI/src/Common/Util

#########################################
#                                       #
#      Debug specific configuration     #
#                                       #
#########################################
debug {
DESTDIR = ../../lib/linux/debug 
TARGET = SqliteVMd 
OBJECTS_DIR = ../../obj/linux/debug

}

#########################################
#                                       #
#    Release specific configuration     #
#                                       #
#########################################
release {
DESTDIR = ../../lib/linux/release
TARGET = SqliteVM
OBJECTS_DIR = ../../obj/linux/release

}
