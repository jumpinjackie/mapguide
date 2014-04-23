#ifndef DESKTOP_TRANSACTION_H
#define DESKTOP_TRANSACTION_H

#include "MgDesktop.h"

class MgdFeatureConnection;
class FdoITransaction;
/// \ingroup Desktop_Misc_Module
/// \{
class MG_DESKTOP_API MgdTransaction : public MgTransaction
{
INTERNAL_API:
	MgdTransaction(MgdFeatureConnection* conn, MgResourceIdentifier* resource);
	virtual ~MgdTransaction();

    FdoITransaction* GetFdoTransaction();
    MgdFeatureConnection* GetConnection();
    virtual void Serialize(MgStream* stream);
    virtual void Deserialize(MgStream* stream);

PUBLISHED_API:
	
    virtual STRING AddSavePoint(CREFSTRING suggestName);

    virtual void ReleaseSavePoint(CREFSTRING savePointName);
    
    virtual void Rollback(CREFSTRING savePointName);

    virtual void Commit();

    virtual void Rollback();

    virtual MgResourceIdentifier* GetFeatureSource();

protected:
	virtual void Dispose() { delete this; }

private:
	MgResourceIdentifier* m_resource;
	FdoITransaction* m_trans;
    MgdFeatureConnection* m_conn;

};
/// \}
#endif