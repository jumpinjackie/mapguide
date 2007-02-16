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

#pragma once

using namespace System;

class SqliteVM;
class SqliteDB;
class SqliteBlob;

namespace SqliteDotNet
{
    public __gc class SqliteGcBlob
    {
    public:
        SqliteGcBlob(SqliteBlob* blob);
        System::Byte Read()[];
    private:
        SqliteBlob* blob;
    };

    public __gc class Sqlite
    {
    public:
        static const int Ok = 0;
        static const int Error = 1;
        static const int Row = 100;
        static const int Done = 101;
    };

    public __gc class SqliteDb
    {
    public:
        ///Default constructor
        SqliteDb();

        ///Destructor
        ~SqliteDb();

        ///Closes database connection
        int Close();

        ///Returns current error message
        System::String* GetErrMsg();

        ///Returns the database name
        System::String* GetName();

        ///Checks if there exist connection
        bool IsConnected();

        ///Initializes a database object
        int Open(System::String* newName);

        SqliteDB* Database();

    private:
        SqliteDB* database;

    };

    public __gc class SqliteVm
    {
    public:
        ///Overloaded constructor that initializes the database connection
        SqliteVm(SqliteDb* newDb, bool noThrow);

        //Destructor. Deletes the virtual machine
        ~SqliteVm();

        ///Returns the name of the column specified
        System::String* ColumnName(int colNum);

        int ColumnNumber(System::String* colName);

        ///Returns the type of the column specified
        System::String* ColumnType(System::String* colName);

        ///Executes a query. Creates a new virtual machine
        int Execute(System::String* newQuery); // do compile and first step here

        ///Returns the sql query being executed
        System::String* GetSqlQuery();

        ///Returns the tail of an SQL query
        System::String* GetQueryTail();

        ///Returns an integer
        int GetInt(System::String* colName);

        ///Returns a string
        System::String* GetString(System::String* colName);

        ///Returns double
        double GetDouble(System::String* colName);

        ///Returns BLOB
        SqliteGcBlob* GetBlob(System::String* colName);

        ///Returns the number of columns in a  table
        int NumCols();

        ///Returns an error message
        System::String* GetErrMsg();

        ///Checks if the database objects is still connected to a database
        bool IsConnected();

        ///Gets the next row of the result
        int NextRow();

        ///Executes the next query in the cases when more than one SQL queries were specified in the first call of Execute()
        int NextQuery();

        ///Checks if there is a SQL query to execute
        bool NextQueryExists();

        ///Deletes a virtual machine
        int SqlFinalize();

    private:
        SqliteVM* vm;
    };
}
