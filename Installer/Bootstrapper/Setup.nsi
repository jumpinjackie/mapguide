!ifdef MAXCOMPRESSION

SetCompressor /FINAL /SOLID lzma
SetCompressorDictSize 64

FileBufSize 256

!endif

VIProductVersion "${MG_VERSION}"
VIAddVersionKey "FileVersion" "1.0.0.0"
VIAddVersionKey "CompanyName" "Open Source Geospatial Foundation"
VIAddVersionKey "FileDescription" "Installer Bootstrapper package for MapGuide Open Source"
VIAddVersionKey "LegalCopyright" "2010 Open Source Geospatial Foundation"

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
FunctionEnd

Section Main
	SetOutPath $TEMP
	SetOverwrite On
	
	Push $R0
    ClearErrors
;
; NOTE: We're keeping the VC2008 redist check here as some PHP extensions are built against MSVCR90
;
CheckVC2008Install:
;
; http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/639e2cfd-bb61-425b-a087-a2442df23402   
;
!if ${CPU} = "x64"
	SetRegView 64
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\DevDiv\VC\Servicing\9.0\RED\1033" "SP"
    IfErrors InstallVC2008Redist CheckVC2010Install
!else
	SetRegView 32
    ; Reg key for 32-bit OS
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\DevDiv\VC\Servicing\9.0\RED\1033" "SP"
    IfErrors CheckVS2008Wow32Registry CheckVC2010Install
	
CheckVS2008Wow32Registry:	
	SetRegView 64
    ; Reg key for 64-bit OS (WoW32)
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\DevDiv\VC\Servicing\9.0\RED\1033" "SP"
    IfErrors InstallVC2008Redist CheckVC2010Install
!endif

InstallVC2008Redist:
	MessageBox MB_OK "Installing Visual C++ 2008 redist"
!if ${CPU} = "x64"
	File /r "${INSTALLER_OUTPUT}\vcredist_2008_x64.exe"
	ExecWait '"$OUTDIR\vcredist_2008_x64.exe" /q:a"'
!else
	File /r "${INSTALLER_OUTPUT}\vcredist_2008_x86.exe"
	ExecWait '"$OUTDIR\vcredist_2008_x86.exe" /q:a'
!endif

CheckVC2010Install:
;
; http://blogs.msdn.com/b/astebner/archive/2010/05/05/10008146.aspx
;
!if ${CPU} = "x64"
	SetRegView 64
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64" "Installed"
    IfErrors InstallVC2010Redist BeginInstall
!else
	SetRegView 32
    ; Reg key for 32-bit OS
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86" "Installed"
    IfErrors CheckVS2010Wow32Registry BeginInstall
	
CheckVS2010Wow32Registry:	
	SetRegView 64
    ; Reg key for 64-bit OS (WoW32)
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86" "Installed"
    IfErrors InstallVC2010Redist BeginInstall
!endif

InstallVC2010Redist:
	MessageBox MB_OK "Installing Visual C++ 2010 redist"
!if ${CPU} = "x64"
	File /r "${INSTALLER_OUTPUT}\vcredist_2010_x64.exe"
	ExecWait '"$OUTDIR\vcredist_2010_x64.exe" /passive /norestart'
!else
	File /r "${INSTALLER_OUTPUT}\vcredist_2010_x86.exe"
	ExecWait '"$OUTDIR\vcredist_2010_x86.exe" /passive /norestart'
!endif

BeginInstall:
	Banner::show /NOUNLOAD "Extracting files. Please Wait"
	Banner::getWindow /NOUNLOAD
	File "${INSTALLER_OUTPUT}\${OUTNAME}.msi"
	Banner::destroy
	; Run the MGOS installer
	ExecWait '"msiexec" /i "$OUTDIR\${OUTNAME}.msi"'
	; Delete the MGOS installer and any other extracted after completion
	Delete "$OUTDIR\${OUTNAME}.msi"
!if ${CPU} = "x64"
	Delete "$OUTDIR\vcredist_2008_x64.exe"
    Delete "$OUTDIR\vcredist_2010_x64.exe"
!else
	Delete "$OUTDIR\vcredist_2008_x86.exe"
    Delete "$OUTDIR\vcredist_2010_x86.exe"
!endif
	
SectionEnd