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
#ifndef _DWFEMAPTK_UIGRAPHIC_H
#define _DWFEMAPTK_UIGRAPHIC_H

#include "../EMapToolkit.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFUIGraphic corresponds to the UIGraphic element defined
    // in the DWF EMap XML schema.
    // The UIGraphic element is used to define UI (User Interface)
    // specific information for layers, layer groups and scale ranges.
    //--------------------------------------------------------------------
    class DWFUIGraphic : public DWFXMLBuildable
                       , public DWFXMLCallback
                       , public DWFXMLSerializable
    {
    public:
        // constructor used with XMLBuildable
        _DWFTK_API DWFUIGraphic( DWFPackageReader* pPackageReader ) throw();

        // basic constructor
        _DWFTK_API DWFUIGraphic( const DWFString& zObjectID,
                                 const DWFString& zLabel,
                                 bool bShowInLegend = true,
                                 bool bCollapsed = false ) throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList( const char** ppAttributeList )
                                                    throw(DWFException);

        // destructor
        _DWFTK_API virtual ~DWFUIGraphic() throw();

        // property accessors
        _DWFTK_API const DWFString& objectID() const throw();
        _DWFTK_API const DWFString& label() const throw();
        _DWFTK_API bool showInLegend() const throw();
        _DWFTK_API bool collapsed() const throw();

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
        // hidden copy constructor and assignment operator
        DWFUIGraphic( const DWFUIGraphic& );
        void operator=( const DWFUIGraphic& );

        // data members
        DWFString _zObjectID;
        DWFString _zLabel;
        bool _bShowInLegend;
        bool _bCollapsed;
        DWFPackageReader* _poCachedPackageReader;

        // flags for parsing
        bool _bIsLabel;
    };
}}

#endif // _DWFEMAPTK_UIGRAPHIC_H
// End of file.
