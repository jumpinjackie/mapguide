//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//


#ifndef _MG_RESOURCE_H_
#define _MG_RESOURCE_H_

class MgResourceService;

//////////////////////////////////////////////////////////////
/// \brief
/// The abstract base class for all resource types.
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResource : public MgNamedSerializable
{
PUBLISHED_API:

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Identifies the resource.
    ///
    /// \return
    /// Returns the unique identifier for this resource or NULL if
    /// the resource has not yet been opened or saved.
    ///
    MgResourceIdentifier* GetResourceId();

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Opens the resource using the specified resource service and
    /// resource identifier.
    ///
    /// \param resourceService
    /// Resource service.
    /// \param resourceId
    /// Resource identifier.
    ///
    void Open(MgResourceService* resourceService, MgResourceIdentifier* resourceId);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Saves the resource using the specified resource service.
    ///
    /// \remarks
    /// This method assumes a valid resource identifier has already
    /// been established for this resource via either Open or Save.
    ///
    /// \param resourceService
    /// Resource service.
    ///
    void Save(MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Saves the resource using the specified resource service and
    /// resource identifier.
    ///
    /// \param resourceService
    /// Resource service to use to save this
    /// resource.
    /// \param resourceId
    /// Resource identifier.
    ///
    void Save(MgResourceService* resourceService, MgResourceIdentifier* resourceId);

protected:
    /// \brief
    /// Construct a MgResource object
    ///
    /// \return
    /// Nothing
    ///
    ///
    MgResource();


    /// \brief
    /// Destruct a MgResource object
    ///
    /// \return
    /// Nothing
    ///
    ///
    virtual ~MgResource();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

    void SerializeToRepository(MgResourceService* resourceService, bool create);

    virtual const char* GetResourceTypeName() = 0;


CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_ResourceService_Resource;

protected:
    Ptr<MgResourceIdentifier>   m_resId;

private:
    static STRING               m_resourceDataTag;
};


#endif
