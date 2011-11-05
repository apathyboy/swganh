;SWG:ANH Client Installer 
;Copyright(c) 2010 The SWG:ANH Team

;--------------------------------
;Variables

Var StartMenuFolder
Var errorsrc

;--------------------------------
;Includes

!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "TextFunc.nsh"
!include "includes\Configuration.nsh"

;--------------------------------
;General

;Name and file
Name "SWGANH Server"
OutFile "installer.exe"
ShowInstDetails show
BrandingText " "
 
VIAddVersionKey "ProductName" "SWGANH Server"
VIAddVersionKey "CompanyName" "SWG:ANH"
VIAddVersionKey "LegalCopyright" "Copyright (c) 2010 SWG:ANH"
VIAddVersionKey "FileDescription" "Game server for SWG:ANH based clients."
VIAddVersionKey "FileVersion" "0.3.0.0"
VIProductVersion "0.3.0.0"

;Get installation folder from registry if available
InstallDir "$PROGRAMFILES\SWGANH Server"
InstallDirRegKey HKCU "Software\SWGANH Server" ""

;Request application privileges for Windows Vista/7
RequestExecutionLevel admin


;--------------------------------
;Functions
Function .onInit
FunctionEnd

Function LaunchServer
  SetOutPath "$INSTDIR"
  Exec "$INSTDIR\swganh.exe"
FunctionEnd

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING
  
!define MUI_ICON "misc\swganh.ico"
  
!define MUI_FINISHPAGE
!define MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_NOTCHECKED
  !define MUI_FINISHPAGE_RUN_TEXT "Start the SWG:ANH Server"
  !define MUI_FINISHPAGE_RUN_FUNCTION "LaunchServer"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.markdown"
  !define MUI_FINISHPAGE_SHOWREADME_TEXT "View Readme Document"
!define MUI_FINISHPAGE_LINK "SWG:ANH Community Website"
  !define MUI_FINISHPAGE_LINK_LOCATION "http://swganh.com"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT


;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\..\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

Page custom Configuration ConfigurationValidate

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\SWGANH Server" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"


;--------------------------------
;Installer Sections

Section "SWGANH Server" SecServer
    SetOutPath $INSTDIR
  
    ;Install the client files
    SetOverwrite on
    File "..\..\..\bin\Release\swganh.exe"
    File "..\..\..\bin\Release\*.dll"
    File "..\..\..\README.markdown"
    SetOverwrite off
    File /r "..\..\..\data\*.*"
    
    ;Update the configuration
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "galaxy_name = " "$GalaxyName" $R0
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "connection.address = " "$GalaxyAddress" $R1
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "login.address = " "$GalaxyAddress" $R2
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "galaxy_manager.host = " "$DbHost" $R3
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "galaxy.host = " "$DbHost" $R4
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "galaxy_manager.username = " "$DbUser" $R5
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "galaxy.username = " "$DbUser" $R6
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "galaxy_manager.password = " "$DbPass" $R7
    ${ConfigWrite} "$INSTDIR\config\swganh.cfg" "galaxy.password = " "$DbPass" $R8

    ;Store installation folder
    WriteRegStr HKCU "Software\SWGANH Server" "" $INSTDIR

    ;Create uninstaller
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SWGANH Server" "DisplayName" "SWGANH Server"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SWGANH Server" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SWGANH Server" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SWGANH Server" "NoRepair" 1
    WriteUninstaller "$INSTDIR\uninstall.exe"
  
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    CreateShortCut "$DESKTOP\SWGANH Server.lnk" "$INSTDIR\swganh.exe"
  
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\SWGANH Server.lnk" "$INSTDIR\swganh.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\README.lnk" "$INSTDIR\README.markdown"
    
    !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "Database" SecDatabase 
    DetailPrint "Running import"
    
    ExecWait '"$INSTDIR\sql\setup-alt.bat" /host $DbHost /user $DbUser /pass $DbPass' $0
    StrCmp $0 1 0 endinst
    StrCpy $errorsrc "Wrong login or password"
    Goto abortinst
  
    abortinst:
    DetailPrint "                         "  
    DetailPrint "$\n An error occured ! $\n"
    DetailPrint "  $errorsrc              "
    DetailPrint "                         "
    
    endinst:
SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
    ;Delete any desktop shortcuts that may still exist
    Delete "$DESKTOP\SWGANH Server.lnk"
 
    ;Delete the start menu items and the application installation directory
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
 
    RMDir /r "$SMPROGRAMS\$StartMenuFolder"  
    RMDir /r "$INSTDIR"
  
    ;Clean up the registry
    DeleteRegKey /ifempty HKCU "Software\SWGANH Server"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SWGANH Server"
SectionEnd

;--------------------------------
;Page Descriptions

;Language strings
LangString DESC_SecServer ${LANG_ENGLISH} "The SWG:ANH Server is used to host to SWG:ANH based game clients."
LangString DESC_SecDatabase ${LANG_ENGLISH} "The SWG:ANH database contains all of the required data to run a server instance."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecServer} $(DESC_SecServer)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDatabase} $(DESC_SecDatabase)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
