#!/bin/sh

export LD_LIBRARY_PATH=$(pwd)/../bin

php -n RepositoryAdmin.php -c Restore -i "$(pwd)/../BackupRepositories/Library/HOT_BK_2006-06-30_00-00-00" -o "$(pwd)/../Repositories/Library"
php -n RepositoryAdmin.php -c Restore -i "$(pwd)/../BackupRepositories/Site/HOT_BK_2006-06-30_00-00-00"    -o "$(pwd)/../Repositories/Site"
