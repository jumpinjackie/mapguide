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

#ifndef _DWFTK_PUBLISHED_OBJECT_H
#define _DWFTK_PUBLISHED_OBJECT_H


///
///\file        dwf/publisher/PublishedObject.h
///\brief       This file contains the DWFPublishedObject class declaration.
///


#ifndef DWFTK_READ_ONLY




#include "dwfcore/STL.h"
#include "dwfcore/String.h"
#include "dwfcore/Exception.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/utility/PropertyContainer.h"


namespace DWFToolkit
{

//
// fwd decl
//
class DWFPropertyVisitor;


///
///\ingroup         dwfpublish
///
///\class       DWFPublishedObject   dwf/publisher/PublishedObject.h    "dwf/publisher/PublishedObject.h"
///\brief       This class is used to capture the relationship between a graphics element/node
///             and some publishable structure and metadata.
///\since       7.0.1
///
class DWFPublishedObject : public DWFPropertyContainer
{
public:

    ///
    ///\brief   Defines the data type used for object keys.
    ///
    typedef long    tKey;
    ///
    /// \brief   Defines the data type used for object indices.
    ///
    typedef int     tIndex;

    ///
    ///\brief   Data structure used for capturing cross-object references.
    ///
    typedef struct
    {
        tKey                nKey;
        tIndex              nIndex;
        DWFPublishedObject* pObj;
        DWFString           zName;
    } tReference;

    ///
    ///\brief   This type defines a list of tReference pointers.
    ///
    typedef vector<tReference*>                     tReferenceList;
    ///
    ///\brief   This type defines a list of DWFPublishedObject pointers.
    ///
    typedef vector<DWFPublishedObject*>             tList;
    ///
    ///\brief   This type defines a mapped collection of DWFPublishedObject pointers.
    ///
    typedef DWFSkipList<tKey, DWFPublishedObject*>  tMap;

public:

    ///
    ///\ingroup         dwfpublish
    ///
    ///\interface   Factory   dwf/publisher/PublishedObject.h    "dwf/publisher/PublishedObject.h"
    ///\since       7.0.1
    ///
    class Factory
    {

    public:

        ///
        ///         Copy Constructor
        ///
        ///\throw   None
        ///
        _DWFTK_API
        Factory( const Factory& )
            throw()
        {;}

        ///
        ///         Assignment Operator
        ///
        ///\throw   None
        ///
        _DWFTK_API
        Factory& operator=( const Factory& )
            throw()
        {
            return *this;
        }

        ///
        ///         Destructor
        ///
        ///\throw   None
        ///
        _DWFTK_API
        virtual ~Factory()
            throw()
        {;}

        ///
        ///         Create a new object.
        ///
        ///\param   nKey    A unique key for this object.
        ///\param   zName   The object name.
        ///\return  The new object.
        ///\throw   DWFException
        ///
        _DWFTK_API
        virtual DWFPublishedObject* makePublishedObject( tKey              nKey,
                                                         const DWFString&  zName )
            throw( DWFException ) = 0;

        ///
        ///         Returns a pre-built object for a given key.
        ///
        ///         This is an optional method for those factories that also
        ///         keep their products around.
        ///
        ///\param   nKey    A unique key for this object.
        ///\return  The existing object.
        ///\throw   DWFException
        ///
        _DWFTK_API
        virtual DWFPublishedObject& findPublishedObject( tKey nKey )
            throw( DWFException ) = 0;

    protected:

        ///
        ///         Constructor
        ///
        ///\throw   None
        ///
        _DWFTK_API
        Factory()
            throw()
        {;}
    };

public:

    ///
    ///\ingroup         dwfpublish
    ///
    ///\interface   Visitor   dwf/publisher/PublishedObject.h    "dwf/publisher/PublishedObject.h"
    ///\brief       Base implementation and interface definition for objects (visitors) that
    ///             extract structure and metadata from the published objects.
    ///\since       7.0.1
    ///
    class Visitor
    {

    public:

        ///
        ///         Destructor
        ///
        ///\throw   None
        ///
        _DWFTK_API
        virtual ~Visitor()
            throw()
        {;}

        ///
        ///         Default visitation implementation.  Specializations should
        ///         invoke this base method to ensure any composed visitors are called.
        ///
        ///\param   rObject     An object from the publishable source.
        ///\throw   DWFException
        _DWFTK_API
        virtual void visitPublishedObject( DWFPublishedObject& rObject )
            throw( DWFException )
        {
            if (_pComposite)
            {
                _pComposite->visitPublishedObject( rObject );
            }
        }

    protected:

        ///
        ///         Constructor
        ///
        ///\throw   None
        ///
        _DWFTK_API
        Visitor( Visitor* pComposite = NULL )
            throw()
            : _pComposite( pComposite )
        {;}

    private:

        Visitor( const Visitor& );
        Visitor& operator=( const Visitor& );

    private:

        Visitor* _pComposite;
    };

public:

    ///
    ///         Constructor
    ///
    ///\param   nKey        Unique identifier for the object.
    ///\param   zName       Descriptive name for the object.
    ///\throw   None
    ///
    _DWFTK_API
    DWFPublishedObject( tKey             nKey,
                        const DWFString& zName )
        throw();

    ///
    ///         Copy Constructor
    ///
    ///\param   rSource     The object to copy.
    ///\throw   None
    ///
    _DWFTK_API
    DWFPublishedObject( DWFPublishedObject& )
        throw();

    ///
    ///        Assignment Operator
    ///
    ///\param   rSource     The object to copy.
    ///\throw   None
    ///
    _DWFTK_API
    DWFPublishedObject& operator=( DWFPublishedObject& )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFPublishedObject()
        throw();

    ///
    ///         Returns the object's name
    ///
    ///         Objects may exist in a hierachy and the full path
    ///         of one branch may be expressed as a concatenation
    ///         of object names.  Names are not necessarily unique
    ///         among all objects in the same set.
    ///
    ///\return  The descriptive name.
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& name() const
        throw()
    {
        return _zName;
    }

    ///
    ///         Returns the full path of the object 
    ///         expressed as a concatenation of hierachical
    ///         object names (including its own) and some
    ///         separation character.
    ///
    ///\param   zPath       A string object to receive the path.
    ///\throw   None
    ///
    _DWFTK_API
    void path( DWFString& zPath )
        throw();

    ///
    ///         Returns the object's unique key.
    ///
    ///         The key is used to bind the object to a data structure
    ///         that itself defines the graphic element or entry point
    ///         to it.  This data structure then provides the index of
    ///         (the association to) the actual graphics.  
    ///
    ///\return  The unique object identifier.
    ///\throw   None
    ///
    _DWFTK_API
    tKey key() const
        throw()
    {
        return _nKey;
    }

    //
    // return the object's instance id
    //
    _DWFTK_API
    unsigned int instance() const
        throw()
    {
        return _nIID;
    }

    //
    // assign the object's instance id
    //
    _DWFTK_API
    void setInstance( unsigned int nIID )
        throw()
    {
        _nIID = nIID;
    }

    //
    // return the scene index
    //
    _DWFTK_API
    tIndex index() const
        throw()
    {
        return _nIndex;
    }

    ///
    ///         Assigns an graphic index to the object.
    ///
    ///\param   nIndex  An indentifier that associates this object with the graphic node or element.
    ///\throw   None
    ///
    _DWFTK_API
    void setIndex( tIndex nIndex )
        throw()
    {
        _nIndex = nIndex;
    }

    ///
    ///         Returns the object's parent (if applicable.)
    ///
    ///\return  A pointer to the parent.  This pointer must not be deleted by the caller.
    ///\throw   None
    ///
    _DWFTK_API
    DWFPublishedObject* parent() const
        throw()
    {
        return _pParent;
    }

    ///
    ///         Assigns another as this object's parent.
    ///
    ///\param   pParentObject   A pointer to another object.
    ///                         This pointer remains the responsibility of the caller to delete.
    ///\throw   None
    ///
    _DWFTK_API
    void setParent( DWFPublishedObject* pParentObject )
        throw()
    {
        _pParent = pParentObject;
    }

    ///
    ///         Indicates that this object is a reference object.
    ///
    ///         A reference object is one that indicates reuse
    ///         of graphics in the scene and has additional considerations
    ///         when visited for reporting its metadata [structure].
    ///
    ///         For example, in the 3D model (DWFModel), published objects refer to
    ///         "interesting" segments in the scene graph that the publisher
    ///         wishes to identify as navigable entities and defined object
    ///         instances (DWFDefinedObjectInstance) from which to hang
    ///         object properties (DWFProperty).  The 3D API allows for graphics
    ///         to be defined once and applied many times in the scene resulting
    ///         in smaller files.  These are DWFIncludeSegment classes.
    ///         
    ///\throw   None
    ///
    _DWFTK_API
    void referenced()
        throw()
    {
        _bRef = true;
    }

    ///
    ///         Returns a flag indicating whether or not this is a reference object.
    ///
    ///\return  A flag indicating whether or not this is a reference object.
    ///\throw   None
    ///
    _DWFTK_API
    bool isReferenced()
        throw()
    {
        return _bRef;
    }

    ///
    ///         Creates a reference to an existing object.
    ///
    ///\param   pObject             The object to reference.
    ///\param   nKey                The unique identifier of the object to which the
    ///                             association is being made.
    ///\param   zInstanceName       An optional name for the reference relationship.
    ///\param   bPropertiesOnly     The reference is non-structural; only copy the properties
    ///                             to the referencing (\b this) object.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void addReference( DWFPublishedObject*      rObject,
                       DWFPublishedObject::tKey nKey,
                       const DWFString*         zInstanceName = NULL,
					   bool						bPropertiesOnly = false )
        throw( DWFException );

    ///
    ///         Returns the list of objects that this object makes reference to.
    ///
    ///\return  A reference to the internal list of object references.
    ///\throw   DWFException
    ///
    ///\todo    Don't return the internal collection.  Use a DWFIterator instead.
    ///
    _DWFTK_API
    tReferenceList& references()
        throw( DWFException );

    ///
    ///         Receives an object visitor and provides itself to its interface.
    ///
    ///\param   rVisitor    The published object visitor.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void accept( DWFPublishedObject::Visitor& rVisitor )
        throw( DWFException );

    ///
    ///         Receives a property visitor and provides itself to its interface.
    ///
    ///\param   rVisitor    The property visitor.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void accept( DWFPropertyVisitor& rVisitor )
        throw( DWFException );

private:

    bool                _bRef;
    
    tKey                _nKey;

    unsigned int        _nIID;
    unsigned int        _nIndex;
    DWFPublishedObject* _pParent;
    DWFString           _zName;

    tReferenceList      _oReferenceList;

private:

    DWFPublishedObject();
};


}


#endif  
#endif

