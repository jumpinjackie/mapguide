@echo off

echo.
echo.
echo --- Site repository
echo.
php -n RepositoryAdmin.php -c Restore -i "%cd%\..\BackupRepositories\Site\LastColdBackup" -o "%cd%\..\Repositories\Site"
echo.

echo.
echo --- Library repository
echo.
php -n RepositoryAdmin.php -c Restore -i "%cd%\..\BackupRepositories\Library\LastColdBackup" -o "%cd%\..\Repositories\Library"
echo.

@echo on
