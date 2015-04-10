!include "MUI2.nsh"

!define BASENAME "FOE ClanManager"
Name "${BASENAME}"

# define installer name
OutFile "install.exe"

# set desktop as install directory
InstallDir "$PROGRAMFILES\FOE KlanManager"

Var STARTMENU_FOLDER
Var MUI_TEMP

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP_NOSTRETCH
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_STRECH
;!define MUI_HEADERIMAGE_BITMAP "logo.bmp"
;!define MUI_ICON "hmc_icon.ico"
;!define MUI_UNICON "hmc_icon.ico"


;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_STARTMENU "Application" $STARTMENU_FOLDER

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


;--------------------------------
;Languages
 
!insertmacro MUI_LANGUAGE "Danish"


;--------------------------------
;Installer Sections

Section "Install"
	SetOutPath "$INSTDIR"
	File ../win32-build\FOE.exe
	File ../win32-build\*.qm
	File ../win32-build\icudt53.dll
	File ../win32-build\icuin53.dll
	File ../win32-build/icuuc53.dll
	File ../win32-build\libgcc_s_dw2-1.dll
	File ../win32-build\libstdc++-6.dll
	File ../win32-build\libwinpthread-1.dll
	File ../win32-build\Qt5Core.dll
	File ../win32-build\Qt5Network.dll
	File ../win32-build\Qt5Widgets.dll
	File ../win32-build\Qt5Gui.dll
	File ../win32-build\Qt5Sql.dll

	SetOutPath "$INSTDIR\platforms"
	File ../win32-build\qwindows.dll

	SetOutPath "$INSTDIR\sqldrivers"
	File ../win32-build\qsqlite.dll

	SetOutPath "$INSTDIR"

	WriteUninstaller $INSTDIR\Uninstall.exe
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
		CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\${BASENAME}.lnk" "$INSTDIR\FOE.exe"
		CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Afinstaller ${BASENAME}.lnk" "$INSTDIR\Uninstall.exe"
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd
 


Section "Uninstall" 
   RMDir /r "$INSTDIR"
   !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
   RMDir /r "$SMPROGRAMS\$MUI_TEMP"
   RMDir "$SMPROGRAMS\${BASENAME}"
SectionEnd