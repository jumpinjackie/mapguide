@ECHO OFF
set PHP_BIN=php.exe
%PHP_BIN% -d output_buffering=0 PEAR\go-pear.php %1
pause
