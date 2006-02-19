//======================================================
// Generated with IMake version 1.0
// Fri Feb 10 18:00:54 2006

//
%module SQLitePhpApi
%{
#ifdef WIN32
#pragma warning(disable: 4251)
#endif

#include <string>
#include <map>
#include "SqliteWrap.h"

typedef wstring STRINGPARAM;
%}


%include "language.i"	//typemaps specific for each language
%include "../SqliteVM/ClassId.h"

typedef char*   BYTE_ARRAY_OUT;
typedef char*   BYTE_ARRAY_IN;
typedef wstring STRINGPARAM;
typedef unsigned char    INT8;
typedef short   INT16;
typedef long    INT32;
typedef unsigned long UINT32;
typedef long long INT64;

// Definitions from file ../SqliteVM/SqliteDisposable.h
//
class SqliteDisposable
{
public:

};


// Definitions from file ../Exceptions/SqliteException.h
//
class SqliteException : public SqliteDisposable 
{
public:
   virtual STRING GetMessage ( ) throw ( ) ; 
   virtual ~SqliteException();
  
private:
  static const INT32 m_cls_id = SQLite_Exception ; 
};


// Definitions from file ../Exceptions/CantCloseDbException.h
//
class CantCloseDbException : public SqliteException 
{
public:
   CantCloseDbException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ CantCloseDbException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLite_Exception_CantCloseDbException ; 
};


// Definitions from file ../Exceptions/CantOpenDbException.h
//
class CantOpenDbException : public SqliteException 
{
public:
   CantOpenDbException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ CantOpenDbException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_CantOpenDbException ; 
};


// Definitions from file ../Exceptions/ColumnNotFoundException.h
//
class ColumnNotFoundException : public SqliteException 
{
public:
   ColumnNotFoundException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ ColumnNotFoundException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_ColumnNotFoundException ; 
};


// Definitions from file ../Exceptions/ExecuteErrorException.h
//
class ExecuteErrorException : public SqliteException 
{
public:
   ExecuteErrorException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ ExecuteErrorException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_ExecuteErrorException ; 
};


// Definitions from file ../Exceptions/NoDbConnectionException.h
//
class NoDbConnectionException : public SqliteException 
{
public:
   NoDbConnectionException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ NoDbConnectionException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_NoDbConnectionException ; 
};


// Definitions from file ../Exceptions/SQLiteFinalizeException.h
//
class SQLiteFinalizeException : public SqliteException 
{
public:
   SQLiteFinalizeException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ SQLiteFinalizeException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_SQLiteFinalizeException ; 
};


// Definitions from file ../Exceptions/SQLitePrepareException.h
//
class SQLitePrepareException : public SqliteException 
{
public:
   SQLitePrepareException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ SQLitePrepareException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_SQLitePrepareException ; 
};


// Definitions from file ../Exceptions/SQLiteVMConstructException.h
//
class SQLiteVMConstructException : public SqliteException 
{
public:
   SQLiteVMConstructException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ SQLiteVMConstructException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_SQLiteVMConstructException ; 
};


// Definitions from file ../Exceptions/TypeMismatchException.h
//
class TypeMismatchException : public SqliteException 
{
public:
   TypeMismatchException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ TypeMismatchException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_Exception_TypeMismatchException ; 
};


// Definitions from file ../Exceptions/InvalidArgumentException.h
//
class InvalidArgumentException : public SqliteException 
{
public:
   InvalidArgumentException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ InvalidArgumentException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLite_Exception_InvalidArgumentException ; 
};


// Definitions from file ../Exceptions/OutOfMemoryException.h
//
class OutOfMemoryException : public SqliteException 
{
public:
   OutOfMemoryException ( STRINGPARAM methodName , UINT32 lineNumber , STRINGPARAM fileName , STRINGPARAM message ) throw ( ) ; 
   virtual ~ OutOfMemoryException ( ) throw ( ) ; 
   STRING GetMessage ( ) throw ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLite_Exception_OutOfMemoryException ; 
};


// Definitions from file ../SqliteVM/SqliteDB.h
//
class SqliteDB : public SqliteDisposable 
{
public:
   SqliteDB ( ) ; 
   ~ SqliteDB ( ) ; 
   virtual void Dispose ( ) ; 
   INT8 CloseDB ( ) ; 
   STRING GetErrMsg ( ) ; 
   STRING GetName ( ) ; 
   bool IsConnected ( ) ; 
   void SetErrMsg ( STRINGPARAM newMsg ) ; 
   void DumpDatabase ( STRINGPARAM iniFileName ) ; 
   void GenerateDatabase ( STRINGPARAM dumpFileName , STRINGPARAM dbFileName ) ; 
   INT8 Open ( STRINGPARAM newName ) ; 
   INT32 GetClassId ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_SqliteDB ; 
};


// Definitions from file ../SqliteVM/SqliteVM.h
//
class SqliteVM : public SqliteDisposable 
{
public:
   SqliteVM ( SqliteDB * newDb , bool noThrowFlag = false ) ; 
   ~ SqliteVM ( ) ; 
   virtual void Dispose ( ) ; 
   INT8 SqlFinalize ( ) ; 
   STRING ColumnName ( INT8 colNum ) ; 
   INT8 ColumnNumber ( STRINGPARAM colName ) ; 
   STRING ColumnType ( STRINGPARAM colName ) ; 
   INT8 Execute ( STRINGPARAM newQuery ) ; 
   INT8 Prepare ( STRINGPARAM sqlStatement ) ; 
   INT8 Execute ( ) ; 
   INT8 BindBlob ( STRINGPARAM token , char * blob , int blobSize ) ; 
   STRING GetSqlQuery ( ) ; 
   STRING GetQueryTail ( ) ; 
   INT32 GetInt ( STRINGPARAM colName ) ; 
   STRING GetString ( STRINGPARAM colName ) ; 
   double GetDouble ( STRINGPARAM colName ) ; 
   SqliteBlob * GetBlob ( STRINGPARAM colName ) ; 
   INT8 NumCols ( ) ; 
   STRING GetErrMsg ( ) ; 
   bool IsConnected ( ) ; 
   INT8 NextRow ( ) ; 
   INT8 NextQuery ( ) ; 
   bool NextQueryExists ( ) ; 
   void SetErrMsg ( STRINGPARAM errMsg ) ; 
   INT32 GetClassId ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_SqliteVM ; 
};


// Definitions from file ../SqliteVM/SqliteBlob.h
//
class SqliteBlob : public SqliteDisposable 
{
public:
   ~ SqliteBlob ( ) ; 
   virtual void Dispose ( ) ; 
   INT32 Read ( BYTE_ARRAY_OUT buffer , INT32 length ) ; 
   INT32 GetClassId ( ) ; 
   
private:
  static const INT32 m_cls_id = SQLITE_SqliteBlob ; 
};

