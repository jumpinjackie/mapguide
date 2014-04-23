!include x64.nsh
!ifdef MAXCOMPRESSION

SetCompressor /FINAL /SOLID lzma
SetCompressorDictSize 64

FileBufSize 256

!endif

VIProductVersion "${MG_VERSION}"
VIAddVersionKey "FileVersion" "1.0.0.0"
VIAddVersionKey "CompanyName" "Open Source Geospatial Foundation"
VIAddVersionKey "FileDescription" "Installer Bootstrapper package for MapGuide Open Source"
VIAddVersionKey "LegalCopyright" "2013 Open Source Geospatial Foundation"

!ifndef CULTURE
!define CULTURE "en-us"
!endif

!define INSTALLER_OUTPUT "${INSTALLER_ROOT}\Output\${CULTURE}"

OutFile "${INSTALLER_OUTPUT}\${OUTNAME}.exe"

SilentInstall silent

Icon "${INSTALLER_ROOT}\Support\Graphics\OSGeo.ico"

Function .onInit
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "myMutex") i .r1 ?e'
	Pop $R0
	
	StrCmp $R0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
		Abort

; Only for 64-bit installers. Prevent installation on 32-bit windows for obvious reasons
!if ${CPU} = "x64"
    ${IfNot} ${RunningX64}
    MessageBox MB_OK|MB_ICONEXCLAMATION "This installer can only be run on 64-bit windows"
    Abort
    ${EndIf}
!endif
FunctionEnd

Section Main
	SetOutPath $TEMP
	SetOverwrite On
	
	Push $R0
    ClearErrors

BeginInstall:
    ; Install VC 2012 redist. Not going to check here. I think we can trust the vcredist install to not trample over anything
    Banner::show /NOUNLOAD "Installing Visual C++ 2012 Redistributable"
	Banner::getWindow /NOUNLOAD
!if ${CPU} = "x64"
	File /r "${INSTALLER_OUTPUT}\vcredist_2012_x64.exe"
	ExecWait '"$OUTDIR\vcredist_2012_x64.exe" /install /quiet /norestart'
!else
	File /r "${INSTALLER_OUTPUT}\vcredist_2012_x86.exe"
	ExecWait '"$OUTDIR\vcredist_2012_x86.exe" /install /quiet /norestart'
!endif
	Banner::destroy

	Banner::show /NOUNLOAD "Extracting files. Please Wait"
	Banner::getWindow /NOUNLOAD
	File "${INSTALLER_OUTPUT}\${OUTNAME}.msi"
	Banner::destroy
	; Run the MGOS installer
	ExecWait '"msiexec" /i "$OUTDIR\${OUTNAME}.msi"'
	; Delete the MGOS installer and any other extracted after completion
	Delete "$OUTDIR\${OUTNAME}.msi"
!if ${CPU} = "x64"
    Delete "$OUTDIR\vcredist_2012_x64.exe"
!else
    Delete "$OUTDIR\vcredist_2012_x86.exe"
!endif
	
SectionEnd