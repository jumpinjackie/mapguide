//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef _SQLITEVM_H_
#define _SQLITEVM_H_

class SqliteDB;
class SqliteBlob;

class SqliteVM : public SqliteDisposable
{
EXTERNAL_API:

    ///Overloaded constructor that initializes the database connection
    ///TODO: Need to define exceptions for SqliteDB and SqliteVM because if the constructor fails
    ///the user is not aware about that
    SqliteVM (SqliteDB* newDb, bool noThrowFlag=false);

    //Destructor. Deletes the virtual machine
    ~SqliteVM();

    // <summary>Dispose this object.</summary>
    // <returns>Returns nothing</returns>
    virtual void Dispose();

    ///Deletes a virtual machine
    INT8 SqlFinalize();

    ///Returns the name of the column specified
    STRING ColumnName(INT8 colNum);

    INT8 ColumnNumber(CREFSTRING colName);

    ///Returns the type of the column specified
    STRING ColumnType(CREFSTRING colName);

    ///Executes a query. Creates a new virtual machine
    INT8 Execute(CREFSTRING newQuery); // do compile and first step here

    /// Prepares an sql statment for processing.  Call
    /// Execute() to execute.  Bound arguments are specified
    /// in the sql as ?nnn (eg ?1).
    /// A new virtual machine will be created.
    INT8 Prepare(CREFSTRING sqlStatement);

    /// Executes a previously prepared sql statement
    INT8 Execute();

    ///Bind a blob data type to a prepared sql statement
    INT8 BindBlob(CREFSTRING token, char* blob, int blobSize);

    ///Returns the sql query being executed
    STRING GetSqlQuery();

    ///Returns the tail of an SQL query
    STRING GetQueryTail();

    ///Returns an integer
    INT32 GetInt(CREFSTRING colName);

    ///Returns a string
    STRING GetString(CREFSTRING colName);

    ///Returns double
    double GetDouble(CREFSTRING colName);

    ///Returns BLOB
    SqliteBlob* GetBlob(CREFSTRING colName);

    ///Returns the number of columns in a  table
    INT8 NumCols();

    ///Returns an error message
    STRING GetErrMsg();

    ///Checks if the database objects is still connected to a database
    bool IsConnected();

    ///Gets the next row of the result
    INT8 NextRow();

    ///Executes the next query in the cases when more than one SQL queries were specified in the first call of Execute()
    INT8 NextQuery();

    ///Checks if there is a SQL query to execute
    bool NextQueryExists();

    ///Set an error message
    void SetErrMsg(CREFSTRING errMsg);

    //Return the class id
    INT32 GetClassId();

private:

    ///Compiles an sql statement
    INT8 SqlCompile();

    ///Returns pointer to a database object
    SqliteDB* GetDb();

    ///Pointer to a database
    SqliteDB* db;

    ///The virtual machine created
    sqlite3_stmt* virtualMachine;

    ///SQL query to be executed
    STRING sqlQuery;

    ///Remaining part of a query.
    STRING queryTail;

    ///Number of columns in the result
    INT8 pN;

    ///Error message
    STRING errMsg;

    //Flag whether to throw an exception in the Get* methods when column is not found
    //The default is to throw exceptions in those cases
    bool noThrow;

CLASS_ID:
    static const INT32 m_cls_id = SQLITE_SqliteVM;
};

#endif
