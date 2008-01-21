#!/bin/sh
export LD_LIBRARY_PATH=$(pwd)/../bin

echo 
echo 
echo --- Upgrading Session repository ...
echo upgradeContainer MgSessionResourceContents.dbxml > UpgradeRepositories.Script
echo quit >> UpgradeRepositories.Script
echo 
../bin/dbxml -v -h "$(pwd)/../Repositories/Session" -s UpgradeRepositories.Script
../bin/db_upgrade -v "$(pwd)/../Repositories/Session/MgSessionResourceData.db"
echo 

echo 
echo --- Upgrading Site repository ...
echo upgradeContainer MgSiteResourceContents.dbxml > UpgradeRepositories.Script
echo quit >> UpgradeRepositories.Script
echo 
../bin/dbxml -v -h "$(pwd)/../Repositories/Site" -s UpgradeRepositories.Script
echo 

echo 
echo --- Upgrading Library repository ...
echo upgradeContainer MgLibraryResourceContents.dbxml > UpgradeRepositories.Script
echo upgradeContainer MgLibraryResourceHeaders.dbxml >> UpgradeRepositories.Script
echo quit >> UpgradeRepositories.Script
echo 
../bin/dbxml -v -h "$(pwd)/../Repositories/Library" -s UpgradeRepositories.Script
../bin/db_upgrade -v "$(pwd)/../Repositories/Library/MgLibraryResourceData.db"
echo 

rm UpgradeRepositories.Script
