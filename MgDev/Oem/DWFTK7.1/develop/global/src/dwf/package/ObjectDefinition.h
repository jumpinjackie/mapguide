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


#ifndef _DWFTK_OBJECT_DEFINITION_H
#define _DWFTK_OBJECT_DEFINITION_H


///
///\file        dwf/package/ObjectDefinition.h
///\brief       This file contains the DWFObjectDefinition class declaration.
///


#include "dwfcore/STL.h"

#include "dwf/Toolkit.h"
#include "dwf/package/reader/ObjectDefinitionReader.h"
#include "dwf/package/utility/PropertyContainer.h"

namespace DWFToolkit
{

//
// fwd decl
//
class DWFSection;

///
///\ingroup     dwfpackage
///
///\class       DWFObjectDefinition   dwf/package/ObjectDefinition.h     "dwf/package/ObjectDefinition.h"
///\brief       This class provides a reference implementation for the object definition document reader.
///\since       7.0.1
///
///             This implementation attempts to process and resolve the incoming reader data
///             to minimize parsing speed while setting up te object for quicker instance
///             and property resolution later on.  These actions are delayed until requested.
///
class DWFObjectDefinition : public DWFObjectDefinitionReader
{

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
    DWFObjectDefinition( DWFPackageReader* pPackageReader )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFObjectDefinition()
        throw();

    ///
    ///         Fully resolves the object and instance relationship hierarchy.
    ///
    ///\param   rRootInstances      A reference to a list collection that will
    ///                             hold the resolved hierarchy.
    ///\throw   DWFException
    ///
    ///\todo    Migrate away from returning collections through interface boundaries
    ///         in favor of using iterator pointers for consistency across the toolkit.
    ///
    _DWFTK_API
    void getRootInstances( DWFDefinedObjectInstance::tList& rRootInstances )
        throw( DWFException );

    ///
    ///         Resolves all properties associated with the instance provided.
    ///
    ///         An instance can obtain properties from its derived object,
    ///         references to other properties made by that derived object,
    ///         and references to other properties made by the instance itself.
    ///         Since properties may be effectively overridden, this method
    ///         resolves them in the following order, with those resolved
    ///         later overriding those resolved first:
    ///         - Derived object property references
    ///         - Derived object properties
    ///         - Instance property references
    ///         - Instance properties
    ///
    ///\note    This method performs no caching; all properties will be re-resolved
    ///         with each invocation.
    ///
    ///\param   rInstance       The object instance for which properties will be acquired.
    ///\return  A pointer to a new property container that holds the instance properties.
    ///         This pointer should be deleted by the caller using the \b DWFCORE_FREE_OBJECT macro.
    ///\throw   DWFException
    ///
    _DWFTK_API
    DWFPropertyContainer* getInstanceProperties( DWFDefinedObjectInstance& rInstance )
        throw( DWFException );

    ///
    ///         Resolves all properties associated with the instance obtained
    ///         from the identifier provided.
    ///
    ///         An instance can obtain properties from its derived object,
    ///         references to other properties made by that derived object,
    ///         and references to other properties made by the instance itself.
    ///         Since properties may be effectively overridden, this method
    ///         resolves them in the following order, with those resolved
    ///         later overriding those resolved first:
    ///         - Derived object property references
    ///         - Derived object properties
    ///         - Instance property references
    ///         - Instance properties
    ///
    ///\note    This method performs no caching; all properties will be re-resolved
    ///         with each invocation.
    ///
    ///\param   zInstanceID     Uniquely identifies an obejct instance.
    ///\return  A pointer to a new property container that holds the instance properties.
    ///         This pointer should be deleted by the caller using the \b DWFCORE_FREE_OBJECT macro.
    ///\throw   DWFException
    ///
    _DWFTK_API
    DWFPropertyContainer* getInstanceProperties( const DWFString& zInstanceID )
        throw( DWFException );

    ///
    ///         Returns the version of the object definition document schema.
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
    ///\copydoc DWFObjectDefinitionReader::provideVersion()
    ///
    _DWFTK_API
    double provideVersion( double nVersion )
        throw();

    ///
    ///\copydoc DWFObjectDefinitionReader::provideProperties()
    ///
    _DWFTK_API
    void provideProperties( const DWFString&    zID,
                            vector<DWFString>*  pPropertyRefs,
                            DWFProperty::tList* pPropertyList )
        throw();

    ///
    ///\copydoc DWFObjectDefinitionReader::provideObject()
    ///
    _DWFTK_API
    DWFDefinedObject* provideObject( DWFDefinedObject* pObject )
        throw();

    ///
    ///\copydoc DWFObjectDefinitionReader::provideInstance()
    ///
    _DWFTK_API
    DWFDefinedObjectInstance* provideInstance( DWFDefinedObjectInstance* pInstance )
        throw();

private:

    typedef struct _tPropertySet
    {
        DWFProperty::tList*             pList;
        vector<struct _tPropertySet*>   oRefs;

    } tPropertySet;

    typedef DWFStringKeySkipList<tPropertySet*> tPropertySetMap;

private:

    //
    //
    //
    void _resolvePropertyRefs( tPropertySet*         pSet,
                               DWFPropertyContainer* pContainer )
        throw();

protected:

    double                          _nVersion;

    tPropertySetMap                 _oPropertySets;
    DWFDefinedObject::tMap          _oObjects;
    DWFDefinedObjectInstance::tMap  _oInstances;
};


}

#endif
