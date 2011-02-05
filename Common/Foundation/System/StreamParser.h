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

#ifndef MGSTREAMPARSER_H_MG
#define MGSTREAMPARSER_H_MG

class MgStreamData;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The MgStreamParser class wraps static methods that allow parsing of
//  stream structures.
class MG_FOUNDATION_API MgStreamParser
{
    DECLARE_CLASSNAME(MgStreamParser)

    ///////////////////////////////////////////////////////////////////////
    ///  Constructor/Destructor
    protected :
        MgStreamParser( void );

    ///////////////////////////////////////////////////////////////////////
    ///  Enumerations
    public :
        enum MgStreamHeader
        {
            mshStreamStart   =   0x1111F801,
            mshStreamData    =   0x1111F802,
            mshStreamEnd     =   0x1111F803
        };

        // Stream version follows a 16 bit major, 16 bit minor
        // convention.  Minors should be interop compatible.
        // Major versions are not.
        static const UINT32 StreamVersion = (3<<16) + 0;

    ///////////////////////////////////////////////////////////////////////
    ///  Methods
    public :
        static bool ParseStreamHeader( MgStreamData* pStreamData );
        static bool ParseDataHeader( MgStreamData* pStreamData );
        static bool ParseEndHeader( MgStreamData* pStreamData );

        static bool WriteStreamHeader( MgStreamData* pStreamData );
        static bool WriteEndHeader( MgStreamData* pStreamData );
};
/// \endcond

#endif  //  MGSTREAMPARSER_H_MG
