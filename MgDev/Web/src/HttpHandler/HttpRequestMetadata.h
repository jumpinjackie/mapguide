//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _HTTP_REQUEST_METADATA_H
#define _HTTP_REQUEST_METADATA_H

class MgHttpRequestMetadata;
template class MG_MAPAGENT_API Ptr<MgHttpRequestMetadata>;

/// <summary>
/// Purpose of this class is to package request meta data into a collection.
/// Users can add/remove/modify request meta data received from the actual request.
/// </summary>
class MG_MAPAGENT_API MgHttpRequestMetadata : public MgDisposable
{
    EXTERNAL_API:
        MgHttpRequestMetadata();

        /// <summary>
        /// Adds the property name and value to the collection.
        /// property name  -    case-insensitive
        /// property value -    case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the property
        /// Two property can not have same name.
        /// </param>
        /// <param name="value">Input
        /// Value corresponding to the property.
        /// NULL value and empty string is NOT allowed.
        /// </param>
        /// <returns>
        /// TRUE  -  property name is successfully added.
        /// FALSE -  property could not be added.
        /// Possible cause - property value is NULL.
        /// </returns>
        bool AddProperty(CREFSTRING property, CREFSTRING value);

        /// <summary>
        /// Removes the property name and value from the collection.
        /// property name - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the property to be removed.
        /// </param>
        /// <returns>
        /// TRUE  -  property name is successfully removed.
        /// FALSE -  property name could not be removed.
        /// Possible cause is property name does not exist.
        /// </returns>
        bool RemoveProperty(CREFSTRING property);

        /// <summary>
        /// Retrieve the value for the specified property.
        /// property name  - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the property for which value to be retrieved.
        /// </param>
        /// <returns>
        /// Value for the specified property or empty string if property does not exist.
        /// </returns>
        STRING GetPropertyValue(CREFSTRING property);

        /// <summary>
        /// Update the value for the specified property.
        /// property name  - case-insensitive
        /// property value - case-insensitive
        /// </summary>
        /// <param name="name">Input
        /// Name of the property to be updated.
        /// </param>
        /// <param name="value">Input
        /// Value corresponding to the property.
        /// NULL value and empty string is NOT allowed.
        /// </param>
        /// <returns>
        /// TRUE  -  property is successfully updated.
        /// FALSE -  property could not be updated.
        /// Possible cause is property does not exist or value is NULL.
        /// </returns>
        bool SetPropertyValue(CREFSTRING name, CREFSTRING value);

        /// <summary>
        /// Retrieve the list of all properties.
        /// </summary>
        /// <returns>
        /// A string collection containing names of all properties.
        /// </returns>
        MgStringCollection* GetPropertyNames();

    INTERNAL_API:
        /// <summary>
        /// Destructor to clean up internal objects
        /// </summary>
        virtual ~MgHttpRequestMetadata();

    protected:
        /// <summary>
        /// This method is responsible for releasing all
        /// its member variables and then destruct itself.
        /// </summary>
        /// <returns>Returns nothing</returns>
        virtual void Dispose();

        /// <summary>
        /// Get the class Id
        /// </summary>
        /// <returns>
        /// The integer value
        /// </returns>
        INT32 GetClassId();

    private:
        MgStringPropertyCollection m_reqMetadataCollection;
CLASS_ID:
        static const INT32 m_cls_id = HttpHandler_MapAgent_HttpRequestMetadata;
};

#endif
