#!/bin/sh

php -c $(pwd) CheckVersion.php
STATUS=$?
if [ $STATUS -eq 0 ]; then
  php -c $(pwd) RepositoryAdmin.php -c Restore -i "$(pwd)/../BackupRepositories/Library/CurrentHotBackup" -o "$(pwd)/../Repositories/Library"
  php -c $(pwd) RepositoryAdmin.php -c Restore -i "$(pwd)/../BackupRepositories/Site/CurrentHotBackup"    -o "$(pwd)/../Repositories/Site"
fi
