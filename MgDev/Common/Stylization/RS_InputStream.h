//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef RSINPUTSTREAM_H_
#define RSINPUTSTREAM_H_

#include "sys/types.h" //for off_t

//Simple input stream interface to wrap external incoming data streams
//like DWF files, images, etc.
class RS_InputStream
{
public:
    //
    // Destructor
    //
    virtual ~RS_InputStream() {}

    //
    // Returns the number of bytes available to read from stream
    //
    virtual size_t available() const = 0;

    //
    // Reads at most nBytesToRead from the stream into the buffer
    // Returns the number of actual bytes read
    //
    virtual size_t read(void* pBuffer, size_t nBytesToRead) = 0;

    //
    // Repositions the stream pointer
    // Returns the previous offset before the seek
    //
    virtual off_t seek(int eOrigin, off_t nOffset) = 0;

protected:
    //
    // Default constructor
    //
    RS_InputStream() {}

private:
    //
    // Unimplemented methods
    //

    RS_InputStream( const RS_InputStream& );
    RS_InputStream& operator=( const RS_InputStream& );
};

#endif
