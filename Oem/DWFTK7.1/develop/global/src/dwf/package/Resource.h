//
//  Copyright (C) 1996-2008 by Autodesk, Inc.
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


#ifndef _DWFTK_RESOURCE_H
#define _DWFTK_RESOURCE_H

///
///\file        dwf/package/Resource.h
///\brief       This file contains the DWFResource class declaration.
///


#include "dwfcore/STL.h"
#include "dwfcore/Owner.h"
#include "dwfcore/SkipList.h"
#include "dwfcore/InputStream.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/XML.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/writer/PackageWriter.h"
#include "dwf/package/utility/PropertyContainer.h"

namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFResource   dwf/package/Resource.h     "dwf/package/Resource.h"
///\brief       This class contains the base functionality for all resources in DWF section.
///\since       7.0.1
///
///             A resource is the common encapsulation of a set of data within a section.
///
class DWFResource : public DWFXMLBuildable

#ifndef DWFTK_READ_ONLY
                  , public DWFXMLSerializable
#endif
                  , public DWFPropertyContainer
                  , public DWFOwnable
{

public:

    ///
    ///\brief   This type defines a list of DWFResource pointers.
    ///
    typedef vector<DWFResource*>                                            tList;
    ///
    ///\brief   This type defines an integer ordered list of DWFResource pointers.
    ///
    typedef DWFSkipList<uint32_t, DWFResource*>                             tOrderedList;
    ///
    ///\brief   This type defines a mapped collection of DWFResource pointers.
    ///
    typedef DWFWCharKeySkipList<DWFResource*>                               tMap;
    ///
    ///\brief   This type defines a multi-value mapped collection of DWFResource pointers.
    ///
    typedef multimap<const wchar_t*, DWFResource*, tDWFWCharCompareLess>    tMultiMap;

public:

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read resource content
    ///         data from the DWF package.
    ///
    ///\param   pPackageReader  Provides access to resource content in the DWF package.
    ///\throw   None
    ///
    _DWFTK_API
    DWFResource( DWFPackageReader* pPackageReader )
        throw();

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read resource content
    ///         data from the DWF package.
    ///
    ///\param   zTitle          The descriptive and display friendly title text.
    ///\param   zRole           A functional description of the resource; 
    ///                         see the DWFToolkit::DWFXML class for the well-known roles.
    ///\param   zMIME           The MIME type of the resource data; see the DWFCore::DWFMIME
    ///                         class for well-known types.
    ///\param   zHREF           The location of the resource within the DWF package layout.
    ///                         This parameter should not be set manually by client applications
    ///                         in most cases.  The resource location is usually a composition
    ///                         of the section name and resource object ID which is assigned at publish time.
    ///                         
    ///\throw   None
    ///
    _DWFTK_API
    DWFResource( const DWFString& zTitle,
                 const DWFString& zRole,
                 const DWFString& zMIME,
                 const DWFString& zHREF = L"" )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFResource()
        throw();

    ///
    ///         Returns the resource title.
    ///
    ///\return  The title.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& title() const
        throw()
    {
        return _zTitle;
    }

    ///
    ///         Assigns the resource title.
    ///
    ///\param   zTitle          The descriptive and display friendly title text.
    ///\throw   None
    ///
    _DWFTK_API
    void setTitle( const DWFString& zTitle )
        throw()
    {
        _zTitle = zTitle;
    }

    ///
    ///         Returns the resource role.
    ///
    ///\return  The role.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& role() const
        throw()
    {
        return _zRole;
    }

    ///
    ///         Assigns the resource role.
    ///
    ///\param   zRole           A functional description of the resource; 
    ///                         see the DWFToolkit::DWFXML class for the well-known roles.
    ///\throw   None
    ///
    _DWFTK_API
    void setRole( const DWFString& zRole )
        throw()
    {
        _zRole = zRole;
    }

    ///
    ///         Returns the resource MIME type.
    ///
    ///\return  The MIME type.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& mime() const
        throw()
    {
        return _zMIME;
    }

    ///
    ///         Assigns the resource MIME type.
    ///
    ///\param   zMIME           The MIME type of the resource data; see the DWFCore::DWFMIME
    ///                         class for well-known types.
    ///\throw   None
    ///
    _DWFTK_API
    void setMIME( const DWFString& zMIME )
        throw()
    {
        _zMIME = zMIME;
    }

    ///
    ///         Returns the resource HREF.
    ///
    ///\return  The HREF.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& href() const
        throw()
    {
        return _zHRef;
    }

    ///
    ///         Assigns the resource location.
    ///
    ///\param   zHRef           The location of the resource within the DWF package layout.
    ///                         This parameter should not be set manually by client applications
    ///                         in most cases.  The resource location is usually a composition
    ///                         of the section name and resource object ID which is assigned at publish time.
    ///\throw   None
    ///
    _DWFTK_API
    void setHRef( const DWFString& zHRef )
        throw()
    {
        _zHRef = zHRef;
    }

    ///
    ///         Returns the resource object ID.
    ///
    ///\return  The object ID.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& objectID() const
        throw()
    {
        return _zObjectID;
    }

    ///
    ///         Sets the resource object ID.
    ///
    ///\param   zObjectID   A unique identifier.
    ///\throw   None
    ///
    _DWFTK_API
    void setObjectID( const DWFString& zObjectID )
        throw()
    {
        _zObjectID = zObjectID;
    }

    ///
    ///         Returns the object ID of the parent resource.
    ///
    ///\return  The parent object ID.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& parentID() const
        throw()
    {
        return _zParentObjectID;
    }

    ///
    ///         Defines a relationship between two resources.
    ///
    ///         This relationship is required when the
    ///         contents of the child resource are directly applicable
    ///         to the parent.  One example of this is object definition
    ///         resources as children of the graphic resource that define
    ///         the entities to which the instances are bound.
    ///
    ///\param   pResource   The parent resource or NULL to delete an existing relationship.
    ///\throw   None
    ///
    _DWFTK_API
    void setParentResource( const DWFResource* pResource )
        throw();

    ///
    ///         Defines a relationship between two resources.
    ///
    ///         This relationship is required when the
    ///         contents of the child resource are directly applicable
    ///         to the parent.  One example of this is object definition
    ///         resources as children of the graphic resource that define
    ///         the entities to which the instances are bound.
    ///
    ///\param   zObjectID   The object ID of the parent resource.
    ///\throw   None
    ///
    _DWFTK_API
    void setParentObjectID( const DWFString& zObjectID )
        throw()
    {
        _zParentObjectID = zObjectID;
    }

    ///
    ///         Returns the total uncompressed byte count of the resource data.
    ///
    ///\return  The number of bytes.
    ///\throw   None
    ///
    _DWFTK_API
    const size_t size() const
        throw()
    {
        return _nSize;
    }

    ///
    ///\internal    This method is used during the publishing process
    ///             to construct a valid destination HREF for the resource.
    ///
    ///\param       zSectionName    The unique section identifier.
    ///\param       zObjectID       The object ID that will be used in the HREF.
    ///                             If the object ID of this resource is empty when this
    ///                             function call is made, it will be assigned.
    ///
    _DWFTK_API
    void setPublishedIdentity( const DWFString& zSectionName,
                               const DWFString& zObjectID )
        throw();

    ///
    ///         Returns the publishing target HREF for the resource.
    ///
    ///         This method is used during publishing by DWFPackageWriter::write()
    ///         to discover the final HREF of the resource in the DWF package.
    ///         This is different than the "standard" HREF attribute since the 
    ///         package writer needs to recharacterize and identify sections
    ///         as they are written out; thus, the final resource location will
    ///         change.  The original property is not modified since the resource
    ///         may be used elsewhere; the package writer does not assume that it
    ///         is the final user of the object.
    ///
    ///\return  The final published location of the resource.
    ///
    _DWFTK_API
    const DWFString& publishedIdentity() const
        throw()
    {
        return _zTargetHRef;
    }

    ///
    ///         Provides a stream for reading the resource data.
    ///
    ///\warning If an input stream is bound to the resource using
    ///         setInputStream() for publishing, this method will
    ///         return that stream pointer.  This is very important
    ///         to realize: the caller must delete that stream pointer
    ///         by contract of this interface, as such, this method
    ///         will return a bogus pointer from that point on.  
    ///         <b>Do not call this method more than once if setInputStream()
    ///         binds the stream pointer.</b>
    ///
    ///\todo    Detect invalid stream pointer and either throw an exception
    ///         or return NULL.
    ///
    ///\param   bCache  If \e true, the uncompressed resource file
    ///                 will be stored in a temporary disk file
    ///                 for faster subsequent access.
    ///
    ///\return  A pointer to a data stream.
    ///         The caller is responsible for releasing the pointer
    ///         with the \b DWFCORE_FREE_OBJECT macro.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual DWFInputStream* getInputStream( bool bCache = false )
        throw( DWFException );

    ///
    ///\copydoc DWFXMLBuildable::parseAttributeList()
    ///
    _DWFTK_API
    virtual void parseAttributeList( const char** ppAttributeList )
        throw( DWFException );

#ifndef DWFTK_READ_ONLY

    ///
    ///         Binds a data source stream for providing the resource data.
    ///
    ///         This method is for use when publishing a resource.
    ///         It is assumed that the source stream (provided here) will
    ///         be read once and discarded.
    ///
    ///\warning This function is provided to make the initial connection
    ///         between source data and the DWF package I/O data object -
    ///         the DWFResource.  That is, it is a <b>publish-time only</b>
    ///         method.  This is important to understand since the first
    ///         call to getInputStream() will return this pointer and the caller
    ///         is obligated to delete it.  
    ///
    ///\todo    A future revision of this class should address the potential
    ///         inconsistency in this function pair; possibly introducing
    ///         a stream provider/builder object that is given to the resource
    ///         instead of the stream interface itself. This mediator could
    ///         ensure the availability of new stream object with all data content
    ///         for every call to getInputStream().
    ///
    ///\param   pStream     A pointer to a data stream.  This object
    ///                     must be allocated with the \b DWFCORE_ALLOC_OBJECT macro.
    ///\param   nBytes      Represents the total size (in uncompressed bytes)
    ///                     of the resource.  If \e zero, the number of available
    ///                     bytes in the stream initially will be used; however,
    ///                     there is no guarantee that this amount will always reflect
    ///                     the true size of the source content.
    ///
    _DWFTK_API
    void setInputStream( DWFInputStream* pStream, size_t nBytes = 0 )
        throw( DWFException );

    ///
    ///\copydoc DWFXMLSerializable::serializeXML()
    ///
    _DWFTK_API
    virtual void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
        throw( DWFException );

#endif


protected:

    ///
    ///     The total number of uncompressed bytes in the resource.
    ///
    size_t _nSize;

private:

    DWFString _zTitle;
    DWFString _zRole;
    DWFString _zMIME;
    DWFString _zHRef;
    DWFString _zTargetHRef;
    DWFString _zObjectID;
    DWFString _zParentObjectID;

    DWFInputStream*     _pStream;
    DWFPackageReader*   _pPackageReader;

private:

    DWFResource( const DWFResource& );
    DWFResource& operator=( const DWFResource& );
};

}

#endif
