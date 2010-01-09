//
//  Copyright (C) 1996-2010 by Autodesk, Inc.
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

#ifndef _DWFTK_PUBLISHABLESECTION_H
#define _DWFTK_PUBLISHABLESECTION_H


///
///\file        dwf/publisher/PublishableSection.h
///\brief       This file contains the DWFPublishableSection class declaration.
///

#ifndef DWFTK_READ_ONLY



#include "dwfcore/STL.h"
#include "dwfcore/Iterator.h"
#include "dwfcore/Exception.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/publisher/Publishable.h"
#include "dwf/publisher/PublishableResource.h"
#include "dwf/publisher/EmbeddedFont.h"
#include "dwf/package/utility/PropertyContainer.h"
#include "dwf/package/utility/ResourceContainer.h"


namespace DWFToolkit
{

//
// fwd decl
//
class DWFPublisher;



///
///\ingroup         dwfpublish
///
///\interface   DWFPublishableSection   dwf/publisher/PublishableSection.h    "dwf/publisher/PublishableSection.h"
///\brief       Base class for sections that can be used with the DWFPublisher.
///\since       7.0.1
///
class DWFPublishableSection : public DWFPublishable
                            , public DWFResourceContainer
                            , public DWFPropertyContainer
{

public:

    ///
    ///\class       ResourceIterator   dwf/publisher/model/Model.h    "dwf/publisher/model/Model.h"
    ///\brief       An iterator for retrieving resources in the model.
    ///\since       7.0.1
    ///
        class ResourceIterator : public DWFIterator<DWFPublishableResource*>
    {

    public:

        ///
        ///         Constructor
        ///
        ///\param   rVector     The collection over which to iterate.
        ///\throw   None
        ///
        ResourceIterator( DWFPublishableResource::tList& rVector )
            throw()
            : _rVector( rVector )
            , _iCurrent( 0 )
        {;}

        ///
        ///         Copy Constructor
        ///
        ///\param   i           The iterator to clone.
        ///\throw   None
        ///
        ResourceIterator( const ResourceIterator& i )
            throw()
            : _rVector( i._rVector )
            , _iCurrent( i._iCurrent )
        {;}

        ///
        ///         Assignment Operator
        ///
        ///\param   i           The iterator to clone.
        ///\throw   None
        ///
        ResourceIterator& operator=( const ResourceIterator& i )
            throw()
        {
            _iCurrent = i._iCurrent;

            size_t n = 0;
            for (; n < i._rVector.size(); n++)
            {
                _rVector.push_back( i._rVector[n] );
            }

            return *this;
        }

        ///
        ///         Destructor
        ///
        ///\throw   None
        ///
        virtual ~ResourceIterator()
            throw()
        {;}

        ///
        ///\copydoc DWFIterator::reset
        ///
        void reset()
            throw()
        {
            _iCurrent = 0;
        }

        ///
        ///\copydoc DWFIterator::valid
        ///
        bool valid()
            throw()
        {
            return (_iCurrent < _rVector.size());
        }

        ///
        ///\copydoc DWFIterator::next
        ///
        bool next()
            throw()
        {
            if (valid())
            {
                _iCurrent++;
                return valid();
            }
            else
            {
                return false;
            }
        }

        ///
        ///\copydoc DWFIterator::get
        ///
        DWFPublishableResource*& get()
            throw( DWFException )
        {
            if (valid())
            {
                return _rVector[_iCurrent];
            }
            else
            {
                _DWFCORE_THROW( DWFDoesNotExistException, L"No more elements" );
            }
        }

    private:

        DWFPublishableResource::tList& _rVector;
        size_t  _iCurrent;
    };

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFPublishableSection()
        throw()
    {;}

    ///
    ///         Write the section structure to \a rPublisher.
    ///
    ///\param   rPublisher      The publisher that captures the output of the section.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void publish( DWFPublisher& rPublisher )
        throw( DWFException ) = 0;

    ///
    ///         Request the provided font be embedded in the section.
    /// 
    ///\param   pFont       The font data object to embed.  This object will assume ownership of \a pFont 
    ///                     and will delete it with \b DWFCORE_FREE_OBJECT.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void embedFont( DWFEmbeddedFont* /*pFont*/ )
        throw( DWFException )
    {
        _DWFCORE_THROW( DWFNotImplementedException, L"Font embedding not supported by this object" );
    }

    ///
    ///         Request that the resource be added to the section.
    ///
    ///         Use this method for adding thumbnails, previews, etc.
    ///
    ///\param   pResource   The resource to add. This object will assume ownership of \a pResource 
    ///                     and will delete it with \b DWFCORE_FREE_OBJECT.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void addResource( DWFPublishableResource* /*pResource*/ )
        throw( DWFException )
    {
        _DWFCORE_THROW( DWFNotImplementedException, L"Additional resources are not supported by this object" );
    }

    ///
    ///         Returns the list of resources added to this section.
    ///
    ///\return  A pointer to an iterator than will provide the resources.
    ///         This pointer must be deleted by the caller with \b DWFCORE_FREE_OBJECT
    ///         but the resource pointers themselves should not be.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual DWFIterator<DWFPublishableResource*>* getResources()
        throw( DWFException )
    {
        _DWFCORE_THROW( DWFNotImplementedException, L"Additional resources are not supported by this object" );
    }

    ///
    ///             Returns the title of the section.
    ///
    ///\return      Corresponds to the section title.
    ///\throw       None
    ///
    _DWFTK_API
    virtual const DWFString& getTitle() const
        throw()
    {
        return _zTitle;
    }

    ///
    ///             Returns the name of the source of the section.
    ///
    ///\return      Describes the source (application, component, etc.) of the section data.
    ///\throw       None
    ///
    _DWFTK_API
    virtual const DWFString& getSource() const
        throw()
    {
        return _zSource;
    }

    ///
    ///             Returns an identifier of the source of the section.
    ///
    ///\return      Uniquely identifies the source (document, file, database, etc.) of the section data.
    ///\throw       None
    ///
    _DWFTK_API
    virtual const DWFString& getSourceID() const
        throw()
    {
        return _zSourceID;
    }


protected:

    ///
    ///         Constructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    DWFPublishableSection( const DWFString& zSectionTitle = "",
                           const DWFString& zSectionSource = "",
                           const DWFString& zSectionSourceID = "" )
                           : _zTitle(zSectionTitle)
                           , _zSource(zSectionSource)
                           , _zSourceID(zSectionSourceID)
    {;}

private:

    DWFString               _zTitle;
    DWFString               _zSource;
    DWFString               _zSourceID;
};

}

#endif  

#endif

