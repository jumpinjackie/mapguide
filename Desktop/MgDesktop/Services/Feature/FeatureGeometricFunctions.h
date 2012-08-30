//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef MG_FEATURE_GEOMETRIC_FUNCTIONS_H_
#define MG_FEATURE_GEOMETRIC_FUNCTIONS_H_

class MgDisposable;
class FdoFunction;
class MgReader;
class MgdFeatureDistribution;
class MgGeometry;
class MgGeometryCollection;
/// \cond INTERNAL
class MgdFeatureGeometricFunctions : public MgdFeatureDistribution
{
    DECLARE_CLASSNAME(MgdFeatureGeometricFunctions)

public:

    MgdFeatureGeometricFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);
    virtual MgReader* Execute();

protected:

    MgdFeatureGeometricFunctions();
    virtual ~MgdFeatureGeometricFunctions();
    virtual void Dispose()
    {
        delete this;
    }

private:

    struct CoordinateCollection
    {
        std::vector<double> x_coords;
        std::vector<double> y_coords;
        std::vector<double> z_coords;
        std::vector<double> m_coords;
    };

    MgGeometry* GetValue();

    void Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);

    void ComputeExtents(MgCoordinate* lowerLeft, MgCoordinate* upperRight);
    MgGeometryCollection* ExecuteOperation();

    MgReader* GetReader(MgGeometryCollection* returnGeom);

	//WTF: The original method was void and threw MgInvalidPropertyTypeException if not supported
	//MSVC 2008 blows up on the linker phase with a combination of C1001 at this method and LNK1000
	//
	//Changing this method to return bool (supported = true) and throwing MgInvalidPropertyTypeException in
	//the method that calls this (Initialize()) makes the problem go away. Real WTF there!
    bool CheckSupportedPropertyType();

private:

    STRING m_propertyName;
    INT16 m_type;
    Ptr<MgReader> m_reader;
    FdoPtr<FdoFunction> m_customFunction;
    STRING m_propertyAlias;

    bool m_extentsInitialized;
    double m_minX;
    double m_minY;
    double m_minZ;
    double m_minM;
    double m_maxX;
    double m_maxY;
    double m_maxZ;
    double m_maxM;
};
/// \endcond
#endif
