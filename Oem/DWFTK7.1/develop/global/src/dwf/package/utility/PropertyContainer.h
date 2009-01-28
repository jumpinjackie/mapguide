//
//  Copyright (C) 1996-2009 by Autodesk, Inc.
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


#ifndef _DWFTK_PROPERTY_CONTAINER_H
#define _DWFTK_PROPERTY_CONTAINER_H


///
///\file        dwf/package/utility/PropertyContainer.h
///\brief       This file contains the DWFPropertyContainer class declaration.
///

#include "dwfcore/STL.h"

#include "dwf/Toolkit.h"
#include "dwf/package/Property.h"

namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFPropertyContainer   dwf/package/utility/PropertyContainer.h     "dwf/package/utility/PropertyContainer.h"
///\brief       An auto-mapping collection utility for DWFResource objects.
///\since       7.0.1
///
///             This class is a property owner.  As properties are added to the container,
///             they are associated in internal maps for fast, multi-keyed look-up.
///
///\todo        Consider deriving DWFPropertyContainer from DWFCore::DWFOwnable,
///             for consistent behavior with the rest of the API.
/// 
class DWFPropertyContainer : protected DWFOwner
                            _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE_MEMBER
{

public:

    ///
    ///\brief   This type defines a list of DWFPropertyContainer pointers.
    ///
    typedef vector<DWFPropertyContainer*>    tList;

public:

    ///
    ///         Constructor
    ///
    ///\param   zID     An optional identifier used when referencing this container.
    ///\throw   None
    ///
    _DWFTK_API
    DWFPropertyContainer( const DWFString& zID = L"" )
        throw();

    ///
    ///         Destructor
    ///
    ///         This method will delete only those properties
    ///         that are owned by this object.
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFPropertyContainer()
        throw();

    ///
    ///         Returns the reference identifier for this container.
    ///
    ///\return  The reference ID.
    ///\throw   None
    ///
    virtual const DWFString& id() const
    {
        return _zID;
    }

    ///
    ///         Locates a property in the container.
    ///
    ///\param   zName       The name of the property to look up (required.)
    ///\param   zCategory   The category of the property to look up (optional.)
    ///\return  A pointer to the property (may be NULL).  
    ///         This object is owned by the container by default and
    ///         must not be deleted by the caller without first explicitly
    ///         claiming ownership.
    ///\throw   None
    ///
    _DWFTK_API
    virtual const DWFProperty* const findProperty( const DWFString& zName,
                                                   const DWFString& zCategory = L"" )
        throw();

    ///
    ///         Locates all properties in the container that share a category.
    ///
    ///\param   zCategory   The property category to search by.
    ///\return  A pointer an iterator for enumerating the properties (may be NULL).  
    ///         This pointer must be deleted by the caller with the \b DWFCORE_FREE_OBJECT
    ///         macro.
    ///         Properties returned from the iterator are owned by the container 
    ///         by default and must not be deleted by the caller without first explicitly
    ///         claiming ownership.
    ///\throw   None
    ///
    _DWFTK_API
    virtual DWFProperty::tMap::Iterator* getProperties( const DWFString& zCategory = L"" )
        throw();

    ///
    ///         Inserts a property to the container.
    ///
    ///\param   pProperty               The property to add to the container (must not be NULL).
    ///\param   bOwnProperty            If \e true, the container will claim ownership of the property
    ///                                 and delete it using the \b DWFCORE_FREE_OBJECT macro upon
    ///                                 destruction of this container if it is still owned.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void addProperty( DWFProperty* pProperty,
                              bool         bOwnProperty )
        throw( DWFException );

    ///
    ///         Inserts another property container directly into the container.
    ///
    ///         Use this method to create sets of properties within a larger property [container]
    ///         context.  Inner containers will be serialized as inner sets.
    ///         To publish set references, use the \a referencePropertyContainer() method.
    ///
    ///\param   pContainer              The container to add to this container (must not be NULL).
    ///                                 This pointer will be owned by this container 
    ///                                 and deleted using the \b DWFCORE_FREE_OBJECT.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void addPropertyContainer( DWFPropertyContainer* pContainer )
        throw( DWFException );

    ///
    ///         Inserts another property container by reference into the container.
    ///
    ///         Use this method to create references to other sets of properties within a larger 
    ///         property [container] context.  Inner containers will be serialized as reference IDs.
    ///         To publish inlined inner sets, use the \a addPropertyContainer() method.
    ///
    ///\param   rContainer              The container to add to this container as a reference.
    ///\throw   DWFException
    ///
    _DWFTK_API
    virtual void referencePropertyContainer( const DWFPropertyContainer& rContainer )
        throw( DWFException );

    ///
    ///         Returns a list of only those property containers that have been directly
    ///         added with the \a addPropertyContainer() method.  
    ///
    ///         These inner containers are considered as owned by this object.
    ///\param   rContainerList          A list to which the container pointers are to be added.
    ///                                 These pointers must not be deleted by the caller.
    ///\throw   DWFException
    ///
    ///\todo    For API consistency, consider modifying this method to return an interator.
    ///
    _DWFTK_API
    virtual void getOwnedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
        throw( DWFException );

    ///
    ///         Returns a list of only those property containers that have been 
    ///         added as references with the \a referencePropertyContainer() method.  
    ///
    ///\param   rContainerList          A list to which the container pointers are to be added.
    ///                                 These pointers must not be deleted by the caller.
    ///\throw   DWFException
    ///
    ///\todo    For API consistency, consider modifying this method to return an interator.
    ///
    _DWFTK_API
    virtual void getReferencedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
        throw( DWFException );

    ///
    ///         Returns a list of all inner property containers.  
    ///
    ///\param   rContainerList          A list to which the container pointers are to be added.
    ///                                 These pointers must not be deleted by the caller.
    ///\throw   DWFException
    ///
    ///\todo    For API consistency, consider modifying this method to return an interator.
    ///
    _DWFTK_API
    virtual void getAllPropertyContainers( DWFPropertyContainer::tList& rContainerList )
        throw( DWFException );

    ///
    ///         Removes from the container and returns in a list, those property containers 
    ///         that have been directly added with the \a addPropertyContainer() method.  
    ///
    ///         These inner containers are considered as owned by this object.
    ///\param   rContainerList          A list to which the container pointers are to be added.
    ///                                 These pointers are now the responsibility of the caller
    ///                                 and must be deleted with the \b DWFCORE_FREE_OBJECT macro.
    ///\param   bMakeReferences         If \e true, the inner containers removed by this method
    ///                                 will be add to the referenced inner container list.
    ///\throw   DWFException
    ///
    ///\todo    For API consistency, consider modifying this method to return an interator.
    ///
    _DWFTK_API
    virtual void removeOwnedPropertyContainers( DWFPropertyContainer::tList& rContainerList,
                                                bool                         bMakeReferences = false )
        throw( DWFException );

    ///
    ///         Removes from the container and returns in a list, those property containers 
    ///         that have been added as references with the \a referencePropertyContainer() method.  
    ///
    ///\param   rContainerList          A list to which the container pointers are to be added.
    ///                                 These pointers must not be deleted by the caller.
    ///\throw   DWFException
    ///
    ///\todo    For API consistency, consider modifying this method to return an interator.
    ///
    _DWFTK_API
    virtual void removeReferencedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
        throw( DWFException );

    ///
    ///         Removes from the container and returns in a list, all inner containers.  
    ///
    ///\param   rContainerList          A list to which the container pointers are to be added.
    ///\throw   DWFException
    ///
    ///\todo    For API consistency, consider modifying this method to return an interator.
    ///
    _DWFTK_API
    virtual void removeAllPropertyContainers( DWFPropertyContainer::tList& rContainerList )
        throw( DWFException );

    ///
    ///         Copies all properties and inner containers \b from another container.
    ///
    ///\param   rContainer          The container from which to copy properties and property containers.
    ///\param   bRemoveOwnership    If \e true, this container will assume ownership of all of the
    ///                             the containers owned by \a rContainer by calling its
    ///                             \a removeOwnedPropertyContainers() method.
    ///                             If \e false, this container will reference all of the containers
    ///                             owned by \a rContainer.
    ///                             Irrespective of this flag, all inner containers referenced by
    ///                             \a rContainer will be again referenced by this container.
    ///\param   bMakeReferences     If \a bRemoveOwnership is \e true, this flag will be passed
    ///                             to the \a removeOwnedPropertyContainers() method of \a rContainer.
    ///\throw   None
    ///
    _DWFTK_API
    virtual void copyProperties( DWFPropertyContainer& rContainer,
                                 bool                  bRemoveOwnership = false,
                                 bool                  bMakeReferences = false)
        throw( DWFException );


#ifndef DWFTK_READ_ONLY

    ///
    ///         Returns an interface with which the container can be
    ///         serialized into an XML document.
    ///
    ///\return  A reference to a serialization inteface.
    ///\throw   None
    ///
    DWFXMLSerializable& getSerializable() const
        throw()
    {
        return (DWFXMLSerializable&)_oSerializer;
    }
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

#ifndef DWFTK_READ_ONLY

private:

    //
    // in order to avoid bizarre diamond patterns and
    // excessively overload base implementations,
    // all container classes must define and implementation this
    // internal [PRIVATE] class utility for serializing themselves into XML.
    //
    class _Serializer : public DWFXMLSerializable
    {

    public:

        _Serializer()
            throw()
            : DWFXMLSerializable()
            , _pContainer( NULL )
        {;}

        virtual ~_Serializer()
            throw()
        {;}

        void is( DWFPropertyContainer* pContainer )
        {
            _pContainer = pContainer;
        }

        //
        //
        //
        virtual void serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
            throw( DWFException );

    private:

        DWFPropertyContainer* _pContainer;
    };

private:

    //
    // Assigns a transient ID for this property container
    // This method is only intended for internal use within serialize()
    //
    void identify( const DWFString& zID )
        throw()
    {
        _zID = zID;
    }

#endif


protected:

    ///
    ///\brief       The main collection of properties, dual keyed by their names and categories.
    ///
    DWFProperty::tMap           _oProperties;
    ///
    ///\brief       The collection of sub-containers.
    ///
    DWFPropertyContainer::tList _oContainers;
    ///
    ///\brief       The collection of sub-containers that are only referenced by this container.
    ///
    DWFPropertyContainer::tList _oReferences;

private:

    DWFString                   _zID;

#ifndef DWFTK_READ_ONLY

    _Serializer                 _oSerializer;

#endif



private:

    DWFPropertyContainer( const DWFPropertyContainer& );
    DWFPropertyContainer& operator=( const DWFPropertyContainer& );
};

}

#endif
