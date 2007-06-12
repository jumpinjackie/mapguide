#!/bin/sh
if [ ! -d ../BackupRepositories/Library ]; then
  mkdir -p ../BackupRepositories/Library
fi
if [ ! -d ../BackupRepositories/Site ]; then
  mkdir -p ../BackupRepositories/Site
fi

php -c $(pwd) CheckVersion.php
STATUS=$?
if [ $STATUS -eq 0 ]; then
  php -c $(pwd) RepositoryAdmin.php -c Backup -l 2 -i "$(pwd)/../Repositories/Library" -o "$(pwd)/../BackupRepositories/Library"
  php -c $(pwd) RepositoryAdmin.php -c Backup -l 2 -i "$(pwd)/../Repositories/Site"    -o "$(pwd)/../BackupRepositories/Site"
fi
