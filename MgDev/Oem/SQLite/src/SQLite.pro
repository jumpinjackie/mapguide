TEMPLATE = subdirs
CONFIG += warn_on \
          thread \
          exceptions \
          rtti
          
SUBDIRS += ./SqliteVM \
           ./SQLitePhpApi
