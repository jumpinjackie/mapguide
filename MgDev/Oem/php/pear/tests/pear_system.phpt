--TEST--
System commands tests
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php
error_reporting(E_ALL);
require_once 'System.php';

$sep = DIRECTORY_SEPARATOR;
$ereg_sep = $sep;
if (OS_WINDOWS) {
    $ereg_sep .= $sep;
}
/*******************
        mkDir
********************/
// Single directory creation
System::mkDir('singledir');
if( !is_dir('singledir') ){
    print "System::mkDir('singledir'); failed\n";
}
System::rm('singledir');

// Multiple directory creation
System::mkDir('dir1 dir2 dir3');
if (!@is_dir('dir1') || !@is_dir('dir2') || !@is_dir('dir3')) {
    print "System::mkDir('dir1 dir2 dir3'); failed\n";
}

// Parent creation without "-p" fail
if (@System::mkDir("dir4{$sep}dir3")) {
    print "System::mkDir(\"dir4{$sep}dir3\") did not failed\n";
}

// Create a directory which is a file already fail
touch('file4');
$res = @System::mkDir('file4 dir5');
if ($res) {
    print "System::mkDir('file4 dir5') did not failed\n";
}
if (!@is_dir('dir5')) {
    print "System::mkDir('file4 dir5') failed\n";
}

// Parent directory creation
System::mkDir("-p dir2{$sep}dir21 dir6{$sep}dir61{$sep}dir611");
if (!@is_dir("dir2{$sep}dir21") || !@is_dir("dir6{$sep}dir61{$sep}dir611")) {
    print "System::mkDir(\"-p dir2{$sep}dir21 dir6{$sep}dir61{$sep}dir611\")); failed\n";
}

/*******************
        mkTemp
********************/

// Create a temporal file with "tst" as filename prefix
$tmpfile = System::mkTemp('tst');
$tmpenv = str_replace($sep, $ereg_sep, System::tmpDir());
if (!@is_file($tmpfile) || !ereg("^$tmpenv{$ereg_sep}tst", $tmpfile)) {
    print "System::mkTemp('tst') failed\n";
    var_dump(is_file($tmpfile), $tmpfile, "^$tmpenv{$ereg_sep}tst", !ereg("^$tmpenv{$ereg_sep}tst", $tmpfile));
}

// Create a temporal dir in "dir1" with default prefix "tmp"
$tmpdir  = System::mkTemp('-d -t dir1');
if (!@is_dir($tmpdir) || !ereg("^dir1{$ereg_sep}tmp", $tmpdir)) {
    print "System::mkTemp('-d -t dir1') failed\n";
}

/*******************
        rm
********************/

// Try to delete a dir without "-r" option
if (@System::rm('dir1')) {
    print "System::rm('dir1') did not fail\n";
}

// Multiple and recursive delete
$del = "dir1 dir2 dir3 file4 dir5 dir6";
if (!@System::rm("-r $del")) {
    print "System::rm(\"-r $del\") failed\n";
}

/*******************
        which
********************/

if (OS_UNIX) {
    if (System::which('ls') != '/bin/ls') {
        print "System::which('ls') failed\n";
    }
    if (System::which('i_am_not_a_command')) {
        print "System::which('i_am_not_a_command') did not failed\n";
    }
} // XXX Windows test

/*******************
        cat
********************/
// Missing tests yet

print "end\n";
?>
--EXPECT--
end
