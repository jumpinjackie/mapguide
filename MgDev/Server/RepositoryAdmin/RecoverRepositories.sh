#!/bin/sh

echo 
echo 
echo --- Recovering Site repository ...
echo 
../bin/db_recover -cv -h "$(pwd)/../Repositories/Site"
echo 

echo 
echo --- Recovering Library repository ...
echo 
../bin/db_recover -cv -h "$(pwd)/../Repositories/Library"
echo 
