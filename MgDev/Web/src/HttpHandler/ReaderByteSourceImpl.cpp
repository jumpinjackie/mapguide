//
//  Copyright (C) 2004-2013 by Autodesk, Inc.
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
#include "ReaderByteSourceImpl.h"
#include "PlatformBase.h"

MgReaderByteSourceImpl::MgReaderByteSourceImpl(MgReader* reader, CREFSTRING format)
{
    m_reader = SAFE_ADDREF(reader);
    m_format = format;
    m_buf.reserve(8192);
    m_bufOffset = -1;
    m_bReadHeader = false;
    m_bInternalReaderHasMore = true;
    m_bFirstRecord = true;
}

MgReaderByteSourceImpl::~MgReaderByteSourceImpl()
{
    m_buf.clear();
    MG_TRY()
    m_reader->Close();
    MG_CATCH_AND_RELEASE()
    m_reader = NULL;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Reads a buffer
///
/// \param buffer
/// A buffer receiving the data.
/// \param length
/// Maximum number of bytes to read
///
/// \return
/// Actual number of bytes put in the buffer. 0 means end of bytes
///
INT32 MgReaderByteSourceImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    INT32 ret = 0;

    bool bAdvanceReader = false;
    std::string buf;
    std::string jsonbuf;

    //Haven't read header, pre-fill this buffer
    if (!m_bReadHeader)
    {
        if (m_format == MgMimeType::Json)
        {
            //How this looks:
            //
            // {                            //outer JSON start
            //   "ResponseElementName":     //root element name
            //     {                        //root JSON property start
            //      <header JSON pair>,
            //      "BodyElementName":[     //body JSON array start
            //
            jsonbuf = "{\"";
            jsonbuf += m_reader->GetResponseElementName();
            jsonbuf += "\":{";
            m_reader->HeaderToStringUtf8(buf);
            std::string jsonbuf2;
            MgXmlJsonConvert convert;
            convert.ToJson(buf, jsonbuf2);
            //This will have redudant outer { }, so strip them
            jsonbuf2.erase(0, 1);
            jsonbuf2.erase(jsonbuf2.length() - 2, 1);
            //HACK: To match the original output, we have to array-ify this object (crazy? yes!)
            //
            //We currently have something like this
            //
            // "HeaderElementName":{
            //    <prop1>:<val1>,
            //    <prop2>:<val2>
            // }
            //
            //We have to change it to this
            //
            // "HeaderElementName":[{
            //    <prop1>:<val1>,
            //    <prop2>:<val2>
            // }]

            //Find first instance of ": and insert [ after it. We use ": because a feature
            //reader puts out xs:schema as the header element name
            jsonbuf2.insert(jsonbuf2.find("\":") + 2, "[");
            //Append ] to the end
            jsonbuf2.append("]");
            jsonbuf += jsonbuf2;
            jsonbuf += ",\"";
            jsonbuf += m_reader->GetBodyElementName();
            jsonbuf += "\":[";

            m_buf += jsonbuf;
            m_bReadHeader = true;
        }
        else if (m_format == MgMimeType::Xml)
        {
            m_reader->ResponseStartUtf8(m_buf);
#ifdef _DEBUG
            m_buf += "<!-- BEGIN HEADER -->\n";
#endif
            m_reader->HeaderToStringUtf8(m_buf);
#ifdef _DEBUG
            m_buf += "<!-- END HEADER -->\n";
            m_buf += "<!-- BEGIN BODY -->\n";
#endif
            m_reader->BodyStartUtf8(m_buf);
            m_bReadHeader = true;
        }
    }

    INT32 maxIndex = m_buf.length() - 1;
    //We have an internal buffer. Clear this out first
    if (m_bufOffset < maxIndex)
    {
        INT32 read = ReadInternalBuffer(buffer, ret, length);
        ret += read;
    }

    //If we've filled the whole internal buffer and the requested length is still
    //bigger, we're good to advance to the next feature/record
    if (ret < length)
        bAdvanceReader = true;
    else //Filled up to requested length
        return ret;

    if (m_bInternalReaderHasMore && bAdvanceReader)
    {
        m_bInternalReaderHasMore = m_reader->ReadNext();
        if (m_bInternalReaderHasMore)
        {
            if (m_format == MgMimeType::Json)
            {
                m_reader->CurrentToStringUtf8(buf);
                //The body is a valid full XML element, so no need for gymnastics like its
                //surrounding elements
                MgXmlJsonConvert convert;
                convert.ToJson(buf, jsonbuf);

                //Strip outer { }
                jsonbuf.erase(0, 1);
                jsonbuf.erase(jsonbuf.length() - 2, 1);
                //HACK: Same as the header, this needs to be array-ified to match the old output
                //
                //Find first instance of ": and insert [ after it.
                jsonbuf.insert(jsonbuf.find("\":") + 2, "[");
                //Append ] to the end
                jsonbuf.append("]");
                //Put back in outer { }
                jsonbuf = "{" + jsonbuf;
                jsonbuf += "}";

                if (!m_bFirstRecord)
                {
                    m_buf += ",";
                }
                else
                {
                    m_bFirstRecord = false;
                }

                m_buf += jsonbuf;
            }
            else if (m_format == MgMimeType::Xml)
            {
#ifdef _DEBUG
                m_buf += "<!-- BEGIN RECORD -->\n";
#endif
                m_reader->CurrentToStringUtf8(m_buf);
#ifdef _DEBUG
                m_buf += "<!-- END RECORD -->\n";
#endif
            }
        }
        else //End of reader
        {
            if (m_format == MgMimeType::Json)
            {
                // How this looks:
                //      ]   //End of body JSON array
                //    }     //End of root JSON property
                // }        //End of outer JSON
                m_buf += "]}}";
            }
            else if (m_format == MgMimeType::Xml)
            {
                m_reader->BodyEndUtf8(m_buf);
#ifdef _DEBUG
                m_buf += "<!-- END BODY -->\n";
#endif
                m_reader->ResponseEndUtf8(m_buf);
            }
        }
    }

    //Clear out however many remaining content up to the requested
    //length
    maxIndex = m_buf.length() - 1;
    if (m_bufOffset < maxIndex)
    {
        INT32 remaining = length - ret;
        if (remaining > 0)
        {
            INT32 read = ReadInternalBuffer(buffer, ret, remaining);
            ret += read;
        }
    }

    return ret;
}

INT32 MgReaderByteSourceImpl::ReadInternalBuffer(BYTE_ARRAY_OUT buffer, INT32 fromIndex, INT32 length)
{
    INT32 ret = 0;
    while (ret < length)
    {
        m_bufOffset++;
        INT32 maxIndex = m_buf.length() - 1;
        if (m_bufOffset <= maxIndex)
        {
            buffer[fromIndex + ret] = m_buf[m_bufOffset];
            ret++;
        }
        else //End of internal buffer. Reset offset
        {
            m_buf.clear();
            m_bufOffset = -1;
            break;
        }
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the remaining length of the byte source.  This length is
/// adjusted for previous reads.  If the returned length is zero
/// then the underlying source may be a streaming format and the length
/// is not known.
///
/// \return
/// Remaining length of the byte source
///
INT64 MgReaderByteSourceImpl::GetLength()
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Determines if the source is rewindable.
///
/// \return
/// true if the source is rewindable, false otherwise.
///
bool MgReaderByteSourceImpl::IsRewindable()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Rewinds to the start of the source.  Depending on the source of
/// the reader, Rewind may not be supported.  Readers sourced from
/// true streams cannot be rewound.
///
/// \return
/// Nothing
///
/// \exception MgInvalidOperationException if source cannot be rewound
///
void MgReaderByteSourceImpl::Rewind()
{
    throw new MgInvalidOperationException(L"MgReaderByteSourceImpl.Rewind", __LINE__, __WFILE__, NULL, L"", NULL);
}