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

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: BorderWalker.h.
//
// PURPOSE: Declaration of the BorderWalker class. The BorderWalker class
//          contains declarations and utility methods used in the construction
//          of point, polyline, and polygon buffer zones.
//
//------------------------------------------------------------------------------

#ifndef _BORDERWALKER_H_
#define _BORDERWALKER_H_

class BorderWalker
{
public:
    /// constructor/destructor

    BorderWalker();
    virtual ~BorderWalker();

    virtual void ScanAndWalk(const OpsFloatPoint *fpVerts, int nNumVerts,
        vector<OpsFloatPoint*> &vBufferZones, vector<int> &vBufferZonesCount) = 0;

    virtual void CheckBorderCrossingVertices(const OpsFloatPoint *fpVerts, int nNumVerts) = 0;

};

class BorderWalkerException
{
public:
    /// possible reasons for the exception

    enum ErrorCode {VerticesCrossingBorderError};

    /// constructor

    BorderWalkerException(ErrorCode errorCode) : m_errorCode(errorCode) {}

    /// method to determine the reason for the exception

    ErrorCode GetErrorCode() const {return m_errorCode;}

private:
    ErrorCode m_errorCode;
};

#endif // _BORDERWALKER_H_
