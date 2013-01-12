//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef _MG_FEATURE_SET_H_
#define _MG_FEATURE_SET_H_

#ifndef FEATURESET_TEMPL
#define FEATURESET_TEMPL
class MgFeatureSet;
template class MG_PLATFORMBASE_API Ptr<MgFeatureSet>;
#endif

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// The FeatureSet class provides an iterator a way to create a
/// a set of features. A FeatureSet can be populated either automatically
/// from a FeatureReader or programmatically.
/// Each feature in a feature set is represented by a PropertyValue collection.
class MG_PLATFORMBASE_API MgFeatureSet : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgFeatureSet)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureSet();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureSet();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureSet();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgFeatureSet();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Populates this feature set from a FeatureReader.
    ///
    /// \remarks
    /// This methods is typically used to creates a feature
    /// set based on the result of a select command
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void FromReader(MgFeatureReader featureReader);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void FromReader(MgFeatureReader featureReader);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void FromReader(MgFeatureReader featureReader);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param featureReader (MgFeatureReader)
    /// FeatureReader input
    ///
    void FromReader(MgFeatureReader* featureReader);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of features in this feature set.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of features in this set.
    ///
    INT32 GetCount();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a feature from the feature set.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPropertyCollection GetFeatureAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPropertyCollection GetFeatureAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPropertyCollection GetFeatureAt(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the feature in the feature set
    ///
    /// \return
    /// PropertyValueCollection representing the feature.
    ///
    MgPropertyCollection* GetFeatureAt(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds a feature to the feature set.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void AddFeature(MgPropertyCollection feature);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void AddFeature(MgPropertyCollection feature);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void AddFeature(MgPropertyCollection feature);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param feature (MgPropertyCollection)
    /// PropertyValueCollection representing the feature to be added
    ///
    void AddFeature(MgPropertyCollection* feature);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the definition of the feature class.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the definition of the feature class.
    ///
    MgClassDefinition* GetClassDefinition();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the definition of the feature class.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetClassDefinition(MgClassDefinition classDef);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetClassDefinition(MgClassDefinition classDef);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetClassDefinition(MgClassDefinition classDef);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param classDef (MgClassDefinition)
    /// Definition of the feature class
    ///
    void SetClassDefinition(MgClassDefinition* classDef);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the set of features as a BatchPropertyValueCollection
    ///
    /// \remarks
    /// This methods is used to pass a set of features to an insert command
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgBatchPropertyCollection GetFeatures();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgBatchPropertyCollection GetFeatures();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgBatchPropertyCollection GetFeatures();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// BatchPropertyValueCollection containing all the features
    ///
    MgBatchPropertyCollection* GetFeatures();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    ~MgFeatureSet();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Clears all features from the set.
    ///
    void ClearFeatures();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the collection to the set.
    ///
    /// \param bpCol
    /// Collection to add
    ///
    void AddFeatures(MgBatchPropertyCollection* bpCol);

protected:

    virtual void Dispose();
    virtual INT32 GetClassId();

private:

    Ptr<MgBatchPropertyCollection> m_bpCollection;
    Ptr<MgClassDefinition>  m_classDefinition;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureSet;
};
/// \endcond

#endif
