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

#include "SqliteWrap.h"

class MgObject;
class MgByteReader;

#include "UnicodeString.h"

extern "C"
{
    int newMain(int argc, char **argv);
}

using namespace std;

//Default constructor. Initializes all private members to NULL
SqliteDB::SqliteDB()
:
dbName(L""),
dbConnection(NULL),
errMsg(L"")
{
}


//Destructor. Closes the connection and deallocates the space for db_name
SqliteDB::~SqliteDB()
{
    CloseDB();
}

void SqliteDB::Dispose()
{
    delete this;
}

//Closes existing connection
INT8 SqliteDB::CloseDB()
{
    INT8 status = 0;

    if (dbConnection != NULL)
    {
        status = sqlite3_close( dbConnection );
        if ( SQLITE_OK != status )
        {
            SetErrMsg( SqliteDB::MultiByteToWideChar(sqlite3_errmsg( dbConnection )));
            throw new CantCloseDbException(L"SqliteDB :: CloseDB()", __LINE__, __WFILE__, L"Cannot close database file");
        }
        else
        {
            dbConnection = NULL;
        }
    }
    return status;
}

//Return the current connection. NULL if no connection exists
sqlite3* SqliteDB::GetConnection()
{
    return dbConnection;
}

//Return the current database. NULL if no database selected
STRING SqliteDB::GetName()
{
    return dbName;
}

//Return an error message
STRING SqliteDB::GetErrMsg()
{
    return errMsg;
}

//Checks if there exist a connection
bool SqliteDB::IsConnected()
{
    return ( dbConnection != NULL ) ;
}

//Sets error message
void SqliteDB::SetErrMsg(CREFSTRING newMsg)
{
    errMsg = newMsg;
}


//Initilializes the database name to the given string
//and tries to establish connection with that database
INT8 SqliteDB::Open(CREFSTRING newName)
{
    //Status variable
    INT8 status = SQLITE_OK;

    //Delete any old connecitons
    if ( dbConnection != NULL )
    {
        status = SQLITE_MISUSE;
        SetErrMsg(L"A database is already open.  Close existing database first.\n");
        throw new CantOpenDbException(L"SqliteDB::Open(STRING newName)", __LINE__, __WFILE__, L"Cannot open database file");
    }
    else
    {
        //If we receive an empty string set the status and and the error message
        if ( newName == L"" )
        {
            status = SQLITE_MISUSE;
            SetErrMsg(L"The database name provided is invalid\n");
            throw new CantOpenDbException(L"SqliteDB::Open(STRING newName)", __LINE__, __WFILE__, L"Cannot open database file");
        }
        //Else initialize db_name to the provided string and
        //try to establish connection with the database
        else
        {
            dbName = newName;
            string database = SqliteDB::WideCharToMultiByte(dbName.c_str());

            status = sqlite3_open(database.c_str(), &dbConnection);
            //If open fails reset to an empty object
            if ( SQLITE_OK != status )
            {
                SetErrMsg( SqliteDB::MultiByteToWideChar(sqlite3_errmsg( dbConnection )));
                dbName = L"";
                dbConnection = NULL;
                throw new CantOpenDbException(L"SqliteDB::Open(STRING newName)", __LINE__, __WFILE__, L"Cannot open database file");
            }
        }
    }
    return status;
}

void SqliteDB::DumpDatabase(CREFSTRING iniFileName)
{
    int length = 0;
    INT8 numArgs=5;
    char** argsArray = new char*[5];

    argsArray[0]= strdup("sqlite3.exe");

    argsArray[1] = strdup("-init");

    length = static_cast<int>(wcslen(iniFileName.c_str()))+1;
    argsArray[2]= (char*) malloc(length);
    wcstombs(argsArray[2],iniFileName.c_str(),length);

    length = static_cast<int>(wcslen(dbName.c_str()))+1;
    argsArray[3] = (char*) malloc(length);
    wcstombs(argsArray[3],dbName.c_str(),length);

    argsArray[4]=strdup(".dump");

    newMain(numArgs, argsArray);

    delete [] argsArray[0];
    delete [] argsArray[1];
    delete [] argsArray[2];
    delete [] argsArray[3];
    delete [] argsArray;
}

void SqliteDB::GenerateDatabase(CREFSTRING dumpFileName, CREFSTRING dbFileName)
{
    char* temp=NULL;
    int length = 0;
    INT8 numArgs=3;
    char** argsArray = new char*[4];

    argsArray[0]= strdup("sqlite3.exe");

    length = wcstombs(NULL, dbFileName.c_str(), 1024);
    argsArray[1] = new char[length+1];
    memset(argsArray[1], 0, length+1);
    wcstombs(argsArray[1], dbFileName.c_str(),length);

    length = wcstombs(NULL, dumpFileName.c_str(), 1024);
    temp= new char[length+1];
    memset(temp, 0, length+1);
    wcstombs(temp,dumpFileName.c_str(),length);
    argsArray[2] = new char[length+strlen(".read ")];
    strcpy(argsArray[2], ".read ");
    strcat(argsArray[2], temp);

    newMain(numArgs, argsArray);

    delete [] temp;
    delete [] argsArray[1];
    delete [] argsArray[2];
    delete [] argsArray;
}

INT32 SqliteDB::GetClassId()
{
    return m_cls_id;
}

wstring SqliteDB::MultiByteToWideChar(const char* mbBuf)
{
    wstring wcBuf;

    if (0 != mbBuf)
    {
        try
        {
            UnicodeString::MultiByteToWideChar(mbBuf, wcBuf);
        }
        catch (int)
        {
            throw new InvalidArgumentException(
                L"SqliteDB.MultiByteToWideChar", __LINE__, __WFILE__, L"The argument provided is invalid");
        }
    }
    return wcBuf;
}

string SqliteDB::WideCharToMultiByte(const wchar_t* wcBuf)
{
    string mbBuf;

    if (0 != wcBuf)
    {
        try
        {
            UnicodeString::WideCharToMultiByte(wcBuf, mbBuf);
        }
        catch (int)
        {
            throw new InvalidArgumentException(
                L"SqliteDB.WideCharToMultiByte", __LINE__, __WFILE__, L"The argument provided is invalid");
        }
    }
    return mbBuf;
}
