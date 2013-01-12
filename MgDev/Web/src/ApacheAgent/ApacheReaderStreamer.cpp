#include "ApacheReaderStreamer.h"
#include "http_protocol.h"

ApacheReaderStreamer::ApacheReaderStreamer(request_rec* rec, MgReader* reader, CREFSTRING format) :
    MgHttpReaderStreamer(reader, format), m_r(rec)
{
}

ApacheReaderStreamer::~ApacheReaderStreamer() { }

void ApacheReaderStreamer::SetChunkedEncoding() 
{
    //Setting chunked to 1 enables chunked transfer encoding. No need to manually
    //write out chunk delimiters and fragments
    m_r->chunked = 1; 
}

void ApacheReaderStreamer::WriteChunk(const char* str, size_t length)
{
    //Each ap_rwrite call is considered an individual chunk when m_r->chunked is set to 1
    ap_rwrite(str, length, m_r);
}