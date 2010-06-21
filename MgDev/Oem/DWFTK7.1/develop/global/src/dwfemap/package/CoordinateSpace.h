//
//  Copyright (C) 2003-2010 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//
#ifndef _DWFEMAPTK_COORDINATESPACE_H
#define _DWFEMAPTK_COORDINATESPACE_H

#include "../EMapToolkit.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    class DWFLinearUnit;
    class DWFBounds;
    class DWFView;

    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFCoordinateSpace corresponds to the CoordinateSpace element
    // in the DWF EMap XML schema.
    // The CoordinateSpace element is used to define a map's coordinate
    // system. The definition includes a description (name), its extents,
    // the units used, and an optional initial view of the coordinate system.
    // Also known as MCS space in Autodesk MapGuide(c) terminology.
    //--------------------------------------------------------------------
    class DWFCoordinateSpace : public DWFXMLBuildable
                             , public DWFXMLCallback
                             , public DWFXMLSerializable
    {
    public:
        // constructor used with XMLBuildable
        _DWFTK_API DWFCoordinateSpace() throw();

        // copy constructor
        _DWFTK_API DWFCoordinateSpace( const DWFCoordinateSpace& copy ) throw();

        // basic constructors
        _DWFTK_API DWFCoordinateSpace( const DWFString& zName,
                                       const DWFLinearUnit& units,
                                       const DWFBounds& bounds ) throw();

        _DWFTK_API DWFCoordinateSpace( const DWFString& zName,
                                       const DWFLinearUnit& units,
                                       const DWFBounds& bounds,
                                       const DWFView* pInitialView ) throw();

        _DWFTK_API DWFCoordinateSpace( const DWFString& zName,
                                       const DWFLinearUnit& units,
                                       const DWFBounds& bounds,
                                       const DWFBounds* pInitialBounds ) throw();

        // assignment operator
        DWFCoordinateSpace& operator=( const DWFCoordinateSpace& rho ) throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList( const char** ppAttributeList )
            throw( DWFException );

        // destructor
        _DWFTK_API virtual ~DWFCoordinateSpace() throw();

        // property accessors
        _DWFTK_API const DWFString& name() const throw();
        _DWFTK_API const DWFLinearUnit* const units() const throw();
        _DWFTK_API const DWFBounds* const bounds() const throw();
        _DWFTK_API const DWFView* const initialView() const throw();
        _DWFTK_API const DWFBounds* const initialBounds() const throw();

        // XMLCallback support
        _DWFTK_API void notifyStartElement( const char* zName,
                                            const char** ppAttributeList ) throw();
        _DWFTK_API void notifyEndElement( const char* zName ) throw();
        _DWFTK_API void notifyStartNamespace( const char* zPrefix,
                                              const char* zURI ) throw();
        _DWFTK_API void notifyEndNamespace( const char* zPrefix ) throw();
        _DWFTK_API void notifyCharacterData( const char* zCData, int nLength ) throw();

        // DWFXMLSerializable support
        _DWFTK_API void serializeXML( DWFXMLSerializer& rSerializer,
                                      unsigned int nFlags ) throw( DWFException );

    private:
        // data members
        DWFString _zName;
        DWFLinearUnit* _poUnits;
        DWFBounds* _poBounds;
        DWFView* _poInitialView;
        DWFBounds* _poInitialBounds;

        DWFXMLCallback* _poCallback;
        bool _bIsName;
    };
}}

#endif // _DWFEMAPTK_COORDINATESPACE_H
// End of file.
