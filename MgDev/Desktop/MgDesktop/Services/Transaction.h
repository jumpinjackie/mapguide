#ifndef DESKTOP_TRANSACTION_H
#define DESKTOP_TRANSACTION_H

#include "MgDesktop.h"

class MgFeatureConnection;
class FdoITransaction;

class MG_DESKTOP_API MgdTransaction : public MgTransaction
{
INTERNAL_API:
	MgdTransaction(MgFeatureConnection* conn, MgResourceIdentifier* resource);
	virtual ~MgdTransaction();

    FdoITransaction* GetFdoTransaction();
    MgFeatureConnection* GetConnection();
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
    MgFeatureConnection* m_conn;

};

#endif