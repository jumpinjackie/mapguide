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

#ifndef MGPACKETPARSER_H_MG
#define MGPACKETPARSER_H_MG


/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The PacketParser class wraps static methods that allow parsing of
//  packet structures and other information.
class MG_FOUNDATION_API MgPacketParser
{
    DECLARE_CLASSNAME(MgPacketParser)

    ///////////////////////////////////////////////////////////////////////
    ///  Constructor
    protected :
        MgPacketParser( void );

    ///////////////////////////////////////////////////////////////////////
    ///  Enumerations
    public :

        enum MgPacketHeader
        {
            mphUnknown                   =   0x1111FF01,
            mphOperation                 =   0x1111FF02,
            mphOperationResponse         =   0x1111FF03,
            mphArgumentSimple            =   0x1111FF04,
            mphArgumentCollection        =   0x1111FF05,
            mphArgumentBinaryStream      =   0x1111FF06,
            mphControl                   =   0x1111FF07,
            mphError                     =   0x1111FF08,
            mphEND                       =   0x1111FF09
        };

        enum MgServiceID
        {
            msiUnknown           =   0x1111FE01,
            msiDrawing           =   0x1111FE02,
            msiFeature           =   0x1111FE03,
            msiMapping           =   0x1111FE04,
            msiRendering         =   0x1111FE05,
            msiResource          =   0x1111FE06,
            msiServerAdmin       =   0x1111FE07,
            msiSite              =   0x1111FE08,
            msiTile              =   0x1111FE09,
            msiKml               =   0x1111FE0A,
            msiProfiling         =   0x1111FE0B,
            msiEND               =   0x1111FE0C

        };

        enum MgECode
        {
            mecSuccess            =   0x1111FD01,
            mecSuccessWithWarning =   0x1111FD02,
            mecFailure            =   0x1111FD03
        };

        enum MgArgumentType
        {
            matUnknown          =   0x1111FC01,
            matINT8             =   0x1111FC02,
            matUINT8            =   0x1111FC03,
            matINT16            =   0x1111FC04,
            matUINT16           =   0x1111FC05,
            matINT32            =   0x1111FC06,
            matUINT32           =   0x1111FC07,
            matINT64            =   0x1111FC08,
            matUINT64           =   0x1111FC09,
            matFloat            =   0x1111FC0A,
            matDouble           =   0x1111FC0B,
            matString           =   0x1111FC0C,
            matStream           =   0x1111FC0D,
            matClassId          =   0x1111FC0E,
            matEND              =   0x1111FC0F
        };

        enum MgControlID
        {
            mciUnknown       =   0x1111FB01,
            mciUserStop      =   0x1111FB02,
            mciServerStop    =   0x1111FB03,
            mciPause         =   0x1111FB04,
            mciContinue      =   0x1111FB05,
            mciClose         =   0x1111FB06
        };

        enum MgStreamContentType
        {
            msctUnknown = 0x1111FA00
        };

    ///////////////////////////////////////////////////////////////////////
    ///  Static Methods
    public :
        static MgPacketHeader GetPacketHeader( MgStreamData* pData );

        static bool GetControlPacket( MgStreamData* pData, MgControlPacket* pPacket );
};
/// \endcond

#endif  //  MGPACKETPARSER_H_MG
