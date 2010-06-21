//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _CgiResponseStream_h
#define _CgiResponseStream_h

#include "Stream.h"
// Just want the banana...
// #include "Util/Byte.h"
// but gotta ask for the whole gorilla...
#include "MapGuideCommon.h"

/*
// A standalone (CGI) implementation of the Ogc framework
// could use this class to abstract the response stream.
class CCgiResponseStream: public CStream
{
public:
    void SetContentType(CPSZ pszContentTypeMime)
    {
      ASSERT(0);

        szprintf(_("content-type: "));
        szprintf(pszContentTypeMime);
        szprintf(_("\n\n"));

        // A little assistance: write the XML PI
        if(STR_EQ(pszContentTypeMime,_("text/xml")))
            szprintf(_("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
    }

    long Write(CPSZ pszBuffer,size_t uBytesToWrite,size_t* puBytesWritten)
    {
        szprintf(_("%.*s"),(uBytesToWrite/sizeof(CHARTYPE)),pszBuffer);
        *puBytesWritten = uBytesToWrite;
        return 0; // fake S_OK;
    }
};
*/

class MgHttpResponseStream: public CStream
{
public:
    MgHttpResponseStream()
    {
        m_pStore = new MgByte();
        m_pStream = new MgByteSource(m_pStore);
    }

    void SetContentType(CPSZ pszContentTypeMime)
    {
        m_pStream->SetMimeType(pszContentTypeMime);
    }

    long Write(CPSZ pszBuffer,size_t uBytesToWrite,size_t* puBytesWritten)
    {
        try {
            // In the outside world (outside of this class) strings are UTF-16 wide-chars
            // for consistency.  But the ByteSource backing this stream likes to use
            // UTF-8, so here on the frontier is where we do the conversion.
            //
            STRING sWide(pszBuffer,uBytesToWrite/sizeof(CHARTYPE)); // since WideCharTo... doesn't have CPSZ overload
            string sNarrow = MgUtil::WideCharToMultiByte(sWide);
            m_pStore->Append((BYTE_ARRAY_IN) sNarrow.c_str(), (INT32)sNarrow.length());
            //
            // vs just passing the stuff through in UTF-16:
            //m_pStore->Append((BYTE_ARRAY_IN) pszBuffer, (INT32)uBytesToWrite);
            if(puBytesWritten != NULL)
                *puBytesWritten = uBytesToWrite;
        }
        catch(...) {
            // It doesn't matter what happened, the write
            // was unsuccessful, and nothing was written.
            *puBytesWritten = 0;
        }

        return 0; // fake S_OK...
    };

    MgByteSource& Stream()
    {
        return *m_pStream;
    }

private:
    Ptr<MgByte>       m_pStore;
    Ptr<MgByteSource> m_pStream;
};

#endif//_CgiResponseStream_h
