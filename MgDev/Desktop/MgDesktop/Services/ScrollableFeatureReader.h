#ifndef DESKTOP_SCROLLABLE_FEATURE_READER_H
#define DESKTOP_SCROLLABLE_FEATURE_READER_H

#include "MgDesktop.h"

class FdoIScrollableFeatureReader;
class MgdFeatureConnection;

// The MgdScrollableFeatureReader is an extension to the MgFeatureReader 
// that can be traversed in the forward and backward direction (scrollable)
/// \ingroup Desktop_Misc_Module
/// \{
class MG_DESKTOP_API MgdScrollableFeatureReader : public MgdFeatureReader
{
INTERNAL_API:
	MgdScrollableFeatureReader(MgdFeatureConnection* conn, FdoIScrollableFeatureReader* reader);

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
    FdoIScrollableFeatureReader* GetInternalReader();

private:
	FdoIScrollableFeatureReader* m_scrollReader;
};
/// \}
#endif