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


#ifndef _DWFTK_SECTION_H
#define _DWFTK_SECTION_H


///
///\file        dwf/package/Section.h
///\brief       This file contains the DWFSection class declaration.
///


#include "dwfcore/STL.h"
#include "dwfcore/Owner.h"
#include "dwfcore/SkipList.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/XML.h"
#include "dwf/package/Source.h"
#include "dwf/package/Resource.h"
#include "dwf/package/Interface.h"
#include "dwf/package/ObjectDefinition.h"
#include "dwf/package/ObjectDefinitionResource.h"
#include "dwf/package/writer/PackageWriter.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/reader/ObjectDefinitionReader.h"
#include "dwf/package/reader/SectionDescriptorReader.h"
#include "dwf/package/utility/ResourceContainer.h"
#include "dwf/package/utility/PropertyContainer.h"
#include "dwf/package/utility/DefinedObjectContainer.h"

namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFSection   dwf/package/Section.h     "dwf/package/Section.h"
///\brief       This class contains the base functionality for all sections in a DWF package.
///\since       7.0.1
///
///             <b>Section Descriptors</b>\par
///             While the package manifest outlines general section information,
///             it is the section's descriptor that provides the details.
///             The section classes in the toolkit are structured for highly
///             efficient processing.  As such, the descriptors are never 
///             automatically loaded.  On-demand descriptor parsing allows for
///             the most flexibility when processing an entire DWF package.
///             That said, it is very strongly recommended that a section's descriptor
///             is read into the DWFSection object before using it's data and resources;
///             especially when copying or moving resources between sections.
///
class DWFSection : public    DWFXMLBuildable
#ifndef DWFTK_READ_ONLY
                 , public    DWFXMLSerializable
#endif
                 , public    DWFOwnable
                 , public    DWFResourceContainer
                 , public    DWFPropertyContainer
                 , protected DWFXMLElementBuilder 
{

public:

    ///
    ///\brief   This type defines a list of DWFSection pointers.
    ///
    typedef vector<DWFSection*>                                         tList;
    ///
    ///\brief   This type defines a mapped collection of DWFSection pointers.
    ///
    typedef DWFWCharKeySkipList<DWFSection*>                            tMap;
    ///
    ///\brief   This type defines a multi-value mapped collection of DWFSection pointers.
    ///
    typedef multimap<const wchar_t*, DWFSection*, tDWFWCharCompareLess> tMultiMap;

public:

    ///
    ///\class   Factory     dwf/package/Section.h     "dwf/package/Section.h"
    ///\brief   Class factory for DWFSection objects.
    ///\since       7.0.1
    ///
    ///         Section factories are used by the DWFSectionBuilder.
    ///         Any custom sections that require creation by the DWFPackageReader
    ///         must provide a section factory.
    ///
    class Factory
    {

    public:

        ///
        ///\brief   This type defines a list of DWFSection::Factory pointers.
        ///
        typedef vector<DWFSection::Factory*>                tList;
        ///
        ///\brief   This type defines a mapped collection of DWFSection::Factory pointers.
        ///
        typedef DWFWCharKeySkipList<DWFSection::Factory*>   tMap;

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
        ///         The type of section this factory builds.
        ///
        ///\return  The section type.
        ///\throw   None
        ///
        _DWFTK_API
        const DWFString& type() const
            throw();

        ///
        ///         Create a new section.
        ///
        ///         This creation method is generally only used by the 
        ///         parsing process when the DWFPackageReader associated with 
        ///         (and providing read access to) the DWF package file is available.
        ///         The subsequent binding makes it possible to read section content
        ///         data from the DWF package.
        ///         
        ///\param   zName           A string that uniquely identifies the section in the DWF package.
        ///\param   zTitle          The descriptive and display friendly title text.
        ///\param   pPackageReader  A package reader to bind to the section.
        ///\throw   DWFException
        ///
        _DWFTK_API
        virtual DWFSection* build( const DWFString&     zName,
                                   const DWFString&     zTitle,
                                   DWFPackageReader*    pPackageReader )
            throw( DWFException );

        ///
        ///         Create a new section.
        ///
        ///         This creation method is generally only used by the 
        ///         parsing process when the DWFPackageReader associated with 
        ///         (and providing read access to) the DWF package file is available.
        ///         The subsequent binding makes it possible to read section content
        ///         data from the DWF package.
        ///         
        ///\param   zType           The section type identifier.
        ///\param   zName           A string that uniquely identifies the section in the DWF package.
        ///\param   zTitle          The descriptive and display friendly title text.
        ///\param   pPackageReader  A package reader to bind to the section.
        ///\throw   DWFException
        ///
        _DWFTK_API
        virtual DWFSection* build( const DWFString&     zType,
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
    DWFSection( DWFPackageReader* pPackageReader )
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
    DWFSection( const DWFString&  zType,
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
    DWFSection( const DWFString& zType,
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
    DWFSection( const DWFSection& )
        throw();

    ///
    ///         Assignment Operator
    ///
    ///\param   rSection    The source from which to construct this object.
    ///\throw   None
    ///
    _DWFTK_API
    DWFSection& operator=( const DWFSection& )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFSection()
        throw();

    ///
    ///         Returns the section name.
    ///
    ///         The section name refers to the string that uniquely identifies the section in the DWF package.
    ///         In the current format specification, the section name is used as the virtual directory
    ///         in the zip archive.  Usually this name is a string representation of a UUID. 
    ///         The \a title() method should be used to retrieve the descriptive and
    ///         display friendly text title of the section.
    ///
    ///\return  A string that uniquely identifies the section in the DWF package.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& name() const
        throw()
    {
        return _zName;
    }

    ///
    ///         Change the section name.
    ///
    ///         This method is generally used during the internal publishing process
    ///         in DWFPackageWriter::addSection() to ensure uniqueness.
    ///
    ///         The section name refers to the string that uniquely identifies the section in the DWF package.
    ///         In the current format specification, the section name is used as the virtual directory
    ///         in the zip archive.  Usually this name is a string representation of a UUID. 
    ///         The \a title() method should be used to retrieve the descriptive and
    ///         display friendly text title of the section.
    ///         
    ///\param   zName   A string that uniquely identifies the section in the DWF package.
    ///\throw   None
    ///
    _DWFTK_API
    virtual void rename( const DWFString& zName )
        throw();

    ///
    ///         Returns the section type.
    ///
    ///\return  The section type identifier.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& type() const
        throw()
    {
        return _zType;
    }

    ///
    ///         Returns the section title.
    ///
    ///\return  The descriptive and display friendly title text.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& title() const
        throw()
    {
        return _zTitle;
    }

    ///
    ///         Returns an object that can be used to identify
    ///         the source of the original data from which the
    ///         section content was created.
    ///
    ///         Source will always exist but may contain empty elements.
    ///
    ///\return  The source object.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFSource& source() const
        throw()
    {
        return _oSource;
    }

    ///
    ///         Identifies/updates the source information for this section.
    ///
    ///         This method will overwrite any existing data.
    ///
    ///\param   rSource     The source object from which to copy data.
    ///\throw   None
    ///
    _DWFTK_API
    void addSource( const DWFSource& rSource )
        throw();

    ///
    ///         Returns the interface associated with the section type.
    ///
    ///         An interface identifier is used in the DWF package manifest
    ///         for quick reference, to document all section types in the package.
    ///         
    ///\return  A pointer to a new interface object.  This object will be allocated
    ///         using the \b DWFCORE_ALLOC_OBJECT macro and must be deleted by the
    ///         caller using the \b DWFCORE_FREE_OBJECT macro.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual DWFInterface* buildInterface()
        throw( DWFException )
    {
        return NULL;
    }

    ///
    ///         Returns a unique identifier for the section.
    ///
    ///         This identifier should be immutable for the life of the section;
    ///         remaining unchanged during relocation and aggregation (the same
    ///         is not true of the \a name()).
    ///
    ///\return  A reference to the unique section identifier.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& objectID() const
        throw()
    {
        return _zObjectID;
    }

    ///
    ///         Returns the version of the section and/or descriptor.
    ///
    ///         Section types/classes should always be versioned to ensure
    ///         backwards compatability.
    ///
    ///\return  The version number.
    ///\throw   None
    ///
    _DWFTK_API
    double version() const
        throw()
    {
        return _nVersion;
    }

    ///
    ///         Returns the order in which the section was published.
    ///
    ///\return  The plot order.
    ///\throw   None
    ///
    _DWFTK_API
    double order() const
        throw()
    {
        return _nPlotOrder;
    }

    ///
    ///         Changes the plot order value.
    ///
	///\param	nPlotOrder	The new value.
	///\throw	None
	///
    ///\note    Sections are always re-ordered by the DWFPackageWriter.
    ///
    _DWFTK_API
    virtual void reorder( double nPlotOrder )
        throw()
    {
        _nPlotOrder = nPlotOrder;
    }

    ///
    ///			Locates the one descriptor resource in the section
    ///			and parses it accordingly with the reader provided.
   	///
	///\param	pSectionDescriptorReader	An optional interface pointer to an object that
	///										knows how to read and parse the descriptor associated
	///										with this section [type].  If NULL, this base implementation
	///										will always throw an DWFInvalidArgumentException as there is
	///										no generic parser.  
    ///
    ///\return  A reference to the descriptor document resource.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual
    const DWFResource& readDescriptor( DWFSectionDescriptorReader* pSectionDescriptorReader = NULL ) const
        throw( DWFException );

    ///
    ///         Processes the section descriptor using the reader interface provided.
    ///         This allows the caller to participate directly in the parsing process.
    ///         No in-memory data representation will be created nor cached by the DWFSection implicitly.
    ///
    ///\param   pSectionDescriptorReader    An interface reference to an object that knows how to read
    ///                                     and parse the descriptor associated with this section [type].
    ///\param   rResource                   The descriptor document resource for this section.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual 
    void readDescriptor( DWFSectionDescriptorReader& rSectionDescriptorReader,
                         DWFResource&                rResource ) const
        throw( DWFException );

    ///
    ///         Processes the section descriptor using the reader interface provided.
    ///         This allows the caller to participate directly in the parsing process.
    ///         No in-memory data representation will be created nor cached by the DWFSection implicitly.
    ///
    ///\param   rSectionDescriptorReader    An interface reference to an object that knows how to read
    ///                                     and parse the descriptor associated with this section [type].
    ///\param   rSectionDescriptorStream    A stream providing the descriptor document for this section.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual 
    void readDescriptor( DWFSectionDescriptorReader& rSectionDescriptorReader,
                         DWFInputStream&             rSectionDescriptorStream ) const
        throw( DWFException );

    ///
    ///			Locates all object definitions in the section and
    ///			parses them with the default reader implemented by DWFObjectDefinition.
	///
    ///\return  A pointer to a new DWFObjectDefinition.  The caller is reponsible for
    ///         deleting this pointer with the \b DWFCORE_FREE_OBJECT macro.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual DWFObjectDefinition* getObjectDefinition() const
        throw( DWFException );

    ///
    ///         Processes the object definition resource provided using the default reader
    ///         implemented by DWFObjectDefinition.
    ///
    ///         rObjectDefinitionFilter     A parsing filter.  It will be set on the 
    ///                                     new DWFObjectDefinition before processing begins.
    ///
    ///         rResource                   An object definition resource to process.
    ///
    ///\return  A pointer to a new DWFObjectDefinition.  The caller is reponsible for
    ///         deleting this pointer with the \b DWFCORE_FREE_OBJECT macro.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual DWFObjectDefinition* getObjectDefinition( DWFObjectDefinitionReader& rObjectDefinitionFilter,
                                                      DWFResource&               rResource ) const
        throw( DWFException );

    ///
    ///         Processes object definitions using the reader interface provided.
    ///         This allows the caller to participate directly in the parsing process.
    ///         No in-memory data representation will be created nor cached by the DWFSection implicitly.
    ///
    ///\param   rObjectDefinitionReader     An interface reference to an object that knows how to read 
    ///                                     and parse object definition documents.
    ///\param   pResource                   An optional pointer to an object definition resource.
    ///                                     If NULL, all object definition resources will be processed.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual 
    void getObjectDefinition( DWFObjectDefinitionReader& rObjectDefinitionReader,
                              DWFResource*               pResource = NULL ) const
        throw( DWFException );

    ///
    ///         Processes object definitions using the reader interface provided.
    ///         This allows the caller to participate directly in the parsing process.
    ///         No in-memory data representation will be created nor cached by the DWFSection implicitly.
    ///
    ///\param   rObjectDefinitionReader     An interface reference to an object that knows how to read 
    ///                                     and parse object definition documents.
    ///\param   rObjectDefinitionStream     A stream providing an object definition document for this section.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual 
    void getObjectDefinition( DWFObjectDefinitionReader& rObjectDefinitionReader,
                              DWFInputStream&            rObjectDefinitionStream ) const
        throw( DWFException );

    ///
    ///\copydoc DWFXMLBuildable::parseAttributeList()
    ///
    _DWFTK_API
    virtual void parseAttributeList( const char** ppAttributeList )
        throw( DWFException );

#ifndef DWFTK_READ_ONLY

    ///
    ///\copydoc DWFXMLSerializable::serializeXML()
    ///
    _DWFTK_API
    virtual void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
        throw( DWFException );

#endif


protected:

    DWFString           _zType;
    DWFString           _zName;
    DWFString           _zTitle;
    DWFString           _zObjectID;
    double              _nVersion;
    double              _nPlotOrder;
    DWFSource           _oSource;

    DWFPackageReader*   _pPackageReader;

protected:

    ///
    ///\copydoc DWFXMLElementBuilder::buildResource()
    ///
    virtual DWFResource* buildResource( const char**      ppAttributeList,
                                        DWFPackageReader* pPackageReader )
        throw( DWFException );

    ///
    ///\copydoc DWFXMLElementBuilder::buildFontResource()
    ///
    virtual DWFFontResource* buildFontResource( const char**      ppAttributeList,
                                                DWFPackageReader* pPackageReader )
        throw( DWFException );

    ///
    ///\copydoc DWFXMLElementBuilder::buildImageResource()
    ///
    virtual DWFImageResource* buildImageResource( const char**      ppAttributeList,
                                                  DWFPackageReader* pPackageReader )
        throw( DWFException );

    ///
    ///\copydoc DWFXMLElementBuilder::buildGraphicResource()
    ///
    DWFGraphicResource* buildGraphicResource( const char**      ppAttributeList,
                                              DWFPackageReader* pPackageReader )
        throw( DWFException );

private:

    //
    //
    //
    void _parseDocument( DWFInputStream& rDocumentStream,
                         DWFXMLCallback& rDocumentReader ) const
        throw( DWFException );
};

}

#endif
