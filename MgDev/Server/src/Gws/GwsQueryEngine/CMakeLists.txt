include_directories(${FDO_INCLUDE_DIR}
    ${FDO_INCLUDE_DIR}/ExpressionEngine
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../GwsCommon/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

set(GwsQueryEngine_SRCS
    GwsBatchSortedBlockJoinQueryResults.cpp
    GwsBinaryFeature.cpp
    GwsBinaryFeatureReader.cpp
    GwsBinaryFeatureWriter.cpp
    GwsBinaryReader.cpp
    GwsBinaryWriter.cpp
    GwsCsQueryProcessor.cpp
    GwsExtendedFeatureDescription.cpp
    GwsFlatGwsIterator.cpp
    GwsFlatFdoReader.cpp
    GwsFdoCommand.cpp
    GwsFdoCommandDelete.cpp
    GwsFdoCommandInsert.cpp
    GwsFdoCommandLock.cpp
    GwsFdoCommandUnlock.cpp
    GwsFdoCommandUpdate.cpp
    GwsFeatureQueryDefinition.cpp
    GwsFeatureQueryResults.cpp
    GwsFeatureSourceQuery.cpp
    GwsFeaturesPool.cpp
    GwsJoinQueryDefinition.cpp
    GwsJoinQueryResults.cpp
    GwsMultiSelectIterator.cpp
    GwsMutableFeature.cpp
    GwsNestedLoopsJoinQueryResults.cpp
    GwsNestedLoopSortedBlockJoinQueryResults.cpp
    GwsPooledFeature.cpp
    GwsPreparedFeatureQuery.cpp
    GwsPreparedJoinQuery.cpp
    GwsPropertyDesc.cpp
    GwsQueryDefinition.cpp
    GwsQueryFactories.cpp
    GwsQueryUtils.cpp
    GwsQueryXml.cpp
    GwsRightBatchSortedBlockJoinQueryResults.cpp
    GwsRightJoinQueryResults.cpp
    GwsRightNestedLoopJoinQueryResults.cpp
    GwsRightNestedLoopSortedBlockJoinQueryResults.cpp
    GwsRightSortedJoinQueryResults.cpp
    GwsSortMergeJoinQueryResults.cpp
    utf8_.cpp
)

add_library(MgGwsQueryEngine${MG_VERSION_SUFFIX} SHARED ${GwsQueryEngine_SRCS})
install(TARGETS MgGwsQueryEngine${MG_VERSION_SUFFIX} DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${MG_COMPONENT})
install_symlink(${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libMgGwsQueryEngine${MG_VERSION_SUFFIX}.so libMgGwsQueryEngine.so ${MG_COMPONENT})