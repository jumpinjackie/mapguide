<?php

    $version_required = "5.0.5";

    if(!phpVersion_Check($version_required)) {
        print "Your current PHP version ("
               . phpversion() . ") is too low\n";
        print "Please upgrade your copy of PHP ";
        print "to at least version : "
               .  $version_required;
        print "\n";
        exit;
    }

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

function phpVersion_Check ($version) {

    if(!function_exists("version_compare")) {
    /* the function version_compare was
    only introduced in PHP version 4.1.0
    which is what we normally recommend as
    the lower limit for our scripts.
    
    So if the function does not exist then
    we must be using a copy of PHP < 4.1.0 */
        return FALSE;
    }

    if(version_compare($version, phpversion()) <= 0){
        return TRUE;
    } else {
        return FALSE;
    }
}

?>
