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

#ifndef _SQLITEDB_H_
#define _SQLITEDB_H_

class SqliteDB : public SqliteDisposable
{
EXTERNAL_API:

    ///Default constructor
    SqliteDB();

    ///Destructor
    ~SqliteDB();

    // <summary>Dispose this object.</summary>
    // <returns>Returns nothing</returns>
    virtual void Dispose();

    ///Closes database connection
    INT8 CloseDB();

    ///Returns current error message
    STRING GetErrMsg();

    ///Returns the database name
    STRING GetName();

    ///Checks if there exist connection
    bool IsConnected();

    ///Sets an error message
    void SetErrMsg(CREFSTRING newMsg);

    //Dumps binary SQLite database into a SQL text format.
    void DumpDatabase(CREFSTRING iniFileName);

    //Generates binary database from a SQL text file
    void GenerateDatabase(CREFSTRING dumpFileName, CREFSTRING dbFileName);

    ///Initializes a database object
    INT8 Open(CREFSTRING newName);

    //Return the class id
    INT32 GetClassId();

INTERNAL_API:

    ///Get the current connection
    sqlite3* GetConnection();

    static wstring MultiByteToWideChar(const char* mbBuf);

    static string WideCharToMultiByte(const wchar_t* wcBuf);

private:
    ///Database name
    STRING dbName;

    ///Connection with the database
    sqlite3* dbConnection;

    ///Error message
    STRING errMsg;

CLASS_ID:
    static const INT32 m_cls_id = SQLITE_SqliteDB;
};

#endif //_SQLITEDB_H
