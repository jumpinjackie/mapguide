/**
\addtogroup DescribeSchema

<p>
Before you can insert feature data into a datastore, you must
apply a feature schema to it. That is, you must set up
structures within the datastore to receive the data. The
operation of applying a schema to a datastore is not
provided by the Feature Service API. However, the operation
of describing the existing schema or schemas in the
datastore is provided in two ways:
  \li MgFeatureService::DescribeSchema() returns a collection
    of MgFeatureSchema objects. See the \link Feature_Schema_Module Feature Schema overview\endlink.
  \li MgFeatureService::DescribeSchemaAsXml() returns the
    schema collection in the form of an XML specification.
There is also a convenient method for converting the XML
specification of a schema collection into a collection of
MgFeatureSchema objects, namely,
MgFeatureService::XmlToSchema().
</p>

**/
