<?php

    // Numbers of arguments:

    $MG_MIN_ARGS                                    = 7;
    $MG_MAX_ARGS                                    = 9;

    // Argument names:

    $MG_ARGN_COMMAND                                = "-c";
    $MG_ARGN_INCREMENTAL_LEVEL                      = "-l";
    $MG_ARGN_INPUT_PATH                             = "-i";
    $MG_ARGN_OUTPUT_PATH                            = "-o";

    // Argument values:

    $MG_ARGV_BACKUP                                 = "Backup";
    $MG_ARGV_RESTORE                                = "Restore";
    $MG_ARGV_PHP_LINUX                              = "php -c $(pwd)";
    $MG_ARGV_PHP_WINDOWS                            = "php -n";
    $MG_MIN_INCREMENTAL_LEVEL                       = 0;
    $MG_MAX_INCREMENTAL_LEVEL                       = 10;

    // Other constants:

    $MG_BIN_DIR_LINUX                               = "../bin/";
    $MG_BIN_DIR_WINDOWS                             = "..\\bin\\";

    $MG_COLD_BACKUP_DIR_NAME                        = "LastColdBackup";
    $MG_HOT_BACKUP_DIR_NAME                         = "LastHotBackup";
    $MG_PREFIX_COLD_BACKUP                          = "COLD_BK_";
    $MG_PREFIX_HOT_BACKUP                           = "HOT_BK_";
    $MG_PREFIX_BAD_BACKUP                           = "BAD_BK_";
    $MG_PREFIX_TEMP_BACKUP                          = "TEMP_BK_";
    $MG_FORMAT_TIMESTAMP                            = "Y-m-d_H-i-s";

?>
