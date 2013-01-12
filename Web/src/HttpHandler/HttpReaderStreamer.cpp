#include "HttpHandler.h"
#include "XmlJsonConvert.h"

MgHttpReaderStreamer::MgHttpReaderStreamer(MgReader* reader, CREFSTRING format)
{
    m_reader = SAFE_ADDREF(reader);
    m_format = format;
}

MgHttpReaderStreamer::~MgHttpReaderStreamer() 
{
    m_reader = NULL;
}

void MgHttpReaderStreamer::SetChunkedEncoding() 
{

}

void MgHttpReaderStreamer::WriteChunk(const char* str, size_t length)
{

}

void MgHttpReaderStreamer::EndStream() { }

void MgHttpReaderStreamer::StreamResult()
{
    MG_TRY()

    SetChunkedEncoding();

    if (m_format == MgMimeType::Json)
    {
        std::string buf;
        std::string jsonbuf;

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
        ToJson(buf, jsonbuf2);
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

        WriteChunk(jsonbuf.c_str(), jsonbuf.length());

        bool bNext = m_reader->ReadNext();
        while(bNext)
        {
            buf.clear();
            jsonbuf.clear();
            m_reader->CurrentToStringUtf8(buf);
            //The body is a valid full XML element, so no need for gymnastics like its
            //surrounding elements
            ToJson(buf, jsonbuf);

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

            bNext = m_reader->ReadNext();
            if (bNext)
                jsonbuf += ",";
            WriteChunk(jsonbuf.c_str(), jsonbuf.length());
        }

        buf.clear();
        jsonbuf.clear();

        // How this looks:
        //      ]   //End of body JSON array
        //    }     //End of root JSON property
        // }        //End of outer JSON
        jsonbuf = "]}}";
        WriteChunk(jsonbuf.c_str(), jsonbuf.length());
    }
    else if (m_format == MgMimeType::Xml)
    {
        std::string buf;
        m_reader->ResponseStartUtf8(buf);
        m_reader->HeaderToStringUtf8(buf);
        m_reader->BodyStartUtf8(buf);

        WriteChunk(buf.c_str(), buf.length());

        while(m_reader->ReadNext())
        {
            buf.clear();
            m_reader->CurrentToStringUtf8(buf);
            WriteChunk(buf.c_str(), buf.length());
        }

        buf.clear();

        m_reader->BodyEndUtf8(buf);
        m_reader->ResponseEndUtf8(buf);

        WriteChunk(buf.c_str(), buf.length());
    }

    MG_CATCH_AND_THROW(L"MgHttpReaderStreamer.StreamResult");
}

void MgHttpReaderStreamer::ToJson(string& xmlString, string& jsonString)
{
    MgXmlJsonConvert convert;
    convert.ToJson(xmlString, jsonString);
}