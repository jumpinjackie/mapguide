<?php

    include 'RepositoryAdminDefs.inc';

    class RepositoryAdminApp
    {
        private $binDir             = "";
        private $command            = "";
        private $incrementalLevel   = 0;
        private $inputPath          = "";
        private $outputPath         = "";
        private $outputRoot         = "";

        public function RepositoryAdminApp($arguments)
        {
            include 'RepositoryAdminConstants.inc';

            $this->binDir = IsWindows() ? $MG_BIN_DIR_WINDOWS : $MG_BIN_DIR_LINUX;

            if (!$this->ParseArguments($arguments))
            {
                $this->DisplayUsage();
                exit();
            }
        }

        private function ParseArguments($arguments)
        {
            include 'RepositoryAdminConstants.inc';

            $size = count($arguments);

            if ($size < $MG_MIN_ARGS || $size > $MG_MAX_ARGS)
            {
                return FALSE;
            }

            for ($i = 2; $i < $size; $i += 2)
            {
                $argName  = trim($arguments[$i-1]);
                $argValue = trim($arguments[$i]);

                switch ($argName)
                {
                    case $MG_ARGN_COMMAND:
                        $this->command = $argValue;
                        break;

                    case $MG_ARGN_INCREMENTAL_LEVEL:
                        $this->incrementalLevel = $argValue;
                        break;

                    case $MG_ARGN_INPUT_PATH:
                        $this->inputPath = $argValue;
                        break;

                    case $MG_ARGN_OUTPUT_PATH:
                        $this->outputRoot = $argValue;
                        break;

                    default:
                        return FALSE;
                }
            }

            return TRUE;
        }

        public function DisplayUsage()
        {
            include 'RepositoryAdminConstants.inc';
            include 'RepositoryAdminResources.inc';

            $usage = $IDS_PROGRAM_SYNTAX.$IDS_PROGRAM_OPTIONS.$IDS_EXAMPLE_BACK_UP_OFFLINE_REPOSITORIES.$IDS_EXAMPLE_BACK_UP_ONLINE_REPOSITORIES.$IDS_EXAMPLE_RESTORE_COLD_BACKUP_REPOSITORIES.$IDS_EXAMPLE_RESTORE_HOT_BACKUP_REPOSITORIES;

            if (IsWindows())
            {
                $usage = str_replace($MG_ARGV_PHP_LINUX, $MG_ARGV_PHP_WINDOWS, $usage);
            }

            echo $usage;
        }

        protected function GetCurrentTimestamp()
        {
            include 'RepositoryAdminConstants.inc';

            $currTimestamp = date($MG_FORMAT_TIMESTAMP);

            if (empty($currTimestamp))
            {
                include 'RepositoryAdminResources.inc';

                throw new Exception($IDS_ERR_DATE_TIME);
            }

            return $currTimestamp;
        }

        protected function & GetDatabaseFiles($homeDir)
        {
            $program = $this->binDir.'db_archive -sv -h "'.$homeDir.'"';

            try
            {
                $output =& ExecuteProgram($program);
            }
            catch (Exception $e)
            {
                include 'RepositoryAdminResources.inc';

                throw new Exception($IDS_ERR_UNABLE_TO_RETRIEVE_DATABASE_FILES);
            }

            return $output;
        }

        protected function & GetLogFiles($homeDir, $includeUnusedFilesOnly = FALSE)
        {
            if ($includeUnusedFilesOnly)
            {
                $program = $this->binDir.'db_archive -v -h "'.$homeDir.'"';
            }
            else // all log files
            {
                $program = $this->binDir.'db_archive -lv -h "'.$homeDir.'"';
            }

            try
            {
                $output =& ExecuteProgram($program);
            }
            catch (Exception $e)
            {
                include 'RepositoryAdminResources.inc';

                throw new Exception($IDS_ERR_UNABLE_TO_RETRIEVE_LOG_FILES);
            }

            return $output;
        }

        private function TransferFile($fileName, $moveFile = FALSE)
        {
            include 'RepositoryAdminConstants.inc';
            include 'RepositoryAdminResources.inc';

            $srcFile = $this->inputPath.$fileName;
            $dstFile = $this->outputPath.$fileName;

            if ($MG_ARGV_BACKUP == $this->command)
            {
                echo sprintf($IDS_PROGRESS_ARCHIVING_FILE, $fileName);
            }
            else if ($MG_ARGV_RESTORE == $this->command)
            {
                echo sprintf($IDS_PROGRESS_RECOVERING_FILE, $fileName);
            }
            else
            {
                throw new Exception($IDS_ERR_INVALID_OPERATION);
            }

            if ($moveFile)
            {
                MoveFile($srcFile, $dstFile);
            }
            else
            {
                CopyFile($srcFile, $dstFile);
            }
        }

        private function TransferFiles($fileNames, $index = 0, $moveFile = FALSE)
        {
            $size = count($fileNames);

            if (($size - $index) <= 0)
            {
                include 'RepositoryAdminResources.inc';

                throw new Exception($IDS_ERR_DATABASE_OR_LOG_FILE_NOT_FOUND);
            }

            for ($i = $index; $i < $size; ++$i)
            {
                $this->TransferFile($fileNames[$i], $moveFile);
            }
        }

        protected function PerformCheckpoint($homeDir)
        {
            // TODO: db_checkpoint does not work as expected.
/*
            $program = $this->binDir.'db_checkpoint -1v -h "'.$homeDir.'"';

            try
            {
                $output =& ExecuteProgram($program);
            }
            catch (Exception $e)
            {
                // Checkpoint may harmlessly fail if no environment file exists.

//                include 'RepositoryAdminResources.inc';

//                throw new Exception($IDS_ERR_UNABLE_TO_PERFORM_CHECKPOINT);
            }
*/
        }

        protected function TransferDatabaseFiles()
        {
            $databaseFiles = $this->GetDatabaseFiles($this->inputPath);
            $this->TransferFiles($databaseFiles);
        }

        protected function TransferLogFiles()
        {
            include 'RepositoryAdminConstants.inc';
            include 'RepositoryAdminResources.inc';

            if ($MG_ARGV_BACKUP == $this->command)
            {
                $unusedFiles = $this->GetLogFiles($this->inputPath, TRUE);
                $logFiles = $this->GetLogFiles($this->inputPath, FALSE);
                $numUnusedFiles = count($unusedFiles);

                if ($MG_MIN_INCREMENTAL_LEVEL != $this->incrementalLevel
                    && $numUnusedFiles > 0)
                {
                    $this->TransferFiles($unusedFiles, 0, TRUE);
                }
            }
            else if ($MG_ARGV_RESTORE == $this->command)
            {
                $logFiles = $this->GetLogFiles($this->inputPath, FALSE);
                $numUnusedFiles = 0;
            }
            else
            {
                throw new Exception($IDS_ERR_INVALID_OPERATION);
            }

            $this->TransferFiles($logFiles, $numUnusedFiles, FALSE);
        }

        protected function CleanUpLogFiles($homeDir)
        {
            $program = $this->binDir.'db_archive -dv -h "'.$homeDir.'"';

            try
            {
                $output =& ExecuteProgram($program);
            }
            catch (Exception $e)
            {
                include 'RepositoryAdminResources.inc';

                throw new Exception($IDS_ERR_UNABLE_TO_CLEAN_UP_LOG_FILES);
            }
        }

        protected function PerformRecovery($homeDir)
        {
            $program = $this->binDir.'db_recover -cv -h "'.$homeDir.'"';

            try
            {
                $output =& ExecuteProgram($program);
            }
            catch (Exception $e)
            {
                include 'RepositoryAdminResources.inc';

                throw new Exception($IDS_ERR_UNABLE_TO_PERFORM_RECOVERY);
            }
        }

        protected function BackupOfflineRepositories()
        {
            include 'RepositoryAdminConstants.inc';
            include 'RepositoryAdminResources.inc';

            echo $IDS_PROGRESS_BACKING_UP_OFFLINE_REPOSITORY;

            $this->outputPath = $this->outputRoot.$MG_COLD_BACKUP_DIR_NAME;
            AppendSlashToEndOfPath($this->outputPath);

            if (is_dir($this->outputPath))
            {
                $modTimestamp = date($MG_FORMAT_TIMESTAMP, filemtime($this->outputPath));
                $lastBackupDir = $this->outputRoot.$MG_PREFIX_COLD_BACKUP.$modTimestamp;
                AppendSlashToEndOfPath($lastBackupDir);
                RenameDirectory($this->outputPath, $lastBackupDir);
            }

            CreateDirectory($this->outputPath);

            $this->PerformCheckpoint($this->inputPath);
            $this->TransferDatabaseFiles();
            $this->TransferLogFiles();
        }

        protected function BackupOnlineRepositories()
        {
            include 'RepositoryAdminConstants.inc';
            include 'RepositoryAdminResources.inc';

            echo $IDS_PROGRESS_BACKING_UP_ONLINE_REPOSITORY;

            $this->outputPath = $this->outputRoot.$MG_HOT_BACKUP_DIR_NAME;
            AppendSlashToEndOfPath($this->outputPath);
            $fullSnapShot = TRUE;

            if (is_dir($this->outputPath))
            {
                $databaseFiles = $this->GetDatabaseFiles($this->outputPath);
                $logFiles = $this->GetLogFiles($this->outputPath, FALSE);
                $numDatabaseFiles = count($databaseFiles);
                $numLogFiles = count($logFiles);

                if (($numDatabaseFiles == 0 && $numLogFiles != 0)
                 || ($numDatabaseFiles != 0 && $numLogFiles == 0))
                {
                    $badBackupDir = $this->outputRoot.$MG_PREFIX_BAD_BACKUP.$this->GetCurrentTimestamp();
                    AppendSlashToEndOfPath($badBackupDir);
                    RenameDirectory($this->outputPath, $badBackupDir);
                }
                else
                {
                    $unusedFiles = $this->GetLogFiles($this->outputPath, TRUE);
                    $numUnusedFiles = count($unusedFiles);
                    $numActiveFiles = $numLogFiles - $numUnusedFiles;

                    if ($numActiveFiles > $this->incrementalLevel)
                    {
                        $modTimestamp = date($MG_FORMAT_TIMESTAMP, filemtime($this->outputPath));
                        $lastBackupDir = $this->outputRoot.$MG_PREFIX_HOT_BACKUP.$modTimestamp;
                        AppendSlashToEndOfPath($lastBackupDir);
                        RenameDirectory($this->outputPath, $lastBackupDir);
                    }
                    else
                    {
                        $fullSnapShot = FALSE;
                    }
                }
            }

            if ($fullSnapShot)
            {
                CreateDirectory($this->outputPath);
                $this->TransferDatabaseFiles();
            }

            $this->TransferLogFiles();
        }

        protected function RestoreRepositories()
        {
            include 'RepositoryAdminConstants.inc';
            include 'RepositoryAdminResources.inc';

            echo $IDS_PROGRESS_RESTORING_BACKUP_REPOSITORY;

            $this->outputPath = $this->outputRoot;

            if (!IsDirectoryEmpty($this->outputPath, TRUE, FALSE))
            {
                $tempBackupDir = $this->outputRoot.$MG_PREFIX_TEMP_BACKUP.$this->GetCurrentTimestamp();
                AppendSlashToEndOfPath($tempBackupDir);
                CreateDirectory($tempBackupDir);
                MoveFiles($this->outputPath, $tempBackupDir);
            }

            $this->TransferDatabaseFiles();
            $this->TransferLogFiles();
            $this->PerformRecovery($this->outputPath);
            $this->CleanUpLogFiles($this->outputPath);
        }

        public function Run()
        {
            include 'RepositoryAdminConstants.inc';
            include 'RepositoryAdminResources.inc';

            CheckDirectory($this->inputPath);
            CheckDirectory($this->outputRoot);

            AppendSlashToEndOfPath($this->inputPath);
            AppendSlashToEndOfPath($this->outputRoot);

            try
            {
                switch ($this->command)
                {
                    case $MG_ARGV_BACKUP:
                        if ($MG_MIN_INCREMENTAL_LEVEL == $this->incrementalLevel)
                        {
                            $this->BackupOfflineRepositories();
                        }
                        else if ($this->incrementalLevel >  $MG_MIN_INCREMENTAL_LEVEL
                              && $this->incrementalLevel <= $MG_MAX_INCREMENTAL_LEVEL)
                        {
                            $this->BackupOnlineRepositories();
                        }
                        else
                        {
                            throw new Exception(sprintf($IDS_ERR_ARGUMENT_OUT_OF_RANGE, $this->incrementalLevel));
                        }

                        break;

                    case $MG_ARGV_RESTORE:
                        $this->RestoreRepositories();
                        break;

                    default:
                        throw new Exception(sprintf($IDS_ERR_INVALID_ARGUMENT, $this->command));
                }
            }
            catch (Exception $e)
            {
                if ($this->outputPath != $this->outputRoot && is_dir($this->outputPath))
                {
                    DeleteDirectory($this->outputPath);
                }

                throw $e;
            }
        }
    }

?>
