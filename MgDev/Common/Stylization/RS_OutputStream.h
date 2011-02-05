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

#ifndef RSOUTPUTSTREAM_H_
#define RSOUTPUTSTREAM_H_

class RS_OutputStream
{
public:
    //
    // Destructor
    //
    virtual ~RS_OutputStream() {}

    //
    // Flushes any remaining bytes from the stream
    //
    virtual void flush() = 0;

    //
    // Writes at most nBytesToWrite to the stream into the buffer
    // Returns the number of actual bytes read
    //
    virtual size_t write(const void* pBuffer, size_t nBytesToWrite) = 0;

protected:
    //
    // Default constructor
    //
    RS_OutputStream() {}

private:
    //
    // Unimplemented methods
    //
    RS_OutputStream( const RS_OutputStream& );
    RS_OutputStream& operator=( const RS_OutputStream& );
};

#endif
