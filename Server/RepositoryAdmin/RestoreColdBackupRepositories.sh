#!/bin/sh

php -c $(pwd) CheckVersion.php
STATUS=$?

if [ $STATUS -eq 0 ]; then
    echo 
    echo 
    echo --- Site repository
    echo 
    php -c $(pwd) RepositoryAdmin.php -c Restore -i "$(pwd)/../BackupRepositories/Site/LastColdBackup" -o "$(pwd)/../Repositories/Site"
    echo 

    echo 
    echo --- Library repository
    echo 
    php -c $(pwd) RepositoryAdmin.php -c Restore -i "$(pwd)/../BackupRepositories/Library/LastColdBackup" -o "$(pwd)/../Repositories/Library"
    echo 
fi
