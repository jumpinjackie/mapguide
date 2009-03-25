!define MG_VERSION "2.1.0"

VIProductVersion "${MG_VERSION}.0"
VIAddVersionKey "FileVersion" "1.0.0.0"
VIAddVersionKey "CompanyName" "Open Source Geospatial Foundation"
VIAddVersionKey "FileDescription" "Installer Bootstrapper package for MapGuide Open Source"
VIAddVersionKey "LegalCopyright" "2009 Open Source Geospatial Foundation"

!define INSTALLER_OUTPUT "${INSTALLER_ROOT}\Output"

OutFile "${INSTALLER_OUTPUT}\${OUTNAME}.exe"

SilentInstall silent

Icon "${INSTALLER_ROOT}\Support\Graphics\OSGeo.ico"

SetCompressor /SOLID lzma

Function .onInit
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "myMutex") i .r1 ?e'
	Pop $R0
	
	StrCmp $R0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
		Abort
FunctionEnd

Section Main
	SetOutPath $TEMP
	SetOverwrite On
	File "${INSTALLER_OUTPUT}\${OUTNAME}.msi"
	File "${INSTALLER_OUTPUT}\setup.exe"
	File /r "${INSTALLER_OUTPUT}\vcredist_x86"
	ExecWait '"$OUTDIR\setup.exe"'
SectionEnd