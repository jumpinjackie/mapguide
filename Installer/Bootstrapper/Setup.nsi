!define MG_VERSION "2.1.0"

VIProductVersion "${MG_VERSION}.0"
VIAddVersionKey "FileVersion" "1.0.0.0"
VIAddVersionKey "CompanyName" "Open Source Geospatial Foundation"
VIAddVersionKey "FileDescription" "Installer Bootstrapper package for MapGuide Open Source"

OutFile "${INSTALLER_OUTPUT}\${OUTNAME}.exe"

SilentInstall silent

Icon "${NSISDIR}\contrib\graphics\icons\orange-install.ico"

Section Main
	SetOutPath $TEMP
	SetOverwrite On
	File "${INSTALLER_OUTPUT}\${OUTNAME}.msi"
	File "${INSTALLER_OUTPUT}\setup.exe"
	File /r "${INSTALLER_OUTPUT}\vcredist_x86"
	ExecWait '"$OUTDIR\setup.exe"'
SectionEnd