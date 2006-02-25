<?php

    include 'RepositoryAdminApp.inc';

    try
    {
        chdir(dirname(__FILE__));

        $repositoryAdminApp = new repositoryAdminApp($_SERVER['argv']);
        $repositoryAdminApp->Run();

        echo $IDS_PROGRESS_OPERATION_SUCCEEDED;
    }
    catch (Exception $e)
    {
        echo $e->getMessage()."\n".$IDS_PROGRESS_OPERATION_FAILED;
    }

?>
