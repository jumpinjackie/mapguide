//
//  Copyright (C) 1996-2011 by Autodesk, Inc.
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


#ifndef _DWFTK_CUSTOM_SECTION_H
#define _DWFTK_CUSTOM_SECTION_H

///
///\file        dwf/package/CustomSection.h
///\brief       This file contains the DWFCustomSection class declaration.
///


#include "dwfcore/STL.h"

#include "dwf/Toolkit.h"
#include "dwf/package/Section.h"

namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFCustomSection   dwf/package/CustomSection.h     "dwf/package/CustomSection.h"
///\brief       This class can be used to extend DWF with user-defined sections and content.
///\since       7.0.1
///
class DWFCustomSection : public DWFSection
{

public:

    ///
    ///\struct  tBehavior   dwf/package/CustomSection.h     "dwf/package/CustomSection.h"
    ///\brief   This data structure encapsulates certain logic required for the custom section
    ///         to be processed correctly by the toolkit.
    ///
    typedef struct
    {
        ///
        ///     If \e true, the section name will be modified
        ///     according to the current rules applied by
        ///     the DWFPackageWriter.
        ///     By default, this is \e true.
        ///
        bool    bRenameOnPublish;

        ///
        ///     If \e true, the publishing logic of the toolkit
        ///     will treat this section as a standard DWF section
        ///     and apply its particular rules for section descriptors.
        ///     By default, this is \e false.
        ///
        bool    bPublishDescriptor;

        //
        // if true, the publishing logic of the toolkit
        // will treat this section as a standard DWF section
        // with respect to it's package documentation, in particular,
        // the manifest will include a list of the sections resources.
        // by default, this is false
        //
        bool    bPublishResourcesToManifest;

    } tBehavior;

    ///
    ///\brief   This type defines a list of DWFCustomSection pointers.
    ///
    typedef vector<DWFCustomSection*>                                         tList;
    ///
    ///\brief   This type defines a mapped collection of DWFCustomSection pointers.
    ///
    typedef DWFWCharKeySkipList<DWFCustomSection*>                            tMap;
    ///
    ///\brief   This type defines a multi-valued mapped collection of DWFCustomSection pointers.
    ///
    typedef multimap<const wchar_t*, DWFCustomSection*, tDWFWCharCompareLess> tMultiMap;

public:

    ///
    ///\class   Factory     dwf/package/CustomSection.h     "dwf/package/CustomSection.h"
    ///\brief   Class factory for DWFCustomSection objects.
    ///\since       7.0.1
    ///
    class Factory
    {

    public:

        ///
        ///\brief   This type defines a list of DWFCustomSection::Factory pointers.
        ///
        typedef vector<DWFCustomSection::Factory*>                tList;
        ///
        ///\brief   This type defines a mapped collection of DWFCustomSection::Factory pointers.
        ///
        typedef DWFWCharKeySkipList<DWFCustomSection::Factory*>   tMap;

    public:

        ///
        ///         Constructor
        ///
        ///\param   zType   The section type.
        ///\throw   None
        ///
        _DWFTK_API
        Factory( const DWFString& zType )
            throw();

        ///
        ///         Destructor
        ///
        ///\throw   None
        ///
        _DWFTK_API
        virtual ~Factory()
            throw();

  ///
  ///\copydoc DWFSection::Factory::type()
  ///
        _DWFTK_API
        const DWFString& type() const
            throw();

        ///
        ///\copydoc DWFSection::Factory::build(const DWFString&,const DWFString&,DWFPackageReader*)
        ///
        _DWFTK_API
        virtual DWFCustomSection* build( const DWFString&     zName,
                                         const DWFString&     zTitle,
                                         DWFPackageReader*    pPackageReader )
            throw( DWFException );

        ///
        ///\copydoc DWFSection::Factory::build(const DWFString&,const DWFString&,const DWFString&,DWFPackageReader*)
        ///
        _DWFTK_API
        virtual DWFCustomSection* build( const DWFString&     zType,
                                         const DWFString&     zName,
                                         const DWFString&     zTitle,
                                         DWFPackageReader*    pPackageReader )
            throw( DWFException );

    private:

        DWFString _zType;
    };

public:

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read section content
    ///         data from the DWF package.
    ///
    ///\param   pPackageReader  Provides access to section content in the DWF package.
    ///\throw   None
    ///
    _DWFTK_API
    DWFCustomSection( DWFPackageReader* pPackageReader )
        throw();

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read section content
    ///         data from the DWF package.
    ///
    ///\param   zType           The section type identifier.
    ///\param   zName           A string that uniquely identifies the section in the DWF package.
    ///\param   zTitle          The descriptive and display friendly title text.
    ///\param   pPackageReader  Provides access to section content in the DWF package.
    ///\throw   None
    ///
    _DWFTK_API
    DWFCustomSection( const DWFString&  zType,
                      const DWFString&  zName,
                      const DWFString&  zTitle,
                      DWFPackageReader* pPackageReader )
        throw();

    ///
    ///         Constructor
    ///
    ///         This constructor is generally used by applications and publishers for
    ///         creating new section objects.
    ///
    ///\param   zType           The section type identifier.
    ///\param   zTitle          The descriptive and display friendly title text.
    ///\param   zObjectID       Uniquely identifies the section.
    ///\param   nVersion        Identifies the version of the section type and/or descriptor.
    ///\param   nPlotOrder      Indicates the initial position in the package to which the section was/will be published.
    ///\param   rSource         Describes the original source of the data in the section (a drawing file, for example.)
    ///\throw   None
    ///
    _DWFTK_API
    DWFCustomSection( const DWFString& zType,
                      const DWFString& zTitle,
                      const DWFString& zObjectID,
                      double           nVersion,
                      double           nPlotOrder,
                      const DWFSource& rSource )
        throw();

    ///
    ///         Copy Constructor
    ///
    ///\param   rSection    The source from which to construct this object.
    ///\throw   None
    ///
    _DWFTK_API
    DWFCustomSection( const DWFCustomSection& )
        throw();

    ///
    ///         Assignment Operator
    ///
    ///\param   rSection    The source from which to construct this object.
    ///\throw   None
    ///
    _DWFTK_API
    DWFCustomSection& operator=( const DWFCustomSection& )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFCustomSection()
        throw();

    ///
    ///         The section logic for the toolkit.
    ///
    ///\return  A copy of the behavior data.
    ///\throw   None
    ///
    _DWFTK_API
    DWFCustomSection::tBehavior behavior() const
        throw();

    ///
    ///         Sets the section logic for the toolkit.
    ///
    ///\param   rBehavior   The behavior logic.
    ///\throw   None
    ///
    _DWFTK_API
    void applyBehavior( const DWFCustomSection::tBehavior& rBehavior )
        throw();

#ifndef DWFTK_READ_ONLY

    ///
    ///\copydoc DWFXMLSerializable::serializeXML()
    ///
    _DWFTK_API
    virtual void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
        throw( DWFException );

#endif

protected:

    ///
    ///         The current section logic used by the toolkit.
    ///
    tBehavior   _tBehavior;
};

}

#endif
