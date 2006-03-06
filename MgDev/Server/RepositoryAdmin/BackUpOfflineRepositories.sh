#!/bin/sh
if [ ! -d ../BackupRepositories/Library ]; then
  mkdir -p ../BackupRepositories/Library
fi
if [ ! -d ../BackupRepositories/Site ]; then
  mkdir -p ../BackupRepositories/Site
fi

export LD_LIBRARY_PATH=$(pwd)/../bin

php -n RepositoryAdmin.php -c Backup -l 0 -i "$(pwd)/../Repositories/Library" -o "$(pwd)/../BackupRepositories/Library"
php -n RepositoryAdmin.php -c Backup -l 0 -i "$(pwd)/../Repositories/Site"    -o "$(pwd)/../BackupRepositories/Site"
