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
#ifndef _DWFEMAPTK_BACKGROUND_H
#define _DWFEMAPTK_BACKGROUND_H

#include "../EMapToolkit.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFBackground corresponds to the Background element in the
    // DWF EMap XML schema.
    // The Background element is used to define all the requisite background
    // information for the Map. Currently this is limited to a simple 32
    // bit RGBA color.
    //--------------------------------------------------------------------
    class DWFBackground : public DWFXMLBuildable
                        , public DWFXMLCallback
                        , public DWFXMLSerializable
    {
    public:
        // constructor used with XMLBuildable
        _DWFTK_API DWFBackground() throw();

        // basic constructor
        _DWFTK_API DWFBackground( unsigned int nColorARGB ) throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList( const char** ppAttributeList )
            throw( DWFException );

        // destructor
        _DWFTK_API virtual ~DWFBackground() throw();

        // property accessors
        _DWFTK_API const unsigned int color() const throw();

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
        unsigned int _nColorARGB;
        bool _bIsColor;
    };
}}

#endif // _DWFEMAPTK_BACKGROUND_H
// End of file.
