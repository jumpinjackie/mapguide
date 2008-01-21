/**
\addtogroup Feature_Service_Module

<p>
  The Feature Service provides an abstraction layer for the
  storage and retrieval of feature data in a
  technology-independent way. You can use the API to determine
  which storage technologies are available and which
  capabilities they have. Access to the storage technology is
  modeled as a connection. For example, you can connect to a
  file and do simple insertions or connect to a relational
  database and do transaction-based operations.
</p>
<p>
  The root methods are contained in the MgFeatureService class.
</p>
<p>
  In general, you may do any of the following, subject to
  provider capability limitations:
  \li Determine which storage technologies are available (see
  MgFeatureService::GetFeatureProviders()),
  \li Determine which capabilities a feature provider has (see
  MgFeatureService::GetCapabilities()),
  \li Verify the connection property values (see
  MgFeatureService::GetConnectionPropertyValues()),
  \li Connect to a storage technology (feature provider) using
  either a
  \link MgFeatureService::TestConnection resource identifier \endlink
  or a
  \link MgFeatureService::TestConnection provider name and a connection string \endlink,
  \li Describe the structure of feature data (see
  \link DescribeSchema describe a feature schema \endlink,
  \li Determine the spatial contexts available in the datastore
  (see MgFeatureService::GetSpatialContexts()),
  \li Insert feature data (see MgInsertFeatures),
  \li Select feature data (see
  MgFeatureService::SelectFeatures() and
  MgFeatureService::SelectAggregate()),
  \li Update feature data (see MgUpdateFeatures),
  \li Delete feature data (see MgDeleteFeatures),
  \li Execute SQL commands (see
  MgFeatureService::ExecuteSqlQuery() and
  MgFeatureService::ExecuteSqlNonQuery).
  
</p>
**/ 