TEMPLATE = subdirs
CONFIG += warn_on \
          thread \
          exceptions \
          rtti
          
SUBDIRS += ../../../Common/ServiceAPI/src/Common \
           ./SqliteVM \
           ./SQLitePhpApi
