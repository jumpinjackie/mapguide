@echo off

echo.
echo.
echo --- Upgrading Session repository ...
echo upgradeContainer MgSessionResourceContents.dbxml > UpgradeRepositories.Script
echo quit >> UpgradeRepositories.Script
echo.
..\bin\dbxml -v -h "%cd%\..\Repositories\Session" -s UpgradeRepositories.Script
..\bin\db_upgrade -v "%cd%\..\Repositories\Session\MgSessionResourceData.db"
echo.

echo.
echo --- Upgrading Site repository ...
echo upgradeContainer MgSiteResourceContents.dbxml > UpgradeRepositories.Script
echo quit >> UpgradeRepositories.Script
echo.
..\bin\dbxml -v -h "%cd%\..\Repositories\Site" -s UpgradeRepositories.Script
echo.

echo.
echo --- Upgrading Library repository ...
echo upgradeContainer MgLibraryResourceContents.dbxml > UpgradeRepositories.Script
echo upgradeContainer MgLibraryResourceHeaders.dbxml >> UpgradeRepositories.Script
echo quit >> UpgradeRepositories.Script
echo.
..\bin\dbxml -v -h "%cd%\..\Repositories\Library" -s UpgradeRepositories.Script
..\bin\db_upgrade -v "%cd%\..\Repositories\Library\MgLibraryResourceData.db"
echo.

del UpgradeRepositories.Script

@echo on
