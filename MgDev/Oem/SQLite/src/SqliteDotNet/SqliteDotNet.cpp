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

#include "stdafx.h"
#include <string>
using namespace std;

#include "SqliteWrap.h"
#include "SqliteDotNet.h"

using namespace SqliteDotNet;
using namespace System::Runtime::InteropServices;


wstring SToWs(System::String* str)
{
    System::IntPtr strPtr = Marshal::StringToCoTaskMemUni(str);
    wstring retStr = (wchar_t*) strPtr.ToPointer();
    return retStr;
}

System::String* WsToS(wstring str)
{
    System::IntPtr ptr((void*)str.c_str());
    return Marshal::PtrToStringUni(ptr);
}


///Default constructor
SqliteDb::SqliteDb()
{
    database = new SqliteDB();
}

///Destructor
SqliteDb::~SqliteDb()
{
    Close();
}

///Closes database connection
int SqliteDb::Close()
{
    int err = SQLITE_ERROR;
    if (database != NULL)
    {
        err = database->CloseDB();
        delete database;
        database = NULL;
    }
    return err;
}

///Returns current error message
System::String* SqliteDb::GetErrMsg()
{
    System::String* str = NULL;
    if (database != NULL)
    {
        wstring msg = database->GetErrMsg();
        str = WsToS(msg);
    }
    return str;
}

///Returns the database name
System::String* SqliteDb::GetName()
{
    System::String* str = NULL;
    if (database != NULL)
    {
        wstring msg = database->GetName();
        str = WsToS(msg);
    }
    return str;
}

///Checks if there exist connection
bool SqliteDb::IsConnected()
{
    if (database != NULL)
    {
        return database->IsConnected();
    }
    return false;
}

///Initializes a database object
int SqliteDb::Open(System::String* newName)
{
    if (database != NULL)
    {
        wstring str = SToWs(newName);
        return database->Open(str);
    }
    return SQLITE_ERROR;
}

SqliteDB* SqliteDb::Database()
{
    return database;
}

///Overloaded constructor that initializes the database connection
SqliteVm::SqliteVm(SqliteDb* newDb, bool noThrow)
{
    vm = new SqliteVM(newDb->Database(), noThrow);
}

//Destructor. Deletes the virtual machine
SqliteVm::~SqliteVm()
{
    if (vm != NULL)
    {
        delete vm;
        vm = NULL;
    }
};

///Returns the name of the column specified
System::String* SqliteVm::ColumnName(int colNum)
{
    System::String* str = NULL;
    if (vm != NULL)
    {
        wstring wstr = vm->ColumnName(colNum);
        str = WsToS(wstr);
    }
    return str;
}

int SqliteVm::ColumnNumber(System::String* colName)
{
    int num = -1;
    if (vm != NULL)
    {
        wstring wstrIn = SToWs(colName);
        num = vm->ColumnNumber(wstrIn);
    }
    return num;
}

///Returns the type of the column specified
System::String* SqliteVm::ColumnType(System::String* colName)
{
    System::String* str = NULL;
    if (vm != NULL)
    {
        wstring wstrIn = SToWs(colName);
        wstring wstr = vm->ColumnType(wstrIn);
        str = WsToS(wstr);
    }
    return str;
}

///Executes a query. Creates a new virtual machine
int SqliteVm::Execute(System::String* newQuery)
{
    int sqlErr = SQLITE_ERROR;
    if (vm != NULL)
    {
        wstring wstrIn = SToWs(newQuery);
        sqlErr = vm->Execute(wstrIn);
    }
    return sqlErr;
}

///Returns the sql query being executed
System::String* SqliteVm::GetSqlQuery()
{
    System::String* str = NULL;
    if (vm != NULL)
    {
        wstring wstr = vm->GetSqlQuery();
        str = WsToS(wstr);
    }
    return str;
}

///Returns the tail of an SQL query
System::String* SqliteVm::GetQueryTail()
{
    System::String* str = NULL;
    if (vm != NULL)
    {
        wstring wstr = vm->GetQueryTail();
        str = WsToS(wstr);
    }
    return str;
}

///Returns an integer
int SqliteVm::GetInt(System::String* colName)
{
    int ret = 0;
    if (vm != NULL)
    {
        wstring wstrIn = SToWs(colName);
        ret = vm->GetInt(wstrIn);
    }
    return ret;
}

///Returns a string
System::String* SqliteVm::GetString(System::String* colName)
{
    System::String* ret = NULL;
    if (vm != NULL)
    {
        wstring wstrIn = SToWs(colName);
        wstring wstr = vm->GetString(wstrIn);
        ret = WsToS(wstr);
    }
    return ret;
}

///Returns double
double SqliteVm::GetDouble(System::String* colName)
{
    double ret = 0;
    if (vm != NULL)
    {
        wstring wstrIn = SToWs(colName);
        ret = vm->GetDouble(wstrIn);
    }
    return ret;
}

///Returns BLOB
SqliteGcBlob* SqliteVm::GetBlob(System::String* colName)
{
    SqliteGcBlob* blob = NULL;

    if (vm != NULL)
    {
        wstring wstrIn = SToWs(colName);
        int blobLen = 0;
        SqliteBlob* tmpBlob = vm->GetBlob(wstrIn);
        if (tmpBlob != NULL)
        {
            blob = new SqliteGcBlob(tmpBlob);
        }
    }
    return blob;
}

///Returns the number of columns in a  table
int SqliteVm::NumCols()
{
    int ret = 0;
    if (vm != NULL)
    {
        ret = vm->NumCols();
    }
    return ret;
}

///Returns an error message
System::String* SqliteVm::GetErrMsg()
{
    System::String* ret = NULL;
    if (vm != NULL)
    {
        wstring wstr = vm->GetErrMsg();
        ret = WsToS(wstr);
    }
    return ret;
}

///Checks if the database objects is still connected to a database
bool SqliteVm::IsConnected()
{
    bool ret = false;
    if (vm != NULL)
    {
        ret = vm->IsConnected();
    }
    return ret;
}

///Gets the next row of the result
int SqliteVm::NextRow()
{
    int ret = SQLITE_ERROR;
    if (vm != NULL)
    {
        ret = vm->NextRow();
    }
    return ret;
}

///Executes the next query in the cases when more than one SQL queries were specified in the first call of Execute()
int SqliteVm::NextQuery()
{
    int ret = SQLITE_ERROR;
    if (vm != NULL)
    {
        ret = vm->NextQuery();
    }
    return ret;
}

///Checks if there is a SQL query to execute
bool SqliteVm::NextQueryExists()
{
    bool ret = false;
    if (vm != NULL)
    {
        ret = vm->NextQueryExists();
    }
    return ret;
}

///Deletes a virtual machine
int SqliteVm::SqlFinalize()
{
    int ret = SQLITE_ERROR;
    if (vm != NULL)
    {
        ret = vm->SqlFinalize();
    }
    return ret;
}


SqliteGcBlob::SqliteGcBlob(SqliteBlob* blob)
{
    this->blob = blob;
}

System::Byte SqliteGcBlob::Read()[]
{
    int bufSize = 0;
    char* buf = NULL;

    unsigned char tmpBuf[8192];

    int nBytes = (blob != NULL) ? blob->Read(tmpBuf, 8192) : 0;

    while (nBytes > 0)
    {
        bufSize += nBytes;
        char* newBuf = new char[bufSize];
        if (bufSize - nBytes > 0)
        {
            memcpy(newBuf, buf, bufSize - nBytes);
        }

        memcpy(&newBuf[bufSize-nBytes], tmpBuf, nBytes);
        delete [] buf;
        buf = newBuf;

        nBytes = blob->Read(tmpBuf, 8192);
    }

    System::Byte bytes[] = NULL;

    if (bufSize > 0)
    {
        bytes = new System::Byte[bufSize];
        System::IntPtr bufPtr(buf);
        Marshal::Copy(bufPtr, bytes, 0, bufSize);
        delete [] buf;
    }

    if (nBytes == 0 && blob != NULL)
    {
        delete blob;
        blob = NULL;
    }

    return bytes;
}
