#ifndef DESKTOP_SCROLLABLE_FEATURE_READER_H
#define DESKTOP_SCROLLABLE_FEATURE_READER_H

#include "Fdo.h"
#include "MgDesktop.h"

class MgFeatureConnection;

// The MgdScrollableFeatureReader is an extension to the MgFeatureReader 
// that can be traversed in the forward and backward direction (scrollable)

class MG_DESKTOP_API MgdScrollableFeatureReader : public MgdFeatureReader
{
INTERNAL_API:
	MgdScrollableFeatureReader(MgFeatureConnection* conn, FdoIScrollableFeatureReader* reader);

EXTERNAL_API:
	virtual ~MgdScrollableFeatureReader();

	//MgReader
PUBLISHED_API:
    // --------- Scrollable API ----------- //
    virtual INT32 Count();

    virtual bool ReadFirst();

    virtual bool ReadLast();

    virtual bool ReadPrevious();

    virtual bool ReadAt(MgPropertyCollection* key);

    virtual bool ReadAtIndex(UINT32 recordIndex);

    virtual UINT32 IndexOf(MgPropertyCollection* key);

protected:
	virtual void Dispose() { delete this; }

INTERNAL_API:
    FdoIScrollableFeatureReader* GetInternalReader() { return FDO_SAFE_ADDREF(m_scrollReader); }

private:
	FdoIScrollableFeatureReader* m_scrollReader;
};

#endif