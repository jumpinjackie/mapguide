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

#ifndef DWFRSINPUTSTREAM_H
#define DWFRSINPUTSTREAM_H

#include "Renderer.h"
#include "dwfcore/InputStream.h"

//DWFInputStream wrapper to an RS_InputStream
class DWFRSInputStream : public DWFInputStream
{
public:
    DWFRSInputStream(RS_InputStream* in)
    {
        m_in = in;
    }

    virtual ~DWFRSInputStream() throw()
    {
    }

    virtual size_t available() const throw()
    {
        return m_in->available();
    }

    virtual size_t read( void* pBuffer, size_t nBytesToRead ) throw()
    {
        return m_in->read(pBuffer, nBytesToRead);
    }

    virtual off_t seek( int eOrigin, off_t  nOffset ) throw()
    {
        return m_in->seek(eOrigin, nOffset);
    }

private:
    RS_InputStream* m_in;
};

#endif
