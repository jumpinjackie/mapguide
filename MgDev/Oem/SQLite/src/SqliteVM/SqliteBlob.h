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

#ifndef SQLITE_BLOB_H
#define SQLITE_BLOB_H

class SqliteBlob : public SqliteDisposable
{

EXTERNAL_API:

    ///<summary>Destructor. Deletes the blob object</summary>
    ///<returns>Nothing</returns>
    ~SqliteBlob();

    // <summary>Dispose this object.</summary>
    // <returns>Returns nothing</returns>
    virtual void Dispose();

    ///<summary>Read the blob. Put contentLength number of bytes into content</summary>
    ///<returns>Number of bytes read</returns>
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length);

    //Return the class id
    INT32 GetClassId();

INTERNAL_API:
    SqliteBlob(BYTE_ARRAY_IN newBuffer, INT32 lengthBuf);

CLASS_ID:
    static const INT32 m_cls_id = SQLITE_SqliteBlob;

private:
    char* buffer;
    int length;
    int lastRead;

};

#endif
