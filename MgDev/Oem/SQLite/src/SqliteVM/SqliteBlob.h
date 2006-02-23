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
