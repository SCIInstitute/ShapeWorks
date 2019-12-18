; ShapeWorks.nsi


;--------------------------------s
; Include Modern UI

!include "MUI2.nsh"
!include "x64.nsh"

CRCCheck On
Unicode true
ManifestDPIAware true

;--------------------------------

;--------------------------------
; Initialization function to properly set the installation directory
Function .onInit
  ${If} ${RunningX64}
    StrCpy $INSTDIR "$PROGRAMFILES64\ShapeWorks"
    SetRegView 64
  ${EndIf}
FunctionEnd

;--------------------------------
; General

; The name of the installer
Name "ShapeWorks-$%SW_VERSION%"

; The file to write
OutFile "ShapeWorks-$%SW_VERSION%-windows.exe"

; The default installation directory
InstallDir $PROGRAMFILES64\ShapeWorks

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\ShapeWorks" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

;--------------------------------
;Interface Settings
  !define MUI_ABORTWARNING
 
;--------------------------------
;Installer Pages
  !define MUI_FINISHPAGE_SHOWREADME "Windows_README.txt"
  !define MUI_FINISHPAGE_SHOWREADME_TEXT "Show README"
  !define MUI_FINISHPAGE_TITLE "Installation Complete"
  !define MUI_FINISHPAGE_TEXT "ShapeWorks Installation is complete, please take a moment to read the README."
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
  
;--------------------------------

; The stuff to install
Section "ShapeWorks (required)"
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "conda_installs.bat"
  File "Windows_README.txt"
  File /r "bin"
  File /r "Examples"

  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\ShapeWorks "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ShapeWorks" "DisplayName" "ShapeWorks"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ShapeWorks" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ShapeWorks" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ShapeWorks" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\ShapeWorks"
  CreateShortcut "$SMPROGRAMS\ShapeWorks\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\ShapeWorks\ShapeWorksStudio.lnk" "$INSTDIR\bin\ShapeWorksStudio.exe" "" "$INSTDIR\bin\ShapeWorksStudio.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ShapeWorks"
  DeleteRegKey HKLM SOFTWARE\ShapeWorks

  ; Delete Files 
  RMDir /r "$INSTDIR\*.*"   

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\ShapeWorks\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\ShapeWorks"
  RMDir "$INSTDIR"

SectionEnd
