@echo off
SET MG_SOURCES_DIR=C:\working\sources
svn info "%MG_SOURCES_DIR%\fdo-3.8" | perl revnum.pl > "%MG_SOURCES_DIR%\fdo_38_revision.txt"
svn info "%MG_SOURCES_DIR%\fdo-trunk" | perl revnum.pl > "%MG_SOURCES_DIR%\fdo_trunk_revision.txt"
svn info "%MG_SOURCES_DIR%\mg-2.5\MgDev" | perl revnum.pl > "%MG_SOURCES_DIR%\mapguide_25_revision.txt"
svn info "%MG_SOURCES_DIR%\mg-trunk\MgDev" | perl revnum.pl > "%MG_SOURCES_DIR%\mapguide_trunk_revision.txt"
