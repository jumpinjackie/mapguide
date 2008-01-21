<?php

    // Program usage:

    $IDS_PROGRAM_SYNTAX                             = "Syntax:\n\n  php -c $(pwd) RepositoryAdmin.php [-c {Backup|Restore}] [-l {0 - 10}] [-i \"Input Path\"] [-o \"Ouput Path\"]\n\n";
    $IDS_PROGRAM_OPTIONS                            = "Options:\n\n  -c    Name of the command.\n        = Backup, to back up repositories.\n        = Restore, to restore repositories (any existing file directly under the specified output directory will be moved to a time-stamped backup sub-directory).\n\n  -l    Incremental level (from 0 to 10), only applicable to the Backup command.\n        = 0, offline (cold) backup - i.e. A full snapshot will always be taken.\n        > 0, online (hot) backup - e.g. For an incremental level value of 2:\n            a. If the number of active log files in the previous snapshot is less than or equal to 2, then an incremental snapshot will be taken.\n            b. If the number of active log files in the previous snapshot is greater than 2, then a full snapshot will be taken.\n        Note that active log files are files containing data that have NOT been saved to the database (by default, the size of a log file is about 10 MB).\n\n  -i    Absolute path to the source location from where the repository is backed up or restored.\n\n  -o    Absolute path to the destination location to where the repository is backed up or restored.\n        For offline (cold) backups, repositories will be archived to the \"LastColdBackup\" sub-directory; previous backups are saved to a time-stamped sub-directory.\n        For online (hot) backups, repositories will be archived to the \"LastHotBackup\" or a time-stamped sub-directory when taking an incremental or full snapshot respectively.\n\n";

    $IDS_EXAMPLE_BACK_UP_OFFLINE_REPOSITORIES       = "Examples:\n\n  1. Backing up offline (cold) repositories:\n\n    php -c $(pwd) RepositoryAdmin.php -c Backup -l 0 -i \"/MapGuide/Server/Repositories/Library\" -o \"/MapGuide/Server/BackupRepositories/Library\"\n\n";
    $IDS_EXAMPLE_BACK_UP_ONLINE_REPOSITORIES        = "  2. Backing up online (hot) repositories:\n\n    php -c $(pwd) RepositoryAdmin.php -c Backup -l 2 -i \"/MapGuide/Server/Repositories/Library\" -o \"/MapGuide/Server/BackupRepositories/Library\"\n\n";
    $IDS_EXAMPLE_RESTORE_COLD_BACKUP_REPOSITORIES   = "  3. Restoring cold backup repositories:\n\n    php -c $(pwd) RepositoryAdmin.php -c Restore -i \"/MapGuide/Server/BackupRepositories/Library/LastColdBackup\" -o \"/MapGuide/Server/Repositories/Library\"\n\n";
    $IDS_EXAMPLE_RESTORE_HOT_BACKUP_REPOSITORIES    = "  4. Restoring hot backup repositories:\n\n    php -c $(pwd) RepositoryAdmin.php -c Restore -i \"/MapGuide/Server/BackupRepositories/Library/LastHotBackup\" -o \"/MapGuide/Server/Repositories/Library\"\n\n";

    // Error messages:

    $IDS_ERR_UNCLASSIFIED                           = "An unclassified exception occurred.";
    $IDS_ERR_INVALID_OPERATION                      = "Invalid operation.";
    $IDS_ERR_DATE_TIME                              = "A date and/or time error occurred.";
    $IDS_ERR_INVALID_ARGUMENT                       = "Invalid argument: %s";
    $IDS_ERR_ARGUMENT_OUT_OF_RANGE                  = "Argument out of range: %d";

    $IDS_ERR_DIRECTORY_NOT_FOUND                    = "Directory not found: \"%s\"";
    $IDS_ERR_UNABLE_TO_CREATE_DIRECTORY             = "Unable to create directory: \"%s\"";
    $IDS_ERR_UNABLE_TO_DELETE_DIRECTORY             = "Unable to delete directory: \"%s\"";
    $IDS_ERR_UNABLE_TO_RENAME_DIRECTORY             = "Unable to rename directory \"%s\" to \"%s\"";

    $IDS_ERR_FILE_NOT_FOUND                         = "File not found: \"%s\"";
    $IDS_ERR_UNABLE_TO_DELETE_FILE                  = "Unable to delete file: \"%s\"";
    $IDS_ERR_UNABLE_TO_COPY_FILE                    = "Unable to copy file \"%s\" to \"%s\"";
    $IDS_ERR_UNABLE_TO_MOVE_FILE                    = "Unable to move file \"%s\" to \"%s\"";

    $IDS_ERR_DATABASE_OR_LOG_FILE_NOT_FOUND         = "Database or log file not found.";
    $IDS_ERR_UNABLE_TO_EXECUTE_PROGRAM              = "Unable to execute program: %s";
    $IDS_ERR_UNABLE_TO_PERFORM_CHECKPOINT           = "Unable to perform checkpoint.";
    $IDS_ERR_UNABLE_TO_RETRIEVE_DATABASE_FILES      = "Unable to retrieve database files.";
    $IDS_ERR_UNABLE_TO_RETRIEVE_LOG_FILES           = "Unable to retrieve log files.";
    $IDS_ERR_UNABLE_TO_CLEAN_UP_LOG_FILES           = "Unable to clean up log files.";
    $IDS_ERR_UNABLE_TO_PERFORM_RECOVERY             = "Unable to perform recovery.";

    // Progress messages:

    $IDS_PROGRESS_BACKING_UP_OFFLINE_REPOSITORY     = "Backing up offline (cold) repository ...\n";
    $IDS_PROGRESS_BACKING_UP_ONLINE_REPOSITORY      = "Backing up online (hot) repository ...\n";
    $IDS_PROGRESS_RESTORING_BACKUP_REPOSITORY       = "Restoring backup repository ...\n";
    $IDS_PROGRESS_ARCHIVING_FILE                    = "\tArchiving file: \"%s\"\n";
    $IDS_PROGRESS_RECOVERING_FILE                   = "\tRecovering file: \"%s\"\n";
    $IDS_PROGRESS_OPERATION_SUCCEEDED               = "Operation succeeded!\n";
    $IDS_PROGRESS_OPERATION_FAILED                  = "Operation failed!\n";

?>
