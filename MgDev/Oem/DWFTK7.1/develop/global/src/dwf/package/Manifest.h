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


#ifndef _DWFTK_MANIFEST_H
#define _DWFTK_MANIFEST_H


///
///\file        dwf/package/Manifest.h
///\brief       This file contains the DWFManifest class declaration.
///


#include "dwf/Toolkit.h"
#include "dwf/package/Section.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/writer/PackageWriter.h"
#include "dwf/package/reader/PackageManifestReader.h"
#include "dwf/package/utility/PropertyContainer.h"

#include "dwfcore/Owner.h"
#include "dwfcore/Iterator.h"
using namespace DWFCore;


namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFManifest   dwf/package/Manifest.h     "dwf/package/Manifest.h"
///\brief       This class represents a fully parsed DWF package manifest document.
///\since       7.0.1
///
class DWFManifest : public DWFManifestReader

#ifndef DWFTK_READ_ONLY
                  , public DWFXMLSerializable
#endif
                  , public DWFPropertyContainer
{

public:

    ///
    ///\brief   This type defines the iterator for enumerating manifest interfaces.
    ///
    typedef DWFWCharKeySkipList<DWFInterface*>::Iterator    tInterfaceIterator;

    ///
    ///\class   SectionIterator     dwf/package/Manifest.h     "dwf/package/Manifest.h"
    ///\brief   This class defines the iterator for enumerating manifest sections.
    ///\since   7.0.1
    ///
    ///         This iterator implementation works over STL lists and maps.
    ///
    class SectionIterator : public DWFIterator<DWFSection*>
    {

    public:

        ///
        ///         Constructor
        ///
        ///\param   iBegin  Points to the first section.
        ///\param   iEnd    Points to the end of the section list.
        ///\throw   None
        ///
        SectionIterator( DWFSection::tList::iterator iBegin,
                         DWFSection::tList::iterator iEnd )
            throw()
            : _bList( true )
            , _ilBegin( iBegin )
            , _ilEnd( iEnd )
            , _ilCurrent( iBegin )
        {;}

        ///
        ///         Constructor
        ///
        ///\param   iBegin  Points to the first section.
        ///\param   iEnd    Points to the end of the section map.
        ///\throw   None
        ///
        SectionIterator( DWFSection::tMultiMap::iterator iBegin,
                         DWFSection::tMultiMap::iterator iEnd )
            throw()
            : _bList( false )
            , _imBegin( iBegin )
            , _imEnd( iEnd )
            , _imCurrent( iBegin )
        {;}

        ///
        ///         Copy Constructor
        ///
        ///\param   i       The iterator to clone.
        ///\throw   None
        ///
        SectionIterator( const SectionIterator& i )
            throw()
            : _bList( i._bList )
            , _ilBegin( i._ilBegin )
            , _ilEnd( i._ilEnd )
            , _ilCurrent( i._ilCurrent )
            , _imBegin( i._imBegin )
            , _imEnd( i._imEnd )
            , _imCurrent( i._imCurrent )
        {;}

        ///
        ///         Assignment Operator
        ///
        ///\param   i       The iterator to clone.
        ///\throw   None
        ///
        SectionIterator& operator=( const SectionIterator& i )
            throw()
        {
            _ilBegin = i._ilBegin;
            _ilEnd = i._ilEnd;
            _ilCurrent = i._ilCurrent;

            _imBegin = i._imBegin;
            _imEnd = i._imEnd;
            _imCurrent = i._imCurrent;

            _bList = i._bList;

            return *this;
        }

        ///
        ///         Destructor
        ///
        ///\throw   None
        ///
        virtual ~SectionIterator()
            throw()
        {;}

        ///
        ///\copydoc DWFCore::DWFIterator::reset()
        ///
        void reset()
            throw()
        {
            if (_bList)
            {
                _ilCurrent = _ilBegin;
            }
            else
            {
                _imCurrent = _imBegin;
            }
        }

        ///
        ///\copydoc DWFCore::DWFIterator::valid()
        ///
        bool valid()
            throw()
        {
            return (_bList ? (_ilCurrent != _ilEnd) : (_imCurrent != _imEnd));
        }

        ///
        ///\copydoc DWFCore::DWFIterator::next()
        ///
        bool next()
            throw()
        {
            if (valid())
            {
                if (_bList)
                {
                    _ilCurrent++;
                }
                else
                {
                    _imCurrent++;
                }

                return valid();
            }
            else
            {
                return false;
            }
        }

        ///
        ///\copydoc DWFCore::DWFIterator::get()
        ///
        DWFSection*& get()
            throw( DWFException )
        {
            if (valid())
            {
                return (_bList ? *_ilCurrent : _imCurrent->second);
            }
            else
            {
                _DWFCORE_THROW( DWFDoesNotExistException, L"No more elements" );
            }
        }

    private:

        bool _bList;

        DWFSection::tList::iterator _ilBegin;
        DWFSection::tList::iterator _ilEnd;
        DWFSection::tList::iterator _ilCurrent;

        DWFSection::tMultiMap::iterator _imBegin;
        DWFSection::tMultiMap::iterator _imEnd;
        DWFSection::tMultiMap::iterator _imCurrent;
    };

public:

    ///
    ///         Constructor
    ///
    ///         This constructor is generally only used by the 
    ///         parsing process when the DWFPackageReader associated with 
    ///         (and providing read access to) the DWF package file is available.
    ///         The subsequent binding makes it possible to read content
    ///         from the DWF package.
    ///
    ///\param   pPackageReader  Provides access to section content in the DWF package.
    ///\throw   None
    ///    
    _DWFTK_API
    DWFManifest( DWFPackageReader* pPackageReader )
        throw();

    ///
    ///         Constructor
    ///
    ///         This constructor is generally used by applications and publishers for
    ///         creating a package manifest.
    ///
    ///\param   zObjectID       Uniquely identifies the manifest.
    ///\throw   None
    ///
    _DWFTK_API
    DWFManifest( const DWFString& zObjectID )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFManifest()
        throw();

    ///
    ///         Returns version of the manifest document schema.
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
    ///         Returns the unique identifer of the manifest document.
    ///
    ///\return  The manifest object ID.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& objectID() const
        throw()
    {
        return _zObjectID;
    }

    ///
    ///         Returns an iterator for listing each unique section interface in the manifest.
    ///
    ///\return  A pointer to an iterator that must be deleted by the caller with the \b DWFCORE_FREE_OBJECT macro.
    ///         This pointer may be NULL.
    ///\throw   None
    ///
    _DWFTK_API
    tInterfaceIterator* getInterfaces()
        throw();

    ///
    ///         Locates a section in the manifest by name.
    ///
    ///\param   zName   The unique name of the section in the package.
    ///\return  A pointer to the section, if it exists, NULL otherwise.
    ///         The caller must not delete this pointer without explicitly
    ///         claiming ownership of it using the DWFCore::DWFOwnable interface
    ///         exposed by DWFSection.
    ///\throw   None
    ///
    _DWFTK_API
    DWFSection* findSectionByName( const DWFString& zName )
        throw();

    ///
    ///         Locates all sections in the manifest that share a type.
    ///
    ///\param   zType   The type of sections to locate.
    ///\return  A pointer to an iterator that must be deleted by the caller with the \b DWFCORE_FREE_OBJECT macro.
    ///         This pointer may be NULL.
    ///         The caller must not delete any section pointers obtained from the iterator without explicitly
    ///         claiming ownership of it using the DWFCore::DWFOwnable interface exposed by DWFSection.
    ///\throw   None
    ///
    _DWFTK_API
    SectionIterator* findSectionsByType( const DWFString& zType )
        throw();

    ///
    ///         Locates all non-global sections in the package.
    ///
    ///\return  A pointer to an iterator that must be deleted by the caller with the \b DWFCORE_FREE_OBJECT macro.
    ///         This pointer may be NULL.
    ///         The caller must not delete any section pointers obtained from the iterator without explicitly
    ///         claiming ownership of it using the DWFCore::DWFOwnable interface exposed by DWFSection.
    ///\throw   None
    ///
    _DWFTK_API
    SectionIterator* getSections()
        throw();

    ///
    ///         Locates all global sections in the package.
    ///
    ///\return  A pointer to an iterator that must be deleted by the caller with the \b DWFCORE_FREE_OBJECT macro.
    ///         This pointer may be NULL.
    ///         The caller must not delete any section pointers obtained from the iterator without explicitly
    ///         claiming ownership of it using the DWFCore::DWFOwnable interface exposed by DWFSection.
    ///\throw   None
    ///
    _DWFTK_API
    SectionIterator* getGlobalSections()
        throw();

    ///
    ///\copydoc DWFManifestReader::provideVersion()
    ///
    _DWFTK_API
    double provideVersion( double nVersion )
        throw();

    ///
    ///\copydoc DWFManifestReader::provideObjectID()
    ///
    _DWFTK_API
    const char* provideObjectID( const char* zObjectID )
        throw();

    ///
    ///\copydoc DWFManifestReader::provideDependency()
    ///
    _DWFTK_API
    DWFDependency* provideDependency( DWFDependency* pDependency )
        throw();

    ///
    ///\copydoc DWFManifestReader::provideInterface()
    ///
    _DWFTK_API
    DWFInterface* provideInterface( DWFInterface* pInterface )
        throw();

    ///
    ///\copydoc DWFManifestReader::provideProperty()
    ///
    _DWFTK_API
    DWFProperty* provideProperty( DWFProperty* pProperty )
        throw();

    ///
    ///\copydoc DWFManifestReader::provideSection()
    ///
    _DWFTK_API
    DWFSection* provideSection( DWFSection* pSection )
        throw();

#ifndef DWFTK_READ_ONLY

    ///
    ///\copydoc DWFXMLSerializable::serializeXML()
    ///
    _DWFTK_API
    void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
        throw( DWFException );

#endif

protected:

    ///
    ///\copydoc DWFCore::DWFOwner::notifyOwnerChanged()
    ///
    _DWFTK_API
    virtual void notifyOwnerChanged( DWFOwnable& rOwnable )
        throw( DWFException );

    ///
    ///\copydoc DWFCore::DWFOwner::notifyOwnableDeletion()
    ///
    _DWFTK_API
    virtual void notifyOwnableDeletion( DWFOwnable& rOwnable )
        throw( DWFException );

private:

    double                              _nVersion;
    DWFString                           _zObjectID;
    DWFSection::tList                   _oSectionsInOrder;
    DWFSection::tList                   _oGlobalSectionsInOrder;
    DWFSection::tMap                    _oSectionsByName;
    DWFSection::tMultiMap               _oSectionsByType;
    DWFWCharKeySkipList<DWFInterface*>  _oInterfaces;
};

}

#endif
