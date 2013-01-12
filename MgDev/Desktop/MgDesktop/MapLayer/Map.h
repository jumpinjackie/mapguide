#ifndef DESKTOP_MAP_H
#define DESKTOP_MAP_H

#include "MgDesktop.h"
#include "MdfModel.h"
#include <vector>
#include <list>
#include <map>

typedef std::list<MdfModel::MdfString> ColorStringList;

class MgdServiceFactory;

/// \ingroup Desktop_Map_and_Layers_Module
/// \{
class MG_DESKTOP_API MgdMap : public MgMapBase
{
	MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgdMap)

PUBLISHED_API:

    enum WatermarkUsageType
    {
        WMS = 1,
        Viewer = 2
    };

    //MgdMap();

    MgdMap(MgResourceIdentifier* mapDefinition);

    MgdMap(MgResourceIdentifier* mapDefinition, CREFSTRING mapName);

    virtual void Create(MgResourceService* resourceService, MgResourceIdentifier* mapDefinition, CREFSTRING mapName);

    virtual void Create(MgResourceIdentifier* mapDefinition, CREFSTRING mapName);

    virtual void Create(CREFSTRING mapSRS, MgEnvelope* mapExtent, CREFSTRING mapName);

    virtual void Open(MgResourceService* resourceService, CREFSTRING mapName);

    virtual void Open(CREFSTRING mapName);

    void Save(MgResourceService* resourceService);

    void Save(MgResourceService* resourceService, MgResourceIdentifier* resourceId);

    void Save();

    void SetDisplaySize(INT32 width, INT32 height);

    void SetDpi(INT32 dpi);

    virtual void SetViewScale(double scale);

    void SetViewCenterXY(double x, double y);

    void GotoView(double x, double y, double scale);

    INT32 GetWatermarkUsage();

    double GetMetersPerUnit();

EXTERNAL_API:
    void MakeGroupNormal(MgLayerGroup* group);

    static void SetGroupExpandInLegend(MgLayerGroup* group, bool bExpand);

    static void SetLayerExpandInLegend(MgLayerBase* layer, bool bExpand);

    static void SetLayerGroupType(MgLayerGroup* group, INT32 type);

INTERNAL_API:
    MgdMap();

    virtual ~MgdMap();

    virtual void Serialize(MgStream* stream);

    virtual void Deserialize(MgStream* stream);

    virtual MgService* GetService(INT32 serviceType);

    void SetDelayedLoadResourceService(MgResourceService* resourceService);

    virtual void OnLayerParentChanged(MgLayerBase* layer, CREFSTRING parentId);

    ColorStringList& GetColorPalette();
    void AddColorsToPalette(ColorStringList& newColorPalette);

    void BulkLoadIdentityProperties(MgFeatureService* featureService);

    virtual void SetWatermarkUsage(INT32 watermarkUsage);

protected:

    virtual void UnpackLayersAndGroups();

    virtual MgMemoryStreamHelper* PackLayersAndGroups();

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

	virtual void Dispose() { delete this; }

private:

    void InitializeResourceService(MgResourceService* resourceService);

CLASS_ID:
	static const INT32 m_cls_id = MapGuide_Desktop_MapLayer_Map;

private:

    // Version for serialization
    static const int m_serializeVersion = (4<<16) + 0;
    static STRING m_layerGroupTag;

    Ptr<MgdServiceFactory> m_factory;
    Ptr<MgMemoryStreamHelper> m_layerGroupHelper;
    Ptr<MgResourceService> m_resourceService;
    bool m_inSave;
    bool m_unpackedLayersGroups;
    ColorStringList* m_colorPalette;

    INT32 m_watermarkUsage;
};
/// \}
#endif