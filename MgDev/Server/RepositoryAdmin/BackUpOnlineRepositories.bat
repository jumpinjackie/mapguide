if not exist "..\BackupRepositories\Library" md "..\BackupRepositories\Library"
if not exist "..\BackupRepositories\Site" md "..\BackupRepositories\Site"

php -n RepositoryAdmin.php -c Backup -l 2 -i "%cd%\..\Repositories\Library" -o "%cd%\..\BackupRepositories\Library"
php -n RepositoryAdmin.php -c Backup -l 2 -i "%cd%\..\Repositories\Site"    -o "%cd%\..\BackupRepositories\Site"
