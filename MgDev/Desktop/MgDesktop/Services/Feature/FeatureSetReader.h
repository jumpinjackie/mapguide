#ifndef DESKTOP_FEATURE_SET_READER_H
#define DESKTOP_FEATURE_SET_READER_H

class MgdFeatureSetReader : public MgFeatureReader
{
    DECLARE_CLASSNAME(MgdFeatureSetReader);

INTERNAL_API:
	MgdFeatureSetReader(MgFeatureSet* featureSet);

EXTERNAL_API:
	virtual ~MgdFeatureSetReader();

	//MgReader
PUBLISHED_API:
    virtual bool ReadNext();

    virtual bool IsNull(CREFSTRING propertyName);

    virtual bool GetBoolean(CREFSTRING propertyName);

    virtual BYTE GetByte(CREFSTRING propertyName);

    virtual MgDateTime* GetDateTime(CREFSTRING propertyName);

    virtual float GetSingle(CREFSTRING propertyName);

    virtual double GetDouble(CREFSTRING propertyName);

    virtual INT16 GetInt16(CREFSTRING propertyName);

    virtual INT32 GetInt32(CREFSTRING propertyName);

    virtual INT64 GetInt64(CREFSTRING propertyName);

    virtual STRING GetString(CREFSTRING propertyName);

    virtual MgByteReader* GetBLOB(CREFSTRING propertyName);

    virtual MgByteReader* GetCLOB(CREFSTRING propertyName);

    virtual MgByteReader* GetGeometry(CREFSTRING propertyName);

    virtual MgRaster* GetRaster(CREFSTRING propertyName);

    virtual bool IsNull(INT32 index);

    virtual bool GetBoolean(INT32 index);

    virtual BYTE GetByte(INT32 index);

    virtual MgDateTime* GetDateTime(INT32 index);

    virtual float GetSingle(INT32 index);

    virtual double GetDouble(INT32 index);

    virtual INT16 GetInt16(INT32 index);

    virtual INT32 GetInt32(INT32 index);

    virtual INT64 GetInt64(INT32 index);

    virtual STRING GetString(INT32 index);

    virtual MgByteReader* GetBLOB(INT32 index);

    virtual MgByteReader* GetCLOB(INT32 index);

    virtual MgByteReader* GetGeometry(INT32 index);

    virtual MgRaster* GetRaster(INT32 index);

    virtual void Close();

    virtual INT32 GetReaderType();

EXTERNAL_API:

    virtual MgByteReader* ToXml();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the starting element name as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual string GetResponseElementName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the body starting element name as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual string GetBodyElementName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the start of the response as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void ResponseStartUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the end of the response as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void ResponseEndUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the start of the response body as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void BodyStartUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the end of the response body as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void BodyEndUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the contents of the header in this reader as a UTF-8 string.  The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void HeaderToStringUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the contents of the current record/feature in the reader as a UTF-8 string.  The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void CurrentToStringUtf8(string& str);

    virtual const wchar_t* GetString(CREFSTRING propertyName, INT32& length);

    virtual const wchar_t* GetString(INT32 index, INT32& length);

    virtual void Serialize(MgStream* stream);

    virtual void Deserialize(MgStream* stream);

    void ToXml(string &str);

    void AssignFeatureSet(MgFeatureSet* featureSet);

	//MgFeatureReader
PUBLISHED_API:
    virtual MgClassDefinition* GetClassDefinition();

    virtual MgFeatureReader* GetFeatureObject(CREFSTRING propertyName);

    virtual MgFeatureReader* GetFeatureObject(INT32 index);

INTERNAL_API:

    virtual BYTE_ARRAY_OUT GetGeometry(CREFSTRING propertyName, INT32& length);

    virtual BYTE_ARRAY_OUT GetGeometry(INT32 index, INT32& length);

INTERNAL_API:

    virtual MgClassDefinition* GetClassDefinitionNoXml();

    virtual MgFeatureSet* GetFeatures(INT32 count);

    MgByteReader* GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize);

protected:
	virtual void Dispose() { delete this; }

private:
    MgProperty* GetProperty(CREFSTRING propertyName, INT16 expectedType);
    /// Get the property for the specified name
    MgProperty* GetProperty(CREFSTRING propertyName);

    MgProperty* GetProperty(INT32 index, INT16 expectedType);
    /// Get the property at the specified index
    MgProperty* GetProperty(INT32 index);

    Ptr<MgFeatureSet> m_set;
    INT32 m_currRecord;
};

#endif