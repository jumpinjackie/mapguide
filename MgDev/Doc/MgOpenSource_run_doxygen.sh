############################################################################# 
## Delete the old files
echo Deleting old files ...

rm --recursive --verbose --force MgOpensource_WebAPIReference/*.*
rm --recursive --verbose --force MgOpensource_WebAPIReference/*

############################################################################# 
## Copy the CSS file
echo Copying doxygen.css ...

if [ ! -d MgOpensource_WebAPIReference/ ]; then
 	mkdir MgOpensource_WebAPIReference
fi

cp -f css/doxygen.css 	MgOpensource_WebAPIReference/

############################################################################# 
## Copy the XML schemas
echo Copying XML schemas ...

if [ ! -d MgOpensource_WebAPIReference/xml_schemas ]; then
 	mkdir MgOpensource_WebAPIReference/xml_schemas
fi

cp --recursive  xml_schemas/*		MgOpensource_WebAPIReference/xml_schemas/

############################################################################# 
## Run Doxygen

doxygen MgOpenSource_Doxyfile_Linux
echo Doxygen generation is complete. See 'MgOpenSource_warn_log.txt'.

#############################################################################
## Run the Perl post processor

perl -w PostProcessor/post_process.MgOpenSource.pl

################################################################### 
echo All done. 