#ifndef DESKTOP_SELECTION_H
#define DESKTOP_SELECTION_H

#include "MgDesktop.h"
#include "Map.h"

/// \ingroup Desktop_Map_and_Layers_Module
/// \{
class MG_DESKTOP_API MgdSelection : public MgSelectionBase
{
	MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgdSelection)

PUBLISHED_API: 
    MgdSelection(MgdMap* map);

    MgdSelection(MgdMap* map, CREFSTRING xmlSelectionString);

    MgFeatureReader* GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, bool mappedOnly);

    MgFeatureReader* GetSelectedFeatures(MgLayerBase* layer, CREFSTRING className, MgStringCollection* propertyNames);

    void Clear();

    MgdSelection();

    virtual ~MgdSelection();

public:
    virtual void Open(MgResourceService* resourceService, CREFSTRING mapName);

    void Save(MgResourceService* resourceService, CREFSTRING mapName);

protected:

	void Dispose() { delete this; }

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    virtual const char* GetResourceTypeName();

    STRING GetResourceName(CREFSTRING sessionId, CREFSTRING mapName);

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_MapLayer_Selection;
};
/// \}
#endif