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

SqliteVM::SqliteVM( SqliteDB* newDb, bool noThrowFlag)
:
virtualMachine (NULL),
sqlQuery (L""),
queryTail (L""),
pN (0),
errMsg (L""),
noThrow(noThrowFlag)
{
    if ( newDb != NULL )
    {
        db = newDb;
    }
    else
    {
        SetErrMsg(L"Database pointer is NULL");
        throw new SQLiteVMConstructException(L"SqliteVM::SqliteVM", __LINE__, __WFILE__, L"Cannot construct virtual machine");
    }
}

SqliteVM :: ~SqliteVM()
{
    //Delete the virtual machine
    SqlFinalize();
}

void SqliteVM::Dispose()
{
    delete this;
}

//Returns the name of the column specified in colNum
//Returns NULL if column does not exist
STRING SqliteVM :: ColumnName(INT8 colNum)
{
    STRING colName = L"";
    //Check if the column exist. pN is number of columns in the result. If there is no result then pN is equal to zero
    if ( colNum < pN)
    {
        colName = SqliteDB::MultiByteToWideChar(sqlite3_column_name(virtualMachine, colNum));
    }
    else
    {
        SetErrMsg(L"Column does not exist");
        if (!noThrow)
        {
            throw new ColumnNotFoundException(L"SqliteVM :: ColumnName(INT8 colNum)", __LINE__, __WFILE__, L"Column not found");
        }
    }
    return colName;
}

INT8 SqliteVM::ColumnNumber(CREFSTRING colName)
{
    INT8 count =0;
    INT8 colNum = -1;
    wstring temp;

    while (count<pN)
    {
        temp = SqliteDB::MultiByteToWideChar(sqlite3_column_name(virtualMachine, count));
        if (colName == temp)
        {
            colNum = count;
        }
        count++;
    }
    return colNum;
}
//Returns the type of the column specified
//Returns empty string if column does not exist
STRING SqliteVM :: ColumnType(CREFSTRING colName)
{
    //Loop counter
    int count = 0;
    //Returns the column type
    STRING colType(L"");

    //Loop over all columns and if the column name is found exit the loop
    //pN is used to offset the vector columnNames in order to return the column type
    while ( (count < pN) && (colType == L"") )
    {
        if ( colName == SqliteDB::MultiByteToWideChar(sqlite3_column_name(virtualMachine, count)))
        {
            colType = SqliteDB::MultiByteToWideChar(sqlite3_column_decltype(virtualMachine, count));
        }
        count++;
    }

    if (L"" == colType)
    {
        SetErrMsg(L"Column does not exist");
        if (!noThrow)
        {
            throw new ColumnNotFoundException(L"SqliteVM :: ColumnType(CREFSTRING colName)", __LINE__, __WFILE__, L"Column not found");
        }
    }
    return colType;
}

//Execute a new query
INT8 SqliteVM :: Execute(CREFSTRING newQuery)
{
    INT8 status= SQLITE_OK;
    //Check if we are connected to a database
    if (IsConnected())
    {
        //Check the validity of the new_query string
        if ( L"" != newQuery  )
        {
            sqlQuery = newQuery;

            //Check if there exist a virtual machine and delete it
            if (virtualMachine != NULL)
            {
                SqlFinalize();
            }
            //Compile the new query and create a new virtual machine

            status = SqlCompile();
            //Check if compile succeeded and created a virtual machine
            if ( status == SQLITE_OK  )
            {
                status = sqlite3_step(virtualMachine);
                if (((SQLITE_ROW == status) || (SQLITE_DONE == status)))
                {
                    pN = sqlite3_column_count( virtualMachine );
                }
                else if (SQLITE_DONE != status)
                {
                    pN = 0;
                    SetErrMsg(SqliteDB::MultiByteToWideChar(sqlite3_errmsg(db->GetConnection())));
                    throw new ExecuteErrorException(L"SqliteVM :: Execute (CREFSTRING newQuery)", __LINE__, __WFILE__, L"Unable to execute query");
                }
            }
            else
            {
                throw new ExecuteErrorException(L"SqliteVM :: Execute (CREFSTRING newQuery)", __LINE__, __WFILE__, L"Unable to execute query");
            }
        }
        //Set an error message
        else
        {
            SetErrMsg(L"Invalid query string provided\n");
            status = SQLITE_MISUSE;
            throw new ExecuteErrorException(L"SqliteVM :: Execute (CREFSTRING newQuery)", __LINE__, __WFILE__, L"Unable to execute query");
        }
    }
    //No connection to a database
    else
    {
        SetErrMsg(L"Not able to execute a query. No connection with a database");
        status = SQLITE_MISUSE;
        throw new ExecuteErrorException(L"SqliteVM :: Execute (CREFSTRING newQuery)", __LINE__, __WFILE__, L"Unable to execute query");
    }
    return status;
}

INT8 SqliteVM::Prepare(CREFSTRING sqlStatement)
{
    INT8 status= SQLITE_OK;
    //Check if we are connected to a database
    if (IsConnected())
    {
        //Check the validity of the new_query string
        if ( L"" != sqlStatement  )
        {
            sqlQuery = sqlStatement;

            //Check if there exist a virtual machine and delete it
            if (virtualMachine != NULL)
            {
                SqlFinalize();
            }
            //Compile the new query and create a new virtual machine

            status = SqlCompile();
            //Check if compile succeeded and created a virtual machine
            if ( status != SQLITE_OK  )
            {
                throw new ExecuteErrorException(L"SqliteVM :: Prepare (CREFSTRING newQuery)", __LINE__, __WFILE__, L"Unable to execute query");
            }
        }
        //Set an error message
        else
        {
            SetErrMsg(L"Invalid query string provided\n");
            status = SQLITE_MISUSE;
            throw new ExecuteErrorException(L"SqliteVM :: Prepare (CREFSTRING newQuery)", __LINE__, __WFILE__, L"Unable to execute query");
        }
    }
    //No connection to a database
    else
    {
        SetErrMsg(L"Not able to execute a query. No connection with a database");
        status = SQLITE_MISUSE;
        throw new ExecuteErrorException(L"SqliteVM :: Prepare (CREFSTRING newQuery)", __LINE__, __WFILE__, L"Unable to execute query");
    }
    return status;
}


INT8 SqliteVM::Execute()
{
    INT8 status = sqlite3_step(virtualMachine);
    if ( ( (SQLITE_ROW == status) || (SQLITE_DONE == status) ) )
    {
        pN = sqlite3_column_count( virtualMachine );
    }
    else
    {
        pN = 0;
        SetErrMsg( SqliteDB::MultiByteToWideChar(sqlite3_errmsg(db->GetConnection())));
        throw new ExecuteErrorException(L"SqliteVM :: Execute ()", __LINE__, __WFILE__, L"Unable to execute query");
    }
    return status;
}

INT8 SqliteVM::BindBlob(CREFSTRING token, char* blob, int blobSize)
{
    INT8 status = SQLITE_OK;

    char* mbBuf = 0;

    size_t size = ::wcstombs(0, token.c_str(), 0) + 1; // ::wcslen(wcBuf) + 1;
    if (size > 1)
    {
        size *= sizeof(wchar_t);
        mbBuf = new char[size];

        if (0 != mbBuf)
        {
            size = ::wcstombs(mbBuf, token.c_str(), size);
        }
    }

    if (0 != mbBuf && size > 0)
    {
        int index = sqlite3_bind_parameter_index(virtualMachine, mbBuf);
        status = sqlite3_bind_blob(virtualMachine, index, blob, blobSize, SQLITE_TRANSIENT);
        delete [] mbBuf;
        mbBuf = 0;
    }
    return status;
}


//Returns pointer to a database object
SqliteDB* SqliteVM :: GetDb()
{
    return db;
}

//Returns an error message
STRING SqliteVM :: GetErrMsg()
{
    return errMsg;
}

//Get the remaining (not executed) part of the query
STRING SqliteVM :: GetQueryTail()
{
    return queryTail;
}
//Returns the current SQL query
STRING SqliteVM :: GetSqlQuery()
{
    return sqlQuery;
}

INT32 SqliteVM::GetInt(CREFSTRING colName)
{
    INT8 colNum= ColumnNumber(colName);
    INT32 result = 0;

    if (colNum != -1)
    {
        result = sqlite3_column_int(virtualMachine, colNum);
    }
    else
    {
        if (!noThrow)
        {
            throw new ColumnNotFoundException(L"SqliteVM::GetInt(CREFSTRING colName)", __LINE__, __WFILE__, L"Column not found");
        }
    }

    return result;
}
STRING SqliteVM::GetString(CREFSTRING colName)
{
    INT8 colNum = ColumnNumber(colName);
    STRING result(L"");

    if (colNum != -1)
    {
        result = SqliteDB::MultiByteToWideChar((const char*)sqlite3_column_text(virtualMachine, colNum));
    }
    else
    {
        if (!noThrow)
        {
            throw new ColumnNotFoundException(L"SqliteVM::GetString(CREFSTRING colName)", __LINE__, __WFILE__, L"Column not found");
        }
    }
    return result;
}

double SqliteVM::GetDouble(CREFSTRING colName)
{
    INT8 colNum = ColumnNumber(colName);
    double result = 0;

    if (colNum != -1)
    {
        result = sqlite3_column_double(virtualMachine, colNum);
    }
    else
    {
        if (!noThrow)
        {
            throw new ColumnNotFoundException(L"SqliteVM::GetDouble(CREFSTRING colName)", __LINE__, __WFILE__, L"Column not found");
        }
    }
    return  result;
}

/// C++ only
SqliteBlob* SqliteVM::GetBlob(CREFSTRING colName)
{
    INT8 colNum = ColumnNumber(colName);
    BYTE_ARRAY_OUT result;
    INT32 length = 0;
    SqliteBlob* blob = NULL;

    if (colNum == -1)
    {
        SetErrMsg(L"Column does not exist");
        if (!noThrow)
        {
            throw new ColumnNotFoundException(L"SqliteVM::GetBlob(CREFSTRING colName, int& blobLen)", __LINE__, __WFILE__, L"Column not found");
        }
    }
    else
    {
        result =(unsigned char*)sqlite3_column_blob(virtualMachine, colNum);
        if (result != NULL)
        {
            length = sqlite3_column_bytes(virtualMachine, colNum);
            blob = new SqliteBlob(result, length);
        }
        else
        {
            blob = new SqliteBlob(NULL, 0);
        }
    }

    return blob;
}

//Returns the number of columns in the result
//Set to zero if last query did not return result
INT8 SqliteVM::NumCols()
{
    return pN;
}

//Set an error message
void SqliteVM :: SetErrMsg(CREFSTRING newMsg)
{
    errMsg = newMsg;
}

//Compiles an SQL  query
INT8 SqliteVM::SqlCompile()
{
    const char* tail = NULL;
    string query;
    INT8 status = SQLITE_OK;
    //get the current connection with the database
    sqlite3* connection = db->GetConnection();
    //Check if connection is valid
    if ( connection == NULL)
    {
        status = SQLITE_ERROR;
        SetErrMsg(L"No connection detected\n");
        throw new NoDbConnectionException(L"SqliteVM::SqlCompile()", __LINE__, __WFILE__, L"No database connection" );
    }
    else
    {
        query = SqliteDB::WideCharToMultiByte(sqlQuery.c_str());

        //Try to compile sql_query
        status = sqlite3_prepare(connection, query.c_str(), sqlQuery.length(), &virtualMachine, &tail);

        if (NULL != tail)
        {
            queryTail = SqliteDB::MultiByteToWideChar(tail);
        }
        if ( SQLITE_OK != status )
        {
            SetErrMsg(SqliteDB::MultiByteToWideChar(sqlite3_errmsg(db->GetConnection())));
            throw new SQLitePrepareException(L"SqliteVM::SqlCompile()", __LINE__, __WFILE__, L"Unable to execute");
        }
    }
    return status;
}
//Checks if the database object is still connected to a database
bool SqliteVM :: IsConnected()
{
    return (db->IsConnected());
}

//Get the next row from the result
INT8 SqliteVM ::NextRow()
{
    INT8 status;
    //Check if we have an existing virtual machine
    if (virtualMachine != NULL)
    {
        //If there is a virtual machine then get the next row from the result
        status = sqlite3_step(virtualMachine);
        if ( (SQLITE_DONE == status) || (SQLITE_ROW == status) )
        {
            pN = sqlite3_column_count(virtualMachine);
        }
        else
        {
            pN = 0;
            SetErrMsg(SqliteDB::MultiByteToWideChar(sqlite3_errmsg( db->GetConnection())));
            throw new ExecuteErrorException(L"SqliteVM ::NextRow()", __LINE__, __WFILE__, L"Unable execute query");
        }
    }
    //Set error message if no virtual machine exists
    else
    {
        SetErrMsg(L"No virtual machine detected");
        status = SQLITE_MISUSE;
        throw new ExecuteErrorException(L"SqliteVM ::NextRow()", __LINE__, __WFILE__, L"Unable to execute query");
    }
    return status;
}

//Get the next query in the case that more than one SQL queries were specified in the first call to Execute
INT8 SqliteVM::NextQuery()
{
    INT8 status = SQLITE_OK;

    if (L"" == queryTail)
    {
        status = SQLITE_MISUSE;
        SetErrMsg(L"There was no query to execute");
        throw new ExecuteErrorException(L"SqliteVM::NextQuery()", __LINE__, __WFILE__, L"Execute error");
    }
    else
    {
        status = SqliteVM::Execute(queryTail);
    }

    return status;
}

bool SqliteVM::NextQueryExists()
{
    return (L"" != queryTail);
}

//Delete the current virtual machine and reset the pointers to data results from the previous query
INT8 SqliteVM :: SqlFinalize()
{
    INT8 status = SQLITE_OK;
    if (virtualMachine != NULL)
    {
        status = sqlite3_finalize(virtualMachine);
        virtualMachine = NULL;
        if ( SQLITE_OK != status )
        {
            SetErrMsg(SqliteDB::MultiByteToWideChar(sqlite3_errmsg(db->GetConnection())));
            throw new SQLiteFinalizeException(L"SqliteVM :: SqlFinalize()", __LINE__, __WFILE__, L"Error finalizing virtual machine");
        }
    }
    return status;
}

INT32 SqliteVM::GetClassId()
{
    return m_cls_id;
}
