#include "MapGuideCommon.h"
#include "HttpHandler.h"
#include "IsapiReaderStreamer.h"
#include "MapAgentStrings.h"

IsapiReaderStreamer::IsapiReaderStreamer(EXTENSION_CONTROL_BLOCK* rec, const std::string& sResponseHeader, MgReader* reader, CREFSTRING format) :
    MgHttpReaderStreamer(reader, format), 
    m_pECB(rec), 
    m_sResponseHeader(sResponseHeader), 
    m_bEndOfStream(false)
{
    m_hexLen.reserve(32);
}

IsapiReaderStreamer::~IsapiReaderStreamer() 
{
    EndStream();
}

void IsapiReaderStreamer::EndStream()
{
    if (!m_bEndOfStream)
    {
        //Write end of chunked response
        DWORD len = 5;
        m_pECB->WriteClient(m_pECB->ConnID, "0\r\n\r\n", &len, 0);

        m_bEndOfStream = true;
    }
}

void IsapiReaderStreamer::SetChunkedEncoding() 
{
    m_sResponseHeader.append("Connection: Keep-Alive\r\n");
    m_sResponseHeader.append("Transfer-Encoding: chunked\r\n");
    m_sResponseHeader.append("\r\n");

    DWORD dwHeaderSize = (DWORD)m_sResponseHeader.length();
    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, NULL, &dwHeaderSize, (LPDWORD)m_sResponseHeader.c_str());
}

void IsapiReaderStreamer::WriteChunk(const char* str, size_t length)
{
    m_hexLen.clear();
    MgUtil::Int32ToHexString(length, m_hexLen);
    m_hexLen.append(MapAgentStrings::CrLf);

    DWORD len = m_hexLen.size();
    m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)m_hexLen.data(), &len, 0);

    len = (DWORD)length;
    m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)str, &len, 0);

    len = 2;
    m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)MapAgentStrings::CrLf, &len, 0);
}