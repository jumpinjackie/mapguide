#!/bin/sh

php -c $(pwd) CheckVersion.php
STATUS=$?


if [ $STATUS -eq 0 ]; then
    echo 
    echo 
    echo --- Site repository
    echo 

    if [ ! -d ../BackupRepositories/Site ]; then
        mkdir -p ../BackupRepositories/Site
    fi

    php -c $(pwd) RepositoryAdmin.php -c Backup -l 2 -i "$(pwd)/../Repositories/Site" -o "$(pwd)/../BackupRepositories/Site"
    echo 

    echo 
    echo --- Library repository
    echo 

    if [ ! -d ../BackupRepositories/Library ]; then
        mkdir -p ../BackupRepositories/Library
    fi

    php -c $(pwd) RepositoryAdmin.php -c Backup -l 2 -i "$(pwd)/../Repositories/Library" -o "$(pwd)/../BackupRepositories/Library"
    echo 
fi
