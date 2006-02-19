#include "SqliteWrap.h"

SqliteBlob::SqliteBlob(BYTE_ARRAY_IN newBuffer, INT32 lengthBuf):
buffer(NULL),
length(lengthBuf),
lastRead(0)
{
    if (newBuffer != NULL)
    {
        buffer = new char[lengthBuf];
        memcpy(buffer, newBuffer, lengthBuf);
    }
}

SqliteBlob::~SqliteBlob()
{
    delete [] buffer;
}

void SqliteBlob::Dispose()
{
    delete this;
}

INT32 SqliteBlob::Read(BYTE_ARRAY_OUT content, INT32 contentLength)
{
    int numBytes = contentLength;

    if (contentLength+lastRead >= length)
    {
        numBytes = length-lastRead;
    }

    if (numBytes > 0)
    {
        memcpy(content, &(buffer[lastRead]), numBytes);
        lastRead += numBytes;
    }

    return numBytes;
}

INT32 SqliteBlob::GetClassId()
{
    return m_cls_id;
}
