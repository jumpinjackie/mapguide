#include "MgCSTrans.h"
#include "CoordinateSystem/CoordinateSystem.h"

MgCSTrans::MgCSTrans(MgCoordinateSystem* srcCs, MgCoordinateSystem* dstCs)
{
    assert(srcCs);
    assert(dstCs);
    Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
    m_trans = csFactory->GetTransform(srcCs, dstCs);
    m_trans->IgnoreDatumShiftWarning(true);
    m_trans->IgnoreOutsideDomainWarning(true);

    m_dLinearScale = srcCs->ConvertCoordinateSystemUnitsToMeters(1.0) / dstCs->ConvertCoordinateSystemUnitsToMeters(1.0);
}


MgCSTrans::~MgCSTrans()
{
}


void MgCSTrans::TransformPoint(double& x, double& y)
{
    m_trans->Transform(&x, &y);
}


void MgCSTrans::TransformPoints(int numPts, double* x, double* y)
{
    m_trans->Transform(x, y, numPts);
}


void MgCSTrans::TransformExtent(double& minX, double& minY, double& maxX, double& maxY)
{
    MgEnvelope extent(minX, minY, maxX, maxY);
    Ptr<MgEnvelope> transExtent = m_trans->Transform(&extent);
    Ptr<MgCoordinate> ll = transExtent->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = transExtent->GetUpperRightCoordinate();

    minX = ll->GetX();
    minY = ll->GetY();
    maxX = ur->GetX();
    maxY = ur->GetY();
}


double MgCSTrans::GetLinearScale()
{
    return m_dLinearScale;
}
