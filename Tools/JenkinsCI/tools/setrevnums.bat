@echo off
SET MG_SOURCES_DIR=C:\working\sources
svn info "%MG_SOURCES_DIR%\fdo-3.9" | perl revnum.pl > "%MG_SOURCES_DIR%\fdo_39_revision.txt"
svn info "%MG_SOURCES_DIR%\fdo-trunk" | perl revnum.pl > "%MG_SOURCES_DIR%\fdo_trunk_revision.txt"
svn info "%MG_SOURCES_DIR%\mg-2.6\MgDev" | perl revnum.pl > "%MG_SOURCES_DIR%\mapguide_26_revision.txt"
svn info "%MG_SOURCES_DIR%\mg-trunk\MgDev" | perl revnum.pl > "%MG_SOURCES_DIR%\mapguide_trunk_revision.txt"
