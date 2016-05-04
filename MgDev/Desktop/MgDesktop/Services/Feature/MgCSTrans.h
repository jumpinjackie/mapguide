#ifndef DESKTOP_CS_TRANS_H
#define DESKTOP_CS_TRANS_H

#include "MgDesktop.h"
#include "CSysTransformer.h"

class MgCoordinateSystem;
class MgCoordinateSystemTransform;

template class Ptr<MgCoordinateSystemTransform>;

class MG_DESKTOP_API MgdCSTrans : public CSysTransformer
{
public:
    MgdCSTrans(MgCoordinateSystem* srcCs, MgCoordinateSystem* dstCs);
    virtual ~MgdCSTrans();

    virtual void TransformPoint(double& x, double& y);
    virtual void TransformPoints(int numPts, double* x, double* y);
    virtual void TransformExtent(double& minX, double& minY, double& maxX, double& maxY);
    virtual double GetLinearScale();

private:
    Ptr<MgCoordinateSystemTransform> m_trans;
    double m_dLinearScale;
};

#endif