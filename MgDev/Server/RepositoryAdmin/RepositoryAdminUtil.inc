<?php

    function & ExecuteProgram($program)
    {
        $result = "";
        $output = array();
        $status = 0;

        $result = exec($program, $output, $status);

        if (0 != $status)
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_UNABLE_TO_EXECUTE_PROGRAM, $program));
        }

        return $output;
    }

    function IsWindows()
    {
        $path = realpath(".");

        if (FALSE == strstr($path, '/'))
        {
            return TRUE;
        }

        return FALSE;
    }

    function PathEndsWithSlash($path)
    {
        $index = strlen($path) - 1;

        if ($index >= 0 && ('/' == $path[$index] || '\\' == $path[$index]))
        {
            return TRUE;
        }

        return FALSE;
    }

    function AppendSlashToEndOfPath(&$path)
    {
        if (!PathEndsWithSlash($path))
        {
            if (IsWindows() && FALSE == strstr($path, '/'))
            {
                $path = $path."\\\\";
            }
            else
            {
                $path = $path."/";
            }
        }
    }

    function RemoveSlashFromEndOfPath(&$path)
    {
        if (PathEndsWithSlash($path))
        {
            $path = substr($path, 0, strlen($path) - 1);
        }
    }

    function IsDirectoryEmpty($dir, $checkFiles = TRUE, $checkDirectories = TRUE)
    {
        AppendSlashToEndOfPath($dir);

        $files = scandir($dir);
        $size = count($files);
        $numSystemDirs = 2; // "." and ".."

        if ($checkFiles && $checkDirectories)
        {
            return (numSystemDirs == $size);
        }

        for ($i = $numSystemDirs; $i < $size; ++$i)
        {
            $pathname = $dir.$files[$i];

            if ($checkFiles && is_file($pathname))
            {
                return FALSE;
            }

            if ($checkDirectories && is_dir($pathname))
            {
                return FALSE;
            }
        }

        return TRUE;
    }

    function CreateDirectory($dir)
    {
        if (!mkdir($dir))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_UNABLE_TO_CREATE_DIRECTORY, $dir));
        }
    }

    function DeleteDirectory($dir)
    {
        // Note that this function will not delete sub-directories.

        DeleteFiles($dir);

        if (!rmdir($dir))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_UNABLE_TO_DELETE_DIRECTORY, $dir));
        }
    }

    function RenameDirectory($oldDir, $newDir)
    {
        if (!rename($oldDir, $newDir))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_UNABLE_TO_RENAME_DIRECTORY, $oldDir, $newDir));
        }
    }

    function CheckDirectory($pathname)
    {
        if (!is_dir($pathname))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_DIRECTORY_NOT_FOUND, $pathname));
        }
    }

    function CheckFile($pathname)
    {
        if (!is_file($pathname))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_FILE_NOT_FOUND, $pathname));
        }
    }

    function DeleteFile($file)
    {
        if (!unlink($file))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_UNABLE_TO_DELETE_FILE, $file));
        }
    }

    function DeleteFiles($dir)
    {
        AppendSlashToEndOfPath($dir);

        $files = scandir($dir);
        $size = count($files);

        for ($i = 0; $i < $size; ++$i)
        {
            $pathname = $dir.$files[$i];

            if (is_file($pathname))
            {
                DeleteFile($pathname);
            }
        }
    }

    function CopyFile($srcFile, $dstFile)
    {
        if (!copy($srcFile, $dstFile))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_UNABLE_TO_COPY_FILE, $srcFile, $dstFile));
        }
    }

    function CopyFiles($srcDir, $dstDir)
    {
        AppendSlashToEndOfPath($srcDir);
        AppendSlashToEndOfPath($dstDir);

        $files = scandir($srcDir);
        $size = count($files);

        for ($i = 0; $i < $size; ++$i)
        {
            $srcPathname = $srcDir.$files[$i];

            if (is_file($srcPathname))
            {
                $dstPathname = $dstDir.$files[$i];

                CopyFile($srcPathname, $dstPathname);
            }
        }
    }

    function MoveFile($srcFile, $dstFile)
    {
        if (!rename($srcFile, $dstFile))
        {
            include 'RepositoryAdminResources.inc';

            throw new Exception(sprintf($IDS_ERR_UNABLE_TO_MOVE_FILE, $srcFile, $dstFile));
        }
    }

    function MoveFiles($srcDir, $dstDir)
    {
        AppendSlashToEndOfPath($srcDir);
        AppendSlashToEndOfPath($dstDir);

        CheckDirectory($srcDir);
        CheckDirectory($dstDir);

        $files = scandir($srcDir);
        $size = count($files);

        for ($i = 0; $i < $size; ++$i)
        {
            $srcPathname = $srcDir.$files[$i];

            if (is_file($srcPathname))
            {
                $dstPathname = $dstDir.$files[$i];

                MoveFile($srcPathname, $dstPathname);
            }
        }
    }

?>
