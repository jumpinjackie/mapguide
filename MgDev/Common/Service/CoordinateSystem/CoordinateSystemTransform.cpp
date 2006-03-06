//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "CoordinateSystemCommon.h"

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Initializes a new instance of MgCoordinateSystemTransform given source
/// and target coordinate system objects. If the source and target
/// coordinate systems are not compatible (e.g. transformations cannot be
/// performed) an exception will be thrown.
///</summary>
///<param name="source">
/// An MgCoordinateSystem that defines the coordinate system of the source
/// coordiantes.
///</param>
///<param name="target">
/// An MgCoordinateSystem that defines the coordinate system for the target
/// coordiantes.
///</param>
MgCoordinateSystemTransform::MgCoordinateSystemTransform(MgCoordinateSystem* source, MgCoordinateSystem* target) :
    m_coordSysTransform(NULL)
{
    ACE_ASSERT(source);
    ACE_ASSERT(target);

    if((NULL == source) || (NULL == target))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.MgCoordinateSystemTransform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_coordSysSource = SAFE_ADDREF(source);
    m_coordSysTarget = SAFE_ADDREF(target);

    m_coordSysTransform = new CCoordinateSystemTransform(source->GetInternalCoordinateSystem(), target->GetInternalCoordinateSystem());
    if(NULL == m_coordSysTransform)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.MgCoordinateSystemTransform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
MgCoordinateSystemTransform::~MgCoordinateSystemTransform()
{
    // Free resources
    if(m_coordSysTransform)
    {
        delete m_coordSysTransform;
        m_coordSysTransform = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X and Y ordinates and returns a new coordinate.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<returns>
/// A new MgCoordinate defined by the transformed X and Y ordinates.
///</returns>
MgCoordinate* MgCoordinateSystemTransform::Transform(double x, double y)
{
    MgCoordinate* pCoordinate = NULL;

    try
    {
        m_coordSysTransform->Transform(x, y);
        pCoordinate = new MgCoordinateXY(x, y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X and Y ordinates.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::Transform(double* x, double* y)
{
    if((NULL == x) || (NULL == y))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->Transform(*x, *y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the array of X and Y ordinates.
///</summary>
///<param name="x">
/// The X ordinates to transform.
///</param>
///<param name="y">
/// The Y ordinates to transform.
///</param>
///<param name="arraySize">
/// The size of the arrays. All the arrays must be the same size.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::Transform(double x[], double y[], int arraySize)
{
    if((NULL == x) || (NULL == y))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->Transform(x, y, arraySize);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X, Y ordinates and the measure.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<param name="m">
/// The measure to transform.
///</param>
///<returns>
/// A new MgCoordinate defined by the transformed X,Y ordinates and measure.
///</returns>
MgCoordinate* MgCoordinateSystemTransform::TransformM(double x, double y, double m)
{
    MgCoordinate* pCoordinate = NULL;

    try
    {
        m_coordSysTransform->TransformM(x, y, m);
        pCoordinate = new MgCoordinateXYM(x, y, m);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X, Y ordinates and the measure.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<param name="m">
/// The measure to transform.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::TransformM(double* x, double* y, double* m)
{
    if((NULL == x) || (NULL == y) || (NULL == m))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->TransformM(*x, *y, *m);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the array of X, Y ordinates and the measure.
///</summary>
///<param name="x">
/// The X ordinates to transform.
///</param>
///<param name="y">
/// The Y ordinates to transform.
///</param>
///<param name="m">
/// The measures to transform.
///</param>
///<param name="arraySize">
/// The size of the arrays. All the arrays must be the same size.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::TransformM(double x[], double y[], double m[], int arraySize)
{
    if((NULL == x) || (NULL == y) || (NULL == m))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->TransformM(x, y, m, arraySize);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X, Y, and Z ordinates and returns a new coordinate.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<param name="z">
/// The Z ordinate to transform.
///</param>
///<returns>
/// A new MgCoordinate defined by the transformed X, Y, and Z ordinates.
///</returns>
MgCoordinate* MgCoordinateSystemTransform::Transform(double x, double y, double z)
{
    MgCoordinate* pCoordinate = NULL;

    try
    {
        m_coordSysTransform->Transform(x, y, z);
        pCoordinate = new MgCoordinateXYZ(x, y, z);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X, Y, and Z ordinates.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<param name="z">
/// The Z ordinate to transform.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::Transform(double* x, double* y, double* z)
{
    if((NULL == x) || (NULL == y) || (NULL == z))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->Transform(*x, *y, *z);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the array of X, Y, and Z ordinates.
///</summary>
///<param name="x">
/// The X ordinates to transform.
///</param>
///<param name="y">
/// The Y ordinates to transform.
///</param>
///<param name="z">
/// The Z ordinates to transform.
///</param>
///<param name="arraySize">
/// The size of the arrays. All the arrays must be the same size.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::Transform(double x[], double y[], double z[], int arraySize)
{
    if((NULL == x) || (NULL == y) || (NULL == z))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->Transform(x, y, z, arraySize);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X, Y, Z ordinates and the measure.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<param name="z">
/// The Z ordinate to transform.
///</param>
///<param name="m">
/// The measure to transform.
///</param>
///<returns>
/// A new MgCoordinate defined by the transformed X, Y, Z ordinates and measure.
///</returns>
MgCoordinate* MgCoordinateSystemTransform::TransformM(double x, double y, double z, double m)
{
    MgCoordinate* pCoordinate = NULL;

    try
    {
        m_coordSysTransform->TransformM(x, y, z, m);
        pCoordinate = new MgCoordinateXYZM(x, y, z, m);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X, Y, Z ordinates and the measure.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<param name="z">
/// The Z ordinate to transform.
///</param>
///<param name="m">
/// The measure to transform.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::TransformM(double* x, double* y, double* z, double* m)
{
    if((NULL == x) || (NULL == y) || (NULL == z) || (NULL == m))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->TransformM(*x, *y, *z, *m);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the array of X, Y, Z ordinates and the measure.
///</summary>
///<param name="x">
/// The X ordinates to transform.
///</param>
///<param name="y">
/// The Y ordinates to transform.
///</param>
///<param name="z">
/// The Z ordinates to transform.
///</param>
///<param name="m">
/// The measures to transform.
///</param>
///<param name="arraySize">
/// The size of the arrays. All the arrays must be the same size.
///</param>
///<returns>
/// Nothing.
///</returns>
void MgCoordinateSystemTransform::TransformM(double x[], double y[], double z[], double m[], int arraySize)
{
    if((NULL == x) || (NULL == y) || (NULL == z) || (NULL == m))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        m_coordSysTransform->TransformM(x, y, z, m, arraySize);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the specified source coordinate and returns a new coordinate
/// if the target coordinate is NULL, otherwise updates and returns the
/// target coordinate with the values of the transformed source coordinate.
///</summary>
///<param name="coordinate">
/// The input MgCoordinate to transform.
///</param>
///<returns>
/// A new MgCoordinate transformed from the specified coordinate.
///</returns>
MgCoordinate* MgCoordinateSystemTransform::Transform(MgCoordinate* coordinate)
{
    MgCoordinate* pCoordinate = NULL;

    if(NULL == coordinate)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Need to determine type of coordinate to transform
    if(coordinate->GetDimension() == MgCoordinateDimension::XY)
    {
        // 2D transform
        pCoordinate = Transform(coordinate->GetX(), coordinate->GetY());
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        // 2D transform + measure
        pCoordinate = TransformM(coordinate->GetX(), coordinate->GetY(), coordinate->GetM());
    }
    else if(coordinate->GetDimension() == MgCoordinateDimension::XYZ)
    {
        // 3D transform
        pCoordinate = Transform(coordinate->GetX(), coordinate->GetY(), coordinate->GetZ());
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        // 3D transform + measure
        pCoordinate = TransformM(coordinate->GetX(), coordinate->GetY(), coordinate->GetZ(), coordinate->GetM());
    }
    else
    {
        // What dimension is this?
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the specified source envelope and returns a new envelope
/// if the target envelope is NULL, otherwise updates and returns the
/// target envelope with the values of the transformed source envelope.
///</summary>
///<param name="envelope">
/// The input MgEnvelope to transform.
///</param>
///<returns>
/// A new MgEnvelope transformed from the specified envelope.
///</returns>
MgEnvelope* MgCoordinateSystemTransform::Transform(MgEnvelope* envelope)
{
    MgEnvelope* pEnvelope = NULL;

    if(NULL == envelope)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    CEnvelope* e2 = NULL;

    try
    {
        Ptr<MgCoordinate> lowerLeft = envelope->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> upperRight = envelope->GetUpperRightCoordinate();

        if (lowerLeft->GetDimension() == MgCoordinateDimension::XY
            && upperRight->GetDimension() == MgCoordinateDimension::XY)
        {
            //2D transform

            CEnvelope e1(lowerLeft->GetX(), lowerLeft->GetY(),
                         upperRight->GetX(), upperRight->GetY());
            
            //call the internal envelope transform
            e2 = m_coordSysTransform->Transform(&e1);

            lowerLeft = new MgCoordinateXY(e2->GetLowerLeftXCoordinate(),
                                            e2->GetLowerLeftYCoordinate());

            upperRight = new MgCoordinateXY(e2->GetUpperRightXCoordinate(),
                                            e2->GetUpperRightYCoordinate());
        }
        else
        {
            //3D transform

            CEnvelope e1(lowerLeft->GetX(), lowerLeft->GetY(), lowerLeft->GetZ(),
                         upperRight->GetX(), upperRight->GetY(), upperRight->GetZ());

            //call the internal envelope transform
            CEnvelope* e2 = m_coordSysTransform->Transform(&e1);

            lowerLeft = new MgCoordinateXYZ(e2->GetLowerLeftXCoordinate(),
                                            e2->GetLowerLeftYCoordinate(),
                                            e2->GetLowerLeftZCoordinate());

            upperRight = new MgCoordinateXYZ(e2->GetUpperRightXCoordinate(),
                                            e2->GetUpperRightYCoordinate(),
                                            e2->GetUpperRightZCoordinate());
        }

        delete e2;
        e2 = NULL;

        pEnvelope = new MgEnvelope(lowerLeft, upperRight);
    }
    catch(MgException* e)
    {
        if(e2)
        {
            delete e2;
            e2 = NULL;
        }

        delete e;
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    catch(...)
    {
        if(e2)
        {
            delete e2;
            e2 = NULL;
        }

        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pEnvelope;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgCoordinateSystemTransform::GetClassId()
{
    return m_cls_id;
}

// <summary>Dispose this object.</summary>
// <returns>Returns nothing</returns>
void MgCoordinateSystemTransform::Dispose()
{
    delete this;
}
