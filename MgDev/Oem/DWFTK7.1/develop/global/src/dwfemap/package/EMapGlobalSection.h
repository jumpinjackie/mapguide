//
//  Copyright (C) 2003-2008 by Autodesk, Inc.
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
#ifndef _DWFEMAPTK_EMAPGLOBALSECTION_H
#define _DWFEMAPTK_EMAPGLOBALSECTION_H

#include "../EMapToolkit.h"
#include "dwf/package/GlobalSection.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFEMapGlobalSection represents the root document element
    // for defining a GlobalSection that contains Bookmarks to EMapSections,
    // amongst others, Resources that are not tied to a specific section and 
    // global Properties.
    // While all GlobalSections share the same funtionality this subclass 
    // follows the example of DWFEPlot and DWFEModel and defines the GlobalSection 
    // to work in the scope of DWFEMap. 
    //--------------------------------------------------------------------
    class DWFEMapGlobalSection : public DWFGlobalSection
    {
    public:
        // Inner class for defining the Factory that will construct this
        // EMapGlobalSection.
        class Factory : public DWFGlobalSection::Factory
        {
        public:
            // constructor / destructor
            _DWFTK_API Factory() throw();
            _DWFTK_API virtual ~Factory() throw();

            // inherited from DWFSection::Factory
            _DWFTK_API virtual DWFSection* build( const DWFString& zName,
                                                  const DWFString& zTitle,
                                                  DWFPackageReader* pPackageReader)
                                                  throw( DWFException );
        };

    public:
        // Reader Constructor
        _DWFTK_API DWFEMapGlobalSection( const DWFString&  zName,
                                         const DWFString&  zTitle,
                                         DWFPackageReader* pPackageReader ) throw();

        // Standard Constructor
        _DWFTK_API DWFEMapGlobalSection( const DWFString& zTitle = L"",
                                         const DWFSource& rSource = DWFSource(L"",L"",L"") )
            throw();

        // destructor
        _DWFTK_API virtual ~DWFEMapGlobalSection()throw();

        // DWFXMLSerializable support
        _DWFTK_API void serializeXML( DWFXMLSerializer& rSerializer,
                                      unsigned int nFlags ) throw( DWFException );

    private:
        // hidden copy constructor and assignment operator
        DWFEMapGlobalSection( const DWFEMapGlobalSection& );
        DWFEMapGlobalSection& operator=( const DWFEMapGlobalSection& );
    };
}}

#endif // _DWFEMAPTK_EMAPGLOBALSECTION_H
