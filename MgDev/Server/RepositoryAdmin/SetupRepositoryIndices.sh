#!/bin/sh

echo 
echo 
echo --- Setting up Session repository indices ...
echo openContainer MgSessionResourceContents.dbxml > SetupRepositoryIndices.Script
echo addIndex \'\' ResourceId node-element-equality-string >> SetupRepositoryIndices.Script
echo quit >> SetupRepositoryIndices.Script
echo 
../bin/dbxml -v -h "$(pwd)/../Repositories/Session" -s SetupRepositoryIndices.Script
echo 

echo 
echo --- Setting up Site repository indices ...
echo openContainer MgSiteResourceContents.dbxml > SetupRepositoryIndices.Script
echo addIndex \'\' Name edge-element-equality-string >> SetupRepositoryIndices.Script
echo quit >> SetupRepositoryIndices.Script
echo 
../bin/dbxml -v -h "$(pwd)/../Repositories/Site" -s SetupRepositoryIndices.Script
echo 

echo 
echo --- Setting up Library repository indices ...
echo openContainer MgLibraryResourceContents.dbxml > SetupRepositoryIndices.Script
echo addIndex \'\' ResourceId node-element-equality-string >> SetupRepositoryIndices.Script
echo openContainer MgLibraryResourceHeaders.dbxml >> SetupRepositoryIndices.Script
echo addIndex http://www.sleepycat.com/2002/dbxml name node-metadata-substring-string >> SetupRepositoryIndices.Script
echo addIndex http://www.autodesk.com/MapGuide/Resource/Metadata Depth node-metadata-equality-double >> SetupRepositoryIndices.Script
echo addIndex http://www.autodesk.com/MapGuide/Resource/Metadata Owner node-metadata-equality-string >> SetupRepositoryIndices.Script
echo addIndex \'\' Metadata node-element-presence-none >> SetupRepositoryIndices.Script
echo quit >> SetupRepositoryIndices.Script
echo 
../bin/dbxml -v -h "$(pwd)/../Repositories/Library" -s SetupRepositoryIndices.Script
echo 

rm SetupRepositoryIndices.Script
