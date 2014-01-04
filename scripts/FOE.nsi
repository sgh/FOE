!include "MUI2.nsh"

!define BASENAME "FOE KlanManager"
Name "${BASENAME}"

# define installer name
OutFile "install.exe"

# set desktop as install directory
InstallDir "$PROGRAMFILES\FOE KlanManager"

Var STARTMENU_FOLDER

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
	File win32\archive\release\FOE.exe
	File win32\archive\*.qm
	File win32\icuin51.dll
	File win32\icuuc51.dll
	File win32\libgcc_s_dw2-1.dll
	File win32\libstdc++-6.dll
	File win32\libwinpthread-1.dll
	File win32\Qt5Core.dll
	File win32\Qt5Gui.dll
	File win32\Qt5Sql.dll
	File win32\Qt5Widgets.dll
	File win32\libmysql.dll

	SetOutPath "$INSTDIR\platforms"
	File win32\qwindows.dll

	SetOutPath "$INSTDIR\sqldrivers"
	File win32\qsqlmysql.dll

	SetOutPath "$INSTDIR"

	WriteUninstaller $INSTDIR\Uninstall.exe
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
		CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\${BASENAME}.lnk" "$INSTDIR\FOE.exe"
		CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Afinstaller ${BASENAME}.lnk" "$INSTDIR\Uninstall.exe"
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd
 
Section "Uninstall" 
	Delete $INSTDIR\FOE.exe
	Delete $INSTDIR\icudt51.dll
	Delete $INSTDIR\icuin51.dll
	Delete $INSTDIR\icuuc51.dll
	Delete $INSTDIR\libgcc_s_dw2-1.dll
	Delete $INSTDIR\libstdc++-6.dll
	Delete $INSTDIR\libwinpthread-1.dll
	Delete $INSTDIR\Qt5Core.dll
	Delete $INSTDIR\Qt5Gui.dll
	Delete $INSTDIR\Qt5Sql.dll
	Delete $INSTDIR\Qt5Widgets.dll
	Delete $INSTDIR\libmysql.dll
	Delete $INSTDIR\platforms\qwindows.dll
	Delete $INSTDIR\sqldrivers\qsqlmysql.dll
	Delete $INSTDIR\Uninstall.exe
SectionEnd
