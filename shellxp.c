#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <tchar.h>
#include <string.h>
#include <shlobj.h>

/*
 * gcc -shared -Wl,--kill-at,--enable-stdcall-fixup,-s -D_UNICODE -DUNICODE
 * -O shellxp.def -o shellxp.dll shellxp.c -lole32 -luuid
 */
 
HINSTANCE hmodule = NULL;
HINSTANCE shell = NULL;
HINSTANCE shlwapi = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        shell = LoadLibrary(_T("SHELL32.dll"));
        shlwapi = LoadLibrary(_T("SHLWAPI.dll"));
        if (!shell) return FALSE;
        if (!shlwapi) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(shell);
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(shlwapi);
    return TRUE;
}

//redefine funcs
#define strlenW wcslen
#define strcpyW wcscpy

//supress msg
#define TRACE(...) do { } while(0)
#define WARN(...) do { } while(0)
#define FIXME(...) do { } while(0)

//rename funcs
#define SHGetStockIconInfo WineSHGetStockIconInfo
#define SHGetFolderPathEx WineSHGetFolderPathEx
#define SHGetKnownFolderPath WineSHGetKnownFolderPath

/*
 *	shell icon cache (SIC)
 *
 * Copyright 1998, 1999 Juergen Schmied
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
 #define IDI_SHELL_DOCUMENT           1
/****************************************************************************
 * SHGetStockIconInfo [SHELL32.@]
 *
 * Receive information for builtin icons
 *
 * PARAMS
 *  id      [I]  selected icon-id to get information for
 *  flags   [I]  selects the information to receive
 *  sii     [IO] SHSTOCKICONINFO structure to fill
 *
 * RETURNS
 *  Success: S_OK
 *  Failure: A HRESULT failure code
 *
 */
HRESULT WINAPI SHGetStockIconInfo(SHSTOCKICONID id, UINT flags, SHSTOCKICONINFO *sii)
{
    static const WCHAR shell32dll[] = {'\\','s','h','e','l','l','3','2','.','d','l','l',0};

    FIXME("(%d, 0x%x, %p) semi-stub\n", id, flags, sii);
    if ((id < 0) || (id >= SIID_MAX_ICONS) || !sii || (sii->cbSize != sizeof(SHSTOCKICONINFO))) {
        return E_INVALIDARG;
    }

    GetSystemDirectoryW(sii->szPath, MAX_PATH);

    /* no icons defined: use default */
    sii->iIcon = -IDI_SHELL_DOCUMENT;
    lstrcatW(sii->szPath, shell32dll);

    if (flags)
        FIXME("flags 0x%x not implemented\n", flags);

    sii->hIcon = NULL;
    sii->iSysImageIndex = -1;

    TRACE("%3d: returning %s (%d)\n", id, debugstr_w(sii->szPath), sii->iIcon);

    return S_OK;
}

/*
 * Path Functions
 *
 * Copyright 1998, 1999, 2000 Juergen Schmied
 * Copyright 2004 Juan Lang
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 * NOTES:
 *
 * Many of these functions are in SHLWAPI.DLL also
 *
 */
static const WCHAR szCategory[] = {'C','a','t','e','g','o','r','y',0};
static const WCHAR szAttributes[] = {'A','t','t','r','i','b','u','t','e','s',0};
static const WCHAR szName[] = {'N','a','m','e',0};
static const WCHAR szParsingName[] = {'P','a','r','s','i','n','g','N','a','m','e',0};
static const WCHAR szRelativePath[] = {'R','e','l','a','t','i','v','e','P','a','t','h',0};
static const WCHAR szParentFolder[] = {'P','a','r','e','n','t','F','o','l','d','e','r',0};

static const WCHAR szCurrentVersion[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\0'};
static const WCHAR AddNewProgramsFolderW[] = {'A','d','d','N','e','w','P','r','o','g','r','a','m','s','F','o','l','d','e','r',0};
static const WCHAR AppUpdatesFolderW[] = {'A','p','p','U','p','d','a','t','e','s','F','o','l','d','e','r',0};
static const WCHAR Administrative_ToolsW[] = {'A','d','m','i','n','i','s','t','r','a','t','i','v','e',' ','T','o','o','l','s','\0'};
static const WCHAR AppDataW[] = {'A','p','p','D','a','t','a','\0'};
static const WCHAR AppData_RoamingW[] = {'A','p','p','D','a','t','a','\\','R','o','a','m','i','n','g','\0'};
static const WCHAR AppData_LocalLowW[] = {'A','p','p','D','a','t','a','\\','L','o','c','a','l','L','o','w','\0'};
static const WCHAR AppData_LocalW[] = {'A','p','p','D','a','t','a','\\','L','o','c','a','l','\0'};
static const WCHAR Application_DataW[] = {'A','p','p','l','i','c','a','t','i','o','n',' ','D','a','t','a','\0'};
static const WCHAR CacheW[] = {'C','a','c','h','e','\0'};
static const WCHAR CD_BurningW[] = {'C','D',' ','B','u','r','n','i','n','g','\0'};
static const WCHAR ChangeRemoveProgramsFolderW[] = {'C','h','a','n','g','e','R','e','m','o','v','e','P','r','o','g','r','a','m','s','F','o','l','d','e','r',0};
static const WCHAR CommonW[] = {'C','o','m','m','o','n',0};
static const WCHAR Common_Administrative_ToolsW[] = {'C','o','m','m','o','n',' ','A','d','m','i','n','i','s','t','r','a','t','i','v','e',' ','T','o','o','l','s','\0'};
static const WCHAR Common_AppDataW[] = {'C','o','m','m','o','n',' ','A','p','p','D','a','t','a','\0'};
static const WCHAR Common_DesktopW[] = {'C','o','m','m','o','n',' ','D','e','s','k','t','o','p','\0'};
static const WCHAR Common_DocumentsW[] = {'C','o','m','m','o','n',' ','D','o','c','u','m','e','n','t','s','\0'};
static const WCHAR CommonDownloadsW[] = {'C','o','m','m','o','n','D','o','w','n','l','o','a','d','s',0};
static const WCHAR Common_FavoritesW[] = {'C','o','m','m','o','n',' ','F','a','v','o','r','i','t','e','s','\0'};
static const WCHAR CommonFilesDirW[] = {'C','o','m','m','o','n','F','i','l','e','s','D','i','r','\0'};
static const WCHAR CommonFilesDirX86W[] = {'C','o','m','m','o','n','F','i','l','e','s','D','i','r',' ','(','x','8','6',')','\0'};
static const WCHAR CommonMusicW[] = {'C','o','m','m','o','n','M','u','s','i','c','\0'};
static const WCHAR CommonPicturesW[] = {'C','o','m','m','o','n','P','i','c','t','u','r','e','s','\0'};
static const WCHAR Common_ProgramsW[] = {'C','o','m','m','o','n',' ','P','r','o','g','r','a','m','s','\0'};
static const WCHAR CommonRingtonesW[] = {'C','o','m','m','o','n','R','i','n','g','t','o','n','e','s',0};
static const WCHAR Common_StartUpW[] = {'C','o','m','m','o','n',' ','S','t','a','r','t','U','p','\0'};
static const WCHAR Common_StartupW[] = {'C','o','m','m','o','n',' ','S','t','a','r','t','u','p','\0'};
static const WCHAR Common_Start_MenuW[] = {'C','o','m','m','o','n',' ','S','t','a','r','t',' ','M','e','n','u','\0'};
static const WCHAR Common_TemplatesW[] = {'C','o','m','m','o','n',' ','T','e','m','p','l','a','t','e','s','\0'};
static const WCHAR CommonVideoW[] = {'C','o','m','m','o','n','V','i','d','e','o','\0'};
static const WCHAR ConflictFolderW[] = {'C','o','n','f','l','i','c','t','F','o','l','d','e','r',0};
static const WCHAR ConnectionsFolderW[] = {'C','o','n','n','e','c','t','i','o','n','s','F','o','l','d','e','r',0};
static const WCHAR ContactsW[] = {'C','o','n','t','a','c','t','s','\0'};
static const WCHAR ControlPanelFolderW[] = {'C','o','n','t','r','o','l','P','a','n','e','l','F','o','l','d','e','r',0};
static const WCHAR CookiesW[] = {'C','o','o','k','i','e','s','\0'};
static const WCHAR CSCFolderW[] = {'C','S','C','F','o','l','d','e','r',0};
static const WCHAR Default_GadgetsW[] = {'D','e','f','a','u','l','t',' ','G','a','d','g','e','t','s',0};
static const WCHAR DesktopW[] = {'D','e','s','k','t','o','p','\0'};
static const WCHAR Device_Metadata_StoreW[] = {'D','e','v','i','c','e',' ','M','e','t','a','d','a','t','a',' ','S','t','o','r','e',0};
static const WCHAR DocumentsW[] = {'D','o','c','u','m','e','n','t','s','\0'};
static const WCHAR DocumentsLibraryW[] = {'D','o','c','u','m','e','n','t','s','L','i','b','r','a','r','y','\0'};
static const WCHAR Documents_librarymsW[] = {'D','o','c','u','m','e','n','t','s','.','l','i','b','r','a','r','y','-','m','s',0};
static const WCHAR DownloadsW[] = {'D','o','w','n','l','o','a','d','s','\0'};
static const WCHAR FavoritesW[] = {'F','a','v','o','r','i','t','e','s','\0'};
static const WCHAR FontsW[] = {'F','o','n','t','s','\0'};
static const WCHAR GadgetsW[] = {'G','a','d','g','e','t','s',0};
static const WCHAR GamesW[] = {'G','a','m','e','s',0};
static const WCHAR GameTasksW[] = {'G','a','m','e','T','a','s','k','s',0};
static const WCHAR HistoryW[] = {'H','i','s','t','o','r','y','\0'};
static const WCHAR HomeGroupFolderW[] = {'H','o','m','e','G','r','o','u','p','F','o','l','d','e','r',0};
static const WCHAR ImplicitAppShortcutsW[] = {'I','m','p','l','i','c','i','t','A','p','p','S','h','o','r','t','c','u','t','s',0};
static const WCHAR InternetFolderW[] = {'I','n','t','e','r','n','e','t','F','o','l','d','e','r',0};
static const WCHAR LibrariesW[] = {'L','i','b','r','a','r','i','e','s',0};
static const WCHAR LinksW[] = {'L','i','n','k','s','\0'};
static const WCHAR Local_AppDataW[] = {'L','o','c','a','l',' ','A','p','p','D','a','t','a','\0'};
static const WCHAR Local_Settings_Application_DataW[] = {'L','o','c','a','l',' ','S','e','t','t','i','n','g','s','\\','A','p','p','l','i','c','a','t','i','o','n',' ','D','a','t','a','\0'};
static const WCHAR Local_Settings_CD_BurningW[] = {'L','o','c','a','l',' ','S','e','t','t','i','n','g','s','\\','A','p','p','l','i','c','a','t','i','o','n',' ','D','a','t','a','\\','M','i','c','r','o','s','o','f','t','\\','C','D',' ','B','u','r','n','i','n','g','\0'};
static const WCHAR Local_Settings_HistoryW[] = {'L','o','c','a','l',' ','S','e','t','t','i','n','g','s','\\','H','i','s','t','o','r','y','\0'};
static const WCHAR Local_Settings_Temporary_Internet_FilesW[] = {'L','o','c','a','l',' ','S','e','t','t','i','n','g','s','\\','T','e','m','p','o','r','a','r','y',' ','I','n','t','e','r','n','e','t',' ','F','i','l','e','s','\0'};
static const WCHAR LocalAppDataLowW[] = {'L','o','c','a','l','A','p','p','D','a','t','a','L','o','w',0};
static const WCHAR LocalizedResourcesDirW[] = {'L','o','c','a','l','i','z','e','d','R','e','s','o','u','r','c','e','s','D','i','r',0};
static const WCHAR MAPIFolderW[] = {'M','A','P','I','F','o','l','d','e','r',0};
static const WCHAR Microsoft_Internet_Explorer_Quick_LaunchW[] = {'M','i','c','r','o','s','o','f','t','\\','I','n','t','e','r','n','e','t',' ','E','x','p','l','o','r','e','r','\\','Q','u','i','c','k',' ','L','a','u','n','c','h',0};
static const WCHAR Microsoft_Windows_Burn_BurnW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','B','u','r','n','\\','B','u','r','n',0};
static const WCHAR Microsoft_Windows_CookiesW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','C','o','o','k','i','e','s',0};
static const WCHAR Microsoft_Windows_GameExplorerW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','G','a','m','e','E','x','p','l','o','r','e','r','\0'};
static const WCHAR Microsoft_Windows_DeviceMetadataStoreW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','D','e','v','i','c','e','M','e','t','a','d','a','t','a','S','t','o','r','e',0};
static const WCHAR Microsoft_Windows_HistoryW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','H','i','s','t','o','r','y',0};
static const WCHAR Microsoft_Windows_LibrariesW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','L','i','b','r','a','r','i','e','s','\0'};
static const WCHAR Microsoft_Windows_Network_ShortcutsW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','N','e','t','w','o','r','k',' ','S','h','o','r','t','c','u','t','s',0};
static const WCHAR Microsoft_Windows_Photo_Gallery_Original_ImagesW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s',' ','P','h','o','t','o',' ','G','a','l','l','e','r','y','\\','O','r','i','g','i','n','a','l',' ','I','m','a','g','e','s',0};
static const WCHAR Microsoft_Windows_Printer_ShortcutsW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','P','r','i','n','t','e','r',' ','S','h','o','r','t','c','u','t','s',0};
static const WCHAR Microsoft_Windows_RecentW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','R','e','c','e','n','t','\0'};
static const WCHAR Microsoft_Windows_RingtonesW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','R','i','n','g','t','o','n','e','s','\0'};
static const WCHAR Microsoft_Windows_SendToW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','S','e','n','d','T','o',0};
static const WCHAR Microsoft_Windows_Sidebar_GadgetsW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s',' ','S','i','d','e','b','a','r','\\','G','a','d','g','e','t','s',0};
static const WCHAR Microsoft_Windows_Start_MenuW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','S','t','a','r','t',' ','M','e','n','u',0};
static const WCHAR Microsoft_Windows_TemplatesW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','T','e','m','p','l','a','t','e','s',0};
static const WCHAR Microsoft_Windows_Temporary_Internet_FilesW[] = {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','T','e','m','p','o','r','a','r','y',' ','I','n','t','e','r','n','e','t',' ','F','i','l','e','s',0};
static const WCHAR Microsoft_Windows_ThemesW[] =  {'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','T','h','e','m','e','s',0};
static const WCHAR MoviesW[] = {'M','o','v','i','e','s','\0'};
static const WCHAR MusicW[] = {'M','u','s','i','c','\0'};
static const WCHAR MusicLibraryW[] = {'M','u','s','i','c','L','i','b','r','a','r','y',0};
static const WCHAR Music_librarymsW[] = {'M','u','s','i','c','.','l','i','b','r','a','r','y','-','m','s',0};
static const WCHAR Music_PlaylistsW[] = {'M','u','s','i','c','\\','P','l','a','y','l','i','s','t','s','\0'};
static const WCHAR Music_Sample_MusicW[] = {'M','u','s','i','c','\\','S','a','m','p','l','e',' ','M','u','s','i','c','\0'};
static const WCHAR Music_Sample_PlaylistsW[] = {'M','u','s','i','c','\\','S','a','m','p','l','e',' ','P','l','a','y','l','i','s','t','s','\0'};
static const WCHAR My_MusicW[] = {'M','y',' ','M','u','s','i','c','\0'};
static const WCHAR My_PicturesW[] = {'M','y',' ','P','i','c','t','u','r','e','s','\0'};
static const WCHAR My_VideosW[] = {'M','y',' ','V','i','d','e','o','s','\0'};
static const WCHAR My_VideoW[] = {'M','y',' ','V','i','d','e','o','\0'};
static const WCHAR MyComputerFolderW[] = {'M','y','C','o','m','p','u','t','e','r','F','o','l','d','e','r',0};
static const WCHAR NetHoodW[] = {'N','e','t','H','o','o','d','\0'};
static const WCHAR NetworkPlacesFolderW[] = {'N','e','t','w','o','r','k','P','l','a','c','e','s','F','o','l','d','e','r',0};
static const WCHAR OEM_LinksW[] = {'O','E','M',' ','L','i','n','k','s','\0'};
static const WCHAR Original_ImagesW[] = {'O','r','i','g','i','n','a','l',' ','I','m','a','g','e','s',0};
static const WCHAR PersonalW[] = {'P','e','r','s','o','n','a','l','\0'};
static const WCHAR PhotoAlbumsW[] = {'P','h','o','t','o','A','l','b','u','m','s',0};
static const WCHAR PicturesW[] = {'P','i','c','t','u','r','e','s','\0'};
static const WCHAR PicturesLibraryW[] = {'P','i','c','t','u','r','e','s','L','i','b','r','a','r','y',0};
static const WCHAR Pictures_librarymsW[] = {'P','i','c','t','u','r','e','s','.','l','i','b','r','a','r','y','-','m','s',0};
static const WCHAR Pictures_Sample_PicturesW[] = {'P','i','c','t','u','r','e','s','\\','S','a','m','p','l','e',' ','P','i','c','t','u','r','e','s','\0'};
static const WCHAR Pictures_Slide_ShowsW[] = {'P','i','c','t','u','r','e','s','\\','S','l','i','d','e',' ','S','h','o','w','s','\0'};
static const WCHAR PlaylistsW[] = {'P','l','a','y','l','i','s','t','s',0};
static const WCHAR PrintersFolderW[] = {'P','r','i','n','t','e','r','s','F','o','l','d','e','r',0};
static const WCHAR PrintHoodW[] = {'P','r','i','n','t','H','o','o','d','\0'};
static const WCHAR ProfileW[] = {'P','r','o','f','i','l','e',0};
static const WCHAR Program_FilesW[] = {'P','r','o','g','r','a','m',' ','F','i','l','e','s','\0'};
static const WCHAR ProgramFilesW[] = {'P','r','o','g','r','a','m','F','i','l','e','s','\0'};
static const WCHAR ProgramFilesX86W[] = {'P','r','o','g','r','a','m','F','i','l','e','s','X','8','6','\0'};
static const WCHAR ProgramFilesX64W[] = {'P','r','o','g','r','a','m','F','i','l','e','s','X','6','4','\0'};
static const WCHAR Program_Files_Common_FilesW[] = {'P','r','o','g','r','a','m',' ','F','i','l','e','s','\\','C','o','m','m','o','n',' ','F','i','l','e','s','\0'};
static const WCHAR Program_Files_x86W[] = {'P','r','o','g','r','a','m',' ','F','i','l','e','s',' ','(','x','8','6',')','\0'};
static const WCHAR Program_Files_x86_Common_FilesW[] = {'P','r','o','g','r','a','m',' ','F','i','l','e','s',' ','(','x','8','6',')','\\','C','o','m','m','o','n',' ','F','i','l','e','s','\0'};
static const WCHAR ProgramFilesCommonW[] = {'P','r','o','g','r','a','m','F','i','l','e','s','C','o','m','m','o','n',0};
static const WCHAR ProgramFilesCommonX86W[] = {'P','r','o','g','r','a','m','F','i','l','e','s','C','o','m','m','o','n','X','8','6',0};
static const WCHAR ProgramFilesCommonX64W[] = {'P','r','o','g','r','a','m','F','i','l','e','s','C','o','m','m','o','n','X','6','4',0};
static const WCHAR ProgramFilesDirW[] = {'P','r','o','g','r','a','m','F','i','l','e','s','D','i','r','\0'};
static const WCHAR ProgramFilesDirX86W[] = {'P','r','o','g','r','a','m','F','i','l','e','s','D','i','r',' ','(','x','8','6',')','\0'};
static const WCHAR ProgramsW[] = {'P','r','o','g','r','a','m','s','\0'};
static const WCHAR PublicW[] = {'P','u','b','l','i','c',0};
static const WCHAR PublicGameTasksW[] = {'P','u','b','l','i','c','G','a','m','e','T','a','s','k','s',0};
static const WCHAR PublicLibrariesW[] = {'P','u','b','l','i','c','L','i','b','r','a','r','i','e','s',0};
static const WCHAR Quick_LaunchW[] = {'Q','u','i','c','k',' ','L','a','u','n','c','h',0};
static const WCHAR RecentW[] = {'R','e','c','e','n','t','\0'};
static const WCHAR RecordedTVLibraryW[] = {'R','e','c','o','r','d','e','d','T','V','L','i','b','r','a','r','y',0};
static const WCHAR RecordedTV_librarymsW[] = {'R','e','c','o','r','d','e','d','T','V','.','l','i','b','r','a','r','y','-','m','s',0};
static const WCHAR RecycleBinFolderW[] = {'R','e','c','y','c','l','e','B','i','n','F','o','l','d','e','r',0};
static const WCHAR ResourceDirW[] = {'R','e','s','o','u','r','c','e','D','i','r','\0'};
static const WCHAR ResourcesW[] = {'R','e','s','o','u','r','c','e','s','\0'};
static const WCHAR RingtonesW[] = {'R','i','n','g','t','o','n','e','s',0};
static const WCHAR SampleMusicW[] = {'S','a','m','p','l','e','M','u','s','i','c',0};
static const WCHAR Sample_MusicW[] = {'S','a','m','p','l','e',' ','M','u','s','i','c',0};
static const WCHAR SamplePicturesW[] = {'S','a','m','p','l','e','P','i','c','t','u','r','e','s',0};
static const WCHAR Sample_PicturesW[] = {'S','a','m','p','l','e',' ','P','i','c','t','u','r','e','s',0};
static const WCHAR SamplePlaylistsW[] = {'S','a','m','p','l','e','P','l','a','y','l','i','s','t','s',0};
static const WCHAR Sample_PlaylistsW[] = {'S','a','m','p','l','e',' ','P','l','a','y','l','i','s','t','s',0};
static const WCHAR Sample_VideosW[] = {'S','a','m','p','l','e',' ','V','i','d','e','o','s',0};
static const WCHAR SampleVideosW[] = {'S','a','m','p','l','e','V','i','d','e','o','s',0};
static const WCHAR Saved_GamesW[] = {'S','a','v','e','d',' ','G','a','m','e','s','\0'};
static const WCHAR SavedGamesW[] = {'S','a','v','e','d','G','a','m','e','s','\0'};
static const WCHAR SearchesW[] = {'S','e','a','r','c','h','e','s','\0'};
static const WCHAR SearchHomeFolderW[] = {'S','e','a','r','c','h','H','o','m','e','F','o','l','d','e','r',0};
static const WCHAR SendToW[] = {'S','e','n','d','T','o','\0'};
static const WCHAR Slide_ShowsW[] = {'S','l','i','d','e',' ','S','h','o','w','s',0};
static const WCHAR StartUpW[] = {'S','t','a','r','t','U','p','\0'};
static const WCHAR StartupW[] = {'S','t','a','r','t','u','p','\0'};
static const WCHAR Start_MenuW[] = {'S','t','a','r','t',' ','M','e','n','u','\0'};
static const WCHAR Start_Menu_ProgramsW[] = {'S','t','a','r','t',' ','M','e','n','u','\\','P','r','o','g','r','a','m','s','\0'};
static const WCHAR Start_Menu_Admin_ToolsW[] = {'S','t','a','r','t',' ','M','e','n','u','\\','P','r','o','g','r','a','m','s','\\','A','d','m','i','n','i','s','t','r','a','t','i','v','e',' ','T','o','o','l','s','\0'};
static const WCHAR Start_Menu_StartupW[] = {'S','t','a','r','t',' ','M','e','n','u','\\','P','r','o','g','r','a','m','s','\\','S','t','a','r','t','U','p','\0'};
static const WCHAR SyncCenterFolderW[] = {'S','y','n','c','C','e','n','t','e','r','F','o','l','d','e','r',0};
static const WCHAR SyncResultsFolderW[] = {'S','y','n','c','R','e','s','u','l','t','s','F','o','l','d','e','r',0};
static const WCHAR SyncSetupFolderW[] = {'S','y','n','c','S','e','t','u','p','F','o','l','d','e','r',0};
static const WCHAR SystemW[] = {'S','y','s','t','e','m',0};
static const WCHAR SystemX86W[] = {'S','y','s','t','e','m','X','8','6',0};
static const WCHAR TemplatesW[] = {'T','e','m','p','l','a','t','e','s','\0'};
static const WCHAR User_PinnedW[] = {'U','s','e','r',' ','P','i','n','n','e','d',0};
static const WCHAR UsersW[] = {'U','s','e','r','s','\0'};
static const WCHAR UsersFilesFolderW[] = {'U','s','e','r','s','F','i','l','e','s','F','o','l','d','e','r',0};
static const WCHAR UsersLibrariesFolderW[] = {'U','s','e','r','s','L','i','b','r','a','r','i','e','s','F','o','l','d','e','r',0};
static const WCHAR UserProfilesW[] = {'U','s','e','r','P','r','o','f','i','l','e','s',0};
static const WCHAR UserProgramFilesW[] = {'U','s','e','r','P','r','o','g','r','a','m','F','i','l','e','s',0};
static const WCHAR UserProgramFilesCommonW[] = {'U','s','e','r','P','r','o','g','r','a','m','F','i','l','e','s','C','o','m','m','o','n',0};
static const WCHAR UsersPublicW[] = {'U','s','e','r','s','\\','P','u','b','l','i','c','\0'};
static const WCHAR VideosW[] = {'V','i','d','e','o','s','\0'};
static const WCHAR VideosLibraryW[] = {'V','i','d','e','o','s','L','i','b','r','a','r','y',0};
static const WCHAR Videos_librarymsW[] = {'V','i','d','e','o','s','.','l','i','b','r','a','r','y','-','m','s',0};
static const WCHAR Videos_Sample_VideosW[] = {'V','i','d','e','o','s','\\','S','a','m','p','l','e',' ','V','i','d','e','o','s','\0'};
static const WCHAR WindowsW[] = {'W','i','n','d','o','w','s',0};
static const WCHAR Windows_Sidebar_GadgetsW[] = {'W','i','n','d','o','w','s',' ','S','i','d','e','b','a','r','\\','G','a','d','g','e','t','s',0};
static const WCHAR DefaultW[] = {'.','D','e','f','a','u','l','t','\0'};
static const WCHAR AllUsersProfileW[] = {'%','A','L','L','U','S','E','R','S','P','R','O','F','I','L','E','%','\0'};
static const WCHAR UserProfileW[] = {'%','U','S','E','R','P','R','O','F','I','L','E','%','\0'};
static const WCHAR SystemDriveW[] = {'%','S','y','s','t','e','m','D','r','i','v','e','%','\0'};
static const WCHAR ProfileListW[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s',' ','N','T','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','P','r','o','f','i','l','e','L','i','s','t',0};
static const WCHAR ProfilesDirectoryW[] = {'P','r','o','f','i','l','e','s','D','i','r','e','c','t','o','r','y',0};
static const WCHAR AllUsersProfileValueW[] = {'A','l','l','U','s','e','r','s','P','r','o','f','i','l','e','\0'};
static const WCHAR szSHFolders[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','E','x','p','l','o','r','e','r','\\','S','h','e','l','l',' ','F','o','l','d','e','r','s','\0'};
static const WCHAR szSHUserFolders[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','E','x','p','l','o','r','e','r','\\','U','s','e','r',' ','S','h','e','l','l',' ','F','o','l','d','e','r','s','\0'};
static const WCHAR szDefaultProfileDirW[] = {'u','s','e','r','s',0};
static const WCHAR szKnownFolderDescriptions[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','E','x','p','l','o','r','e','r','\\','F','o','l','d','e','r','D','e','s','c','r','i','p','t','i','o','n','s','\0'};
static const WCHAR szKnownFolderRedirections[] = {'S','o','f','t','w','a','r','e','\\','M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\','C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\','E','x','p','l','o','r','e','r','\\','U','s','e','r',' ','S','h','e','l','l',' ','F','o','l','d','e','r','s',0};
static const WCHAR AllUsersW[] = {'P','u','b','l','i','c',0};

#define CHANGEREMOVEPROGRAMS_PARSING_GUID '{','7','b','8','1','b','e','6','a','-','c','e','2','b','-','4','6','7','6','-','a','2','9','e','-','e','b','9','0','7','a','5','1','2','6','c','5','}'
#define SYNCMANAGER_PARSING_GUID '{','9','C','7','3','F','5','E','5','-','7','A','E','7','-','4','E','3','2','-','A','8','E','8','-','8','D','2','3','B','8','5','2','5','5','B','F','}'
#define SYSTEMFOLDERS_PARSING_GUID '{','2','1','E','C','2','0','2','0','-','3','A','E','A','-','1','0','6','9','-','A','2','D','D','-','0','8','0','0','2','B','3','0','3','0','9','D','}'
#define USERFOLDERS_PARSING_GUID '{','5','9','0','3','1','a','4','7','-','3','f','7','2','-','4','4','a','7','-','8','9','c','5','-','5','5','9','5','f','e','6','b','3','0','e','e','}'
#define USERSLIBRARIES_PARSING_GUID '{','0','3','1','E','4','8','2','5','-','7','B','9','4','-','4','d','c','3','-','B','1','3','1','-','E','9','4','6','B','4','4','C','8','D','D','5','}'

static const WCHAR ComputerFolderParsingNameW[] = {':',':','{','2','0','D','0','4','F','E','0','-','3','A','E','A','-','1','0','6','9','-','A','2','D','8','-','0','8','0','0','2','B','3','0','3','0','9','D','}',0};
static const WCHAR ControlPanelFolderParsingNameW[] = {':',':','{','2','6','E','E','0','6','6','8','-','A','0','0','A','-','4','4','D','7','-','9','3','7','1','-','B','E','B','0','6','4','C','9','8','6','8','3','}','\\','0',0};
static const WCHAR ControlPanelFolderRelativePathW[] = {':',':','{','2','1','E','C','2','0','2','0','-','3','A','E','A','-','1','0','6','9','-','A','2','D','D','-','0','8','0','0','2','B','3','0','3','0','9','D','}',0};
static const WCHAR GamesParsingNameW[] = {':',':','{','E','D','2','2','8','F','D','F','-','9','E','A','8','-','4','8','7','0','-','8','3','b','1','-','9','6','b','0','2','C','F','E','0','D','5','2','}',0};
static const WCHAR HomeGroupParsingNameW[] = {':',':','{','B','4','F','B','3','F','9','8','-','C','1','E','A','-','4','2','8','d','-','A','7','8','A','-','D','1','F','5','6','5','9','C','B','A','9','3','}',0};
static const WCHAR InternetFolderParsingNameW[] = {':',':','{','8','7','1','C','5','3','8','0','-','4','2','A','0','-','1','0','6','9','-','A','2','E','A','-','0','8','0','0','2','B','3','0','3','0','9','D','}',0};
static const WCHAR NetworkFolderParsingNameW[] = {':',':','{','F','0','2','C','1','A','0','D','-','B','E','2','1','-','4','3','5','0','-','8','8','B','0','-','7','3','6','7','F','C','9','6','E','F','3','C','}',0};
static const WCHAR PublicParsingNameW[] = {':',':','{','4','3','3','6','a','5','4','d','-','0','3','8','b','-','4','6','8','5','-','a','b','0','2','-','9','9','b','b','5','2','d','3','f','b','8','b','}',0};
static const WCHAR RecycleBinFolderParsingNameW[] = {':',':','{','6','4','5','F','F','0','4','0','-','5','0','8','1','-','1','0','1','B','-','9','F','0','8','-','0','0','A','A','0','0','2','F','9','5','4','E','}',0};
static const WCHAR SearchHomeParsingNameW[] = {':',':','{','9','3','4','3','8','1','2','e','-','1','c','3','7','-','4','a','4','9','-','a','1','2','e','-','4','b','2','d','8','1','0','d','9','5','6','b','}',0};
static const WCHAR SEARCH_CSCParsingNameW[] = {'s','h','e','l','l',':',':',':','{','B','D','7','A','2','E','7','B','-','2','1','C','B','-','4','1','b','2','-','A','0','8','6','-','B','3','0','9','6','8','0','C','6','B','7','E','}','\\','*',0};
static const WCHAR SEARCH_MAPIParsingNameW[] = {'s','h','e','l','l',':',':',':','{','8','9','D','8','3','5','7','6','-','6','B','D','1','-','4','C','8','6','-','9','4','5','4','-','B','E','B','0','4','E','9','4','C','8','1','9','}','\\','*',0};
static const WCHAR UsersFilesParsingNameW[] = {':',':','{','5','9','0','3','1','a','4','7','-','3','f','7','2','-','4','4','a','7','-','8','9','c','5','-','5','5','9','5','f','e','6','b','3','0','e','e','}',0};
static const WCHAR UsersLibrariesParsingNameW[] = {':',':','{','0','3','1','E','4','8','2','5','-','7','B','9','4','-','4','d','c','3','-','B','1','3','1','-','E','9','4','6','B','4','4','C','8','D','D','5','}',0};
static const WCHAR AddNewProgramsParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':','{','1','5','e','a','e','9','2','e','-','f','1','7','a','-','4','4','3','1','-','9','f','2','8','-','8','0','5','e','4','8','2','d','a','f','d','4','}',0};
static const WCHAR ConnectionsFolderParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':','{','7','0','0','7','A','C','C','7','-','3','2','0','2','-','1','1','D','1','-','A','A','D','2','-','0','0','8','0','5','F','C','1','2','7','0','E','}',0};
static const WCHAR PrintersFolderParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':','{','2','2','2','7','A','2','8','0','-','3','A','E','A','-','1','0','6','9','-','A','2','D','E','-','0','8','0','0','2','B','3','0','3','0','9','D','}',0};
static const WCHAR ChangeRemoveProgramsParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':', CHANGEREMOVEPROGRAMS_PARSING_GUID, 0};
static const WCHAR AppUpdatesParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':', CHANGEREMOVEPROGRAMS_PARSING_GUID, '\\',':',':','{','d','4','5','0','a','8','a','1','-','9','5','6','8','-','4','5','c','7','-','9','c','0','e','-','b','4','f','9','f','b','4','5','3','7','b','d','}',0};
static const WCHAR SyncManagerFolderParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':', SYNCMANAGER_PARSING_GUID, 0};
static const WCHAR ConflictFolderParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':', SYNCMANAGER_PARSING_GUID, '\\',':',':','{','E','4','1','3','D','0','4','0','-','6','7','8','8','-','4','C','2','2','-','9','5','7','E','-','1','7','5','D','1','C','5','1','3','A','3','4','}',',',0};
static const WCHAR SyncResultsFolderParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':', SYNCMANAGER_PARSING_GUID, '\\',':',':','{','B','C','4','8','B','3','2','F','-','5','9','1','0','-','4','7','F','5','-','8','5','7','0','-','5','0','7','4','A','8','A','5','6','3','6','A','}',',',0};
static const WCHAR SyncSetupFolderParsingNameW[] = {':',':', SYSTEMFOLDERS_PARSING_GUID, '\\',':',':', SYNCMANAGER_PARSING_GUID, '\\',':',':','{','F','1','3','9','0','A','9','A','-','A','3','F','4','-','4','E','5','D','-','9','C','5','F','-','9','8','F','3','B','D','8','D','9','3','5','C','}',',',0};
static const WCHAR ContactsParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','5','6','7','8','4','8','5','4','-','C','6','C','B','-','4','6','2','B','-','8','1','6','9','-','8','8','E','3','5','0','A','C','B','8','8','2','}',0};
static const WCHAR DocumentsParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','F','D','D','3','9','A','D','0','-','2','3','8','F','-','4','6','A','F','-','A','D','B','4','-','6','C','8','5','4','8','0','3','6','9','C','7','}',0};
static const WCHAR LinksParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','b','f','b','9','d','5','e','0','-','c','6','a','9','-','4','0','4','c','-','b','2','b','2','-','a','e','6','d','b','6','a','f','4','9','6','8','}',0};
static const WCHAR MusicParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','4','B','D','8','D','5','7','1','-','6','D','1','9','-','4','8','D','3','-','B','E','9','7','-','4','2','2','2','2','0','0','8','0','E','4','3','}',0};
static const WCHAR PicturesParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','3','3','E','2','8','1','3','0','-','4','E','1','E','-','4','6','7','6','-','8','3','5','A','-','9','8','3','9','5','C','3','B','C','3','B','B','}',0};
static const WCHAR SavedGamesParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','4','C','5','C','3','2','F','F','-','B','B','9','D','-','4','3','b','0','-','B','5','B','4','-','2','D','7','2','E','5','4','E','A','A','A','4','}',0};
static const WCHAR SavedSearchesParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','7','d','1','d','3','a','0','4','-','d','e','b','b','-','4','1','1','5','-','9','5','c','f','-','2','f','2','9','d','a','2','9','2','0','d','a','}',0};
static const WCHAR VideosParsingNameW[] = {':',':', USERFOLDERS_PARSING_GUID, '\\','{','1','8','9','8','9','B','1','D','-','9','9','B','5','-','4','5','5','B','-','8','4','1','C','-','A','B','7','C','7','4','E','4','D','D','F','C','}',0};
static const WCHAR DocumentsLibraryParsingNameW[] = {':',':', USERSLIBRARIES_PARSING_GUID, '\\','{','7','b','0','d','b','1','7','d','-','9','c','d','2','-','4','a','9','3','-','9','7','3','3','-','4','6','c','c','8','9','0','2','2','e','7','c','}',0};
static const WCHAR MusicLibraryParsingNameW[] = {':',':', USERSLIBRARIES_PARSING_GUID, '\\','{','2','1','1','2','A','B','0','A','-','C','8','6','A','-','4','f','f','e','-','A','3','6','8','-','0','D','E','9','6','E','4','7','0','1','2','E','}',0};
static const WCHAR PicturesLibraryParsingNameW[] = {':',':', USERSLIBRARIES_PARSING_GUID, '\\','{','A','9','9','0','A','E','9','F','-','A','0','3','B','-','4','e','8','0','-','9','4','B','C','-','9','9','1','2','D','7','5','0','4','1','0','4','}',0};
static const WCHAR VideosLibraryParsingNameW[] = {':',':', USERSLIBRARIES_PARSING_GUID, '\\','{','4','9','1','E','9','2','2','F','-','5','6','4','3','-','4','a','f','4','-','A','7','E','B','-','4','E','7','A','1','3','8','D','8','1','7','4','}',0};

typedef enum _CSIDL_Type {
    CSIDL_Type_User,
    CSIDL_Type_AllUsers,
    CSIDL_Type_CurrVer,
    CSIDL_Type_Disallowed,
    CSIDL_Type_NonExistent,
    CSIDL_Type_WindowsPath,
    CSIDL_Type_SystemPath,
    CSIDL_Type_SystemX86Path,
} CSIDL_Type;

#define CSIDL_CONTACTS         0x0043
#define CSIDL_DOWNLOADS        0x0047
#define CSIDL_LINKS            0x004d
#define CSIDL_APPDATA_LOCALLOW 0x004e
#define CSIDL_SAVED_GAMES      0x0062
#define CSIDL_SEARCHES         0x0063

#define IDS_DESKTOPDIRECTORY        54
#define IDS_PERSONAL                46
#define IDS_MYMUSIC                 52
#define IDS_MYVIDEOS                53
#define IDS_MYPICTURES              64

typedef struct
{
    const KNOWNFOLDERID *id;
    CSIDL_Type type;
    LPCWSTR    szValueName;
    LPCWSTR    szDefaultPath; /* fallback string or resource ID */

    /* KNOWNFOLDER_DEFINITION fields */
    KF_CATEGORY category;
    const WCHAR *pszName;
    const WCHAR *pszDescription;
    const KNOWNFOLDERID *fidParent;
    const WCHAR *pszRelativePath;
    const WCHAR *pszParsingName;
    const WCHAR *pszTooltip;
    const WCHAR *pszLocalizedName;
    const WCHAR *pszIcon;
    const WCHAR *pszSecurity;
    DWORD dwAttributes;
    KF_DEFINITION_FLAGS kfdFlags;
    const FOLDERTYPEID *ftidType;
} CSIDL_DATA;

static const CSIDL_DATA CSIDL_Data[] =
{
    { /* 0x00 - CSIDL_DESKTOP */
        &FOLDERID_Desktop,
        CSIDL_Type_User,
        DesktopW,
        MAKEINTRESOURCEW(IDS_DESKTOPDIRECTORY),

        KF_CATEGORY_PERUSER, /* category */
        DesktopW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        DesktopW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x01 - CSIDL_INTERNET */
        &FOLDERID_InternetFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        InternetFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        InternetFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x02 - CSIDL_PROGRAMS */
        &FOLDERID_Programs,
        CSIDL_Type_User,
        ProgramsW,
        Start_Menu_ProgramsW,

        KF_CATEGORY_PERUSER, /* category */
        ProgramsW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        ProgramsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x03 - CSIDL_CONTROLS (.CPL files) */
        &FOLDERID_ControlPanelFolder,
        CSIDL_Type_SystemPath,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        ControlPanelFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        ControlPanelFolderRelativePathW, /* relative path */
        ControlPanelFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x04 - CSIDL_PRINTERS */
        &FOLDERID_PrintersFolder,
        CSIDL_Type_SystemPath,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        PrintersFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        PrintersFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x05 - CSIDL_PERSONAL */
        &FOLDERID_Documents,
        CSIDL_Type_User,
        PersonalW,
        MAKEINTRESOURCEW(IDS_PERSONAL),

        KF_CATEGORY_PERUSER, /* category */
        PersonalW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        DocumentsW, /* relative path */
        DocumentsParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x06 - CSIDL_FAVORITES */
        &FOLDERID_Favorites,
        CSIDL_Type_User,
        FavoritesW,
        FavoritesW,

        KF_CATEGORY_PERUSER, /* category */
        FavoritesW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        FavoritesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x07 - CSIDL_STARTUP */
        &FOLDERID_Startup,
        CSIDL_Type_User,
        StartUpW,
        Start_Menu_StartupW,

        KF_CATEGORY_PERUSER, /* category */
        StartupW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        StartUpW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x08 - CSIDL_RECENT */
        &FOLDERID_Recent,
        CSIDL_Type_User,
        RecentW,
        RecentW,

        KF_CATEGORY_PERUSER, /* category */
        RecentW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_RecentW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x09 - CSIDL_SENDTO */
        &FOLDERID_SendTo,
        CSIDL_Type_User,
        SendToW,
        SendToW,

        KF_CATEGORY_PERUSER, /* category */
        SendToW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_SendToW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x0a - CSIDL_BITBUCKET - Recycle Bin */
        &FOLDERID_RecycleBinFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        RecycleBinFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        RecycleBinFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x0b - CSIDL_STARTMENU */
        &FOLDERID_StartMenu,
        CSIDL_Type_User,
        Start_MenuW,
        Start_MenuW,

        KF_CATEGORY_PERUSER, /* category */
        Start_MenuW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_Start_MenuW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x0c - CSIDL_MYDOCUMENTS */
        &GUID_NULL,
        CSIDL_Type_Disallowed, /* matches WinXP--can't get its path */
        NULL,
        NULL
    },
    { /* 0x0d - CSIDL_MYMUSIC */
        &FOLDERID_Music,
        CSIDL_Type_User,
        My_MusicW,
        MAKEINTRESOURCEW(IDS_MYMUSIC),

        KF_CATEGORY_PERUSER, /* category */
        My_MusicW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        MusicW, /* relative path */
        MusicParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x0e - CSIDL_MYVIDEO */
        &FOLDERID_Videos,
        CSIDL_Type_User,
        My_VideosW,
        MAKEINTRESOURCEW(IDS_MYVIDEOS),

        KF_CATEGORY_PERUSER, /* category */
        My_VideoW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        VideosW, /* relative path */
        VideosParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x0f - unassigned */
        &GUID_NULL,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,
    },
    { /* 0x10 - CSIDL_DESKTOPDIRECTORY */
        &FOLDERID_Desktop,
        CSIDL_Type_User,
        DesktopW,
        MAKEINTRESOURCEW(IDS_DESKTOPDIRECTORY),

        KF_CATEGORY_PERUSER, /* category */
        DesktopW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        DesktopW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x11 - CSIDL_DRIVES */
        &FOLDERID_ComputerFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        MyComputerFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        ComputerFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x12 - CSIDL_NETWORK */
        &FOLDERID_NetworkFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        NetworkPlacesFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NetworkFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x13 - CSIDL_NETHOOD */
        &FOLDERID_NetHood,
        CSIDL_Type_User,
        NetHoodW,
        NetHoodW,

        KF_CATEGORY_PERUSER, /* category */
        NetHoodW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_Network_ShortcutsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x14 - CSIDL_FONTS */
        &FOLDERID_Fonts,
        CSIDL_Type_WindowsPath,
        FontsW,
        FontsW,

        KF_CATEGORY_FIXED, /* category */
        FontsW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &FOLDERID_Windows/* typeid */
    },
    { /* 0x15 - CSIDL_TEMPLATES */
        &FOLDERID_Templates,
        CSIDL_Type_User,
        TemplatesW,
        TemplatesW,

        KF_CATEGORY_PERUSER, /* category */
        TemplatesW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_TemplatesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x16 - CSIDL_COMMON_STARTMENU */
        &FOLDERID_CommonStartMenu,
        CSIDL_Type_AllUsers,
        Common_Start_MenuW,
        Start_MenuW,

        KF_CATEGORY_COMMON, /* category */
        Common_Start_MenuW, /* name */
        NULL, /* description */
        &FOLDERID_ProgramData, /* parent */
        Microsoft_Windows_Start_MenuW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x17 - CSIDL_COMMON_PROGRAMS */
        &FOLDERID_CommonPrograms,
        CSIDL_Type_AllUsers,
        Common_ProgramsW,
        Start_Menu_ProgramsW,

        KF_CATEGORY_COMMON, /* category */
        Common_ProgramsW, /* name */
        NULL, /* description */
        &FOLDERID_CommonStartMenu, /* parent */
        ProgramsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x18 - CSIDL_COMMON_STARTUP */
        &FOLDERID_CommonStartup,
        CSIDL_Type_AllUsers,
        Common_StartUpW,
        Start_Menu_StartupW,

        KF_CATEGORY_COMMON, /* category */
        Common_StartupW, /* name */
        NULL, /* description */
        &FOLDERID_CommonPrograms, /* parent */
        StartUpW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x19 - CSIDL_COMMON_DESKTOPDIRECTORY */
        &FOLDERID_PublicDesktop,
        CSIDL_Type_AllUsers,
        Common_DesktopW,
        MAKEINTRESOURCEW(IDS_DESKTOPDIRECTORY),

        KF_CATEGORY_COMMON, /* category */
        Common_DesktopW, /* name */
        NULL, /* description */
        &FOLDERID_Public, /* parent */
        DesktopW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x1a - CSIDL_APPDATA */
        &FOLDERID_RoamingAppData,
        CSIDL_Type_User,
        AppDataW,
        Application_DataW,

        KF_CATEGORY_PERUSER, /* category */
        AppDataW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        AppData_RoamingW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x1b - CSIDL_PRINTHOOD */
        &FOLDERID_PrintHood,
        CSIDL_Type_User,
        PrintHoodW,
        PrintHoodW,

        KF_CATEGORY_PERUSER, /* category */
        PrintHoodW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_Printer_ShortcutsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x1c - CSIDL_LOCAL_APPDATA */
        &FOLDERID_LocalAppData,
        CSIDL_Type_User,
        Local_AppDataW,
        Local_Settings_Application_DataW,

        KF_CATEGORY_PERUSER, /* category */
        Local_AppDataW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        AppData_LocalW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_LOCAL_REDIRECT_ONLY | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x1d - CSIDL_ALTSTARTUP */
        &GUID_NULL,
        CSIDL_Type_NonExistent,
        NULL,
        NULL
    },
    { /* 0x1e - CSIDL_COMMON_ALTSTARTUP */
        &GUID_NULL,
        CSIDL_Type_NonExistent,
        NULL,
        NULL
    },
    { /* 0x1f - CSIDL_COMMON_FAVORITES */
        &FOLDERID_Favorites,
        CSIDL_Type_AllUsers,
        Common_FavoritesW,
        FavoritesW,

        KF_CATEGORY_PERUSER, /* category */
        FavoritesW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        FavoritesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x20 - CSIDL_INTERNET_CACHE */
        &FOLDERID_InternetCache,
        CSIDL_Type_User,
        CacheW,
        Local_Settings_Temporary_Internet_FilesW,

        KF_CATEGORY_PERUSER, /* category */
        CacheW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        Microsoft_Windows_Temporary_Internet_FilesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_LOCAL_REDIRECT_ONLY, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x21 - CSIDL_COOKIES */
        &FOLDERID_Cookies,
        CSIDL_Type_User,
        CookiesW,
        CookiesW,

        KF_CATEGORY_PERUSER, /* category */
        CookiesW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_CookiesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x22 - CSIDL_HISTORY */
        &FOLDERID_History,
        CSIDL_Type_User,
        HistoryW,
        Local_Settings_HistoryW,

        KF_CATEGORY_PERUSER, /* category */
        HistoryW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        Microsoft_Windows_HistoryW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_LOCAL_REDIRECT_ONLY, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x23 - CSIDL_COMMON_APPDATA */
        &FOLDERID_ProgramData,
        CSIDL_Type_AllUsers,
        Common_AppDataW,
        Application_DataW,

        KF_CATEGORY_FIXED, /* category */
        Common_AppDataW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x24 - CSIDL_WINDOWS */
        &FOLDERID_Windows,
        CSIDL_Type_WindowsPath,
        NULL,
        NULL,

        KF_CATEGORY_FIXED, /* category */
        WindowsW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x25 - CSIDL_SYSTEM */
        &FOLDERID_System,
        CSIDL_Type_SystemPath,
        NULL,
        NULL,

        KF_CATEGORY_FIXED, /* category */
        SystemW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x26 - CSIDL_PROGRAM_FILES */
        &FOLDERID_ProgramFiles,
        CSIDL_Type_CurrVer,
        ProgramFilesDirW,
        Program_FilesW,

        KF_CATEGORY_FIXED, /* category */
        ProgramFilesW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x27 - CSIDL_MYPICTURES */
        &FOLDERID_Pictures,
        CSIDL_Type_User,
        My_PicturesW,
        MAKEINTRESOURCEW(IDS_MYPICTURES),

        KF_CATEGORY_PERUSER, /* category */
        My_PicturesW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        PicturesW, /* relative path */
        PicturesParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x28 - CSIDL_PROFILE */
        &FOLDERID_Profile,
        CSIDL_Type_User,
        NULL,
        NULL,

        KF_CATEGORY_FIXED, /* category */
        ProfileW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x29 - CSIDL_SYSTEMX86 */
        &FOLDERID_SystemX86,
        CSIDL_Type_SystemX86Path,
        NULL,
        NULL,

        KF_CATEGORY_FIXED, /* category */
        SystemX86W, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x2a - CSIDL_PROGRAM_FILESX86 */
        &FOLDERID_ProgramFilesX86,
        CSIDL_Type_CurrVer,
        ProgramFilesDirX86W,
        Program_Files_x86W,

        KF_CATEGORY_FIXED, /* category */
        ProgramFilesX86W, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x2b - CSIDL_PROGRAM_FILES_COMMON */
        &FOLDERID_ProgramFilesCommon,
        CSIDL_Type_CurrVer,
        CommonFilesDirW,
        Program_Files_Common_FilesW,

        KF_CATEGORY_FIXED, /* category */
        ProgramFilesCommonW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x2c - CSIDL_PROGRAM_FILES_COMMONX86 */
        &FOLDERID_ProgramFilesCommonX86,
        CSIDL_Type_CurrVer,
        CommonFilesDirX86W,
        Program_Files_x86_Common_FilesW,

        KF_CATEGORY_FIXED, /* category */
        ProgramFilesCommonX86W, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x2d - CSIDL_COMMON_TEMPLATES */
        &FOLDERID_CommonTemplates,
        CSIDL_Type_AllUsers,
        Common_TemplatesW,
        TemplatesW,

        KF_CATEGORY_COMMON, /* category */
        Common_TemplatesW, /* name */
        NULL, /* description */
        &FOLDERID_ProgramData, /* parent */
        Microsoft_Windows_TemplatesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x2e - CSIDL_COMMON_DOCUMENTS */
        &FOLDERID_PublicDocuments,
        CSIDL_Type_AllUsers,
        Common_DocumentsW,
        DocumentsW,

        KF_CATEGORY_COMMON, /* category */
        Common_DocumentsW, /* name */
        NULL, /* description */
        &FOLDERID_Public, /* parent */
        DocumentsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x2f - CSIDL_COMMON_ADMINTOOLS */
        &FOLDERID_CommonAdminTools,
        CSIDL_Type_AllUsers,
        Common_Administrative_ToolsW,
        Start_Menu_Admin_ToolsW,

        KF_CATEGORY_COMMON, /* category */
        Common_Administrative_ToolsW, /* name */
        NULL, /* description */
        &FOLDERID_CommonPrograms, /* parent */
        Administrative_ToolsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x30 - CSIDL_ADMINTOOLS */
        &FOLDERID_AdminTools,
        CSIDL_Type_User,
        Administrative_ToolsW,
        Start_Menu_Admin_ToolsW,

        KF_CATEGORY_PERUSER, /* category */
        Administrative_ToolsW, /* name */
        NULL, /* description */
        &FOLDERID_Programs, /* parent */
        Administrative_ToolsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x31 - CSIDL_CONNECTIONS */
        &FOLDERID_ConnectionsFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        ConnectionsFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        Administrative_ToolsW, /* relative path */
        ConnectionsFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x32 - unassigned */
        &GUID_NULL,
        CSIDL_Type_Disallowed,
        NULL,
        NULL
    },
    { /* 0x33 - unassigned */
        &GUID_NULL,
        CSIDL_Type_Disallowed,
        NULL,
        NULL
    },
    { /* 0x34 - unassigned */
        &GUID_NULL,
        CSIDL_Type_Disallowed,
        NULL,
        NULL
    },
    { /* 0x35 - CSIDL_COMMON_MUSIC */
        &FOLDERID_PublicMusic,
        CSIDL_Type_AllUsers,
        CommonMusicW,
        MusicW,

        KF_CATEGORY_COMMON, /* category */
        CommonMusicW, /* name */
        NULL, /* description */
        &FOLDERID_Public, /* parent */
        MusicW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x36 - CSIDL_COMMON_PICTURES */
        &FOLDERID_PublicPictures,
        CSIDL_Type_AllUsers,
        CommonPicturesW,
        PicturesW,

        KF_CATEGORY_COMMON, /* category */
        CommonPicturesW, /* name */
        NULL, /* description */
        &FOLDERID_Public, /* parent */
        PicturesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x37 - CSIDL_COMMON_VIDEO */
        &FOLDERID_PublicVideos,
        CSIDL_Type_AllUsers,
        CommonVideoW,
        VideosW,

        KF_CATEGORY_COMMON, /* category */
        CommonVideoW, /* name */
        NULL, /* description */
        &FOLDERID_Public, /* parent */
        VideosW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x38 - CSIDL_RESOURCES */
        &FOLDERID_ResourceDir,
        CSIDL_Type_WindowsPath,
        NULL,
        ResourcesW,

        KF_CATEGORY_FIXED, /* category */
        ResourceDirW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x39 - CSIDL_RESOURCES_LOCALIZED */
        &FOLDERID_LocalizedResourcesDir,
        CSIDL_Type_NonExistent,
        NULL,
        NULL,

        KF_CATEGORY_FIXED, /* category */
        LocalizedResourcesDirW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x3a - CSIDL_COMMON_OEM_LINKS */
        &FOLDERID_CommonOEMLinks,
        CSIDL_Type_AllUsers,
        NULL,
        OEM_LinksW,

        KF_CATEGORY_COMMON, /* category */
        OEM_LinksW, /* name */
        NULL, /* description */
        &FOLDERID_ProgramData, /* parent */
        OEM_LinksW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x3b - CSIDL_CDBURN_AREA */
        &FOLDERID_CDBurning,
        CSIDL_Type_User,
        CD_BurningW,
        Local_Settings_CD_BurningW,

        KF_CATEGORY_PERUSER, /* category */
        CD_BurningW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        Microsoft_Windows_Burn_BurnW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_LOCAL_REDIRECT_ONLY, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x3c unassigned */
        &GUID_NULL,
        CSIDL_Type_Disallowed,
        NULL,
        NULL
    },
    { /* 0x3d - CSIDL_COMPUTERSNEARME */
        &GUID_NULL,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL
    },
    { /* 0x3e - CSIDL_PROFILES */
        &GUID_NULL,
        CSIDL_Type_Disallowed, /* oddly, this matches WinXP */
        NULL,
        NULL
    },
    { /* 0x3f */
        &FOLDERID_AddNewPrograms,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        AddNewProgramsFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        AddNewProgramsParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x40 */
        &FOLDERID_AppUpdates,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        AppUpdatesFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        AppUpdatesParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x41 */
        &FOLDERID_ChangeRemovePrograms,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        ChangeRemoveProgramsFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        ChangeRemoveProgramsParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x42 */
        &FOLDERID_ConflictFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        ConflictFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        ConflictFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x43 - CSIDL_CONTACTS */
        &FOLDERID_Contacts,
        CSIDL_Type_User,
        NULL,
        ContactsW,

        KF_CATEGORY_PERUSER, /* category */
        ContactsW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        ContactsW, /* relative path */
        ContactsParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x44 */
        &FOLDERID_DeviceMetadataStore,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_COMMON, /* category */
        Device_Metadata_StoreW, /* name */
        NULL, /* description */
        &FOLDERID_ProgramData, /* parent */
        Microsoft_Windows_DeviceMetadataStoreW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x45 */
        &GUID_NULL,
        CSIDL_Type_User,
        NULL,
        DocumentsW
    },
    { /* 0x46 */
        &FOLDERID_DocumentsLibrary,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        DocumentsLibraryW, /* name */
        NULL, /* description */
        &FOLDERID_Libraries, /* parent */
        Documents_librarymsW, /* relative path */
        DocumentsLibraryParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE | KFDF_STREAM, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x47 - CSIDL_DOWNLOADS */
        &FOLDERID_Downloads,
        CSIDL_Type_User,
        NULL,
        DownloadsW,

        KF_CATEGORY_PERUSER, /* category */
        DownloadsW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        DownloadsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x48 */
        &FOLDERID_Games,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        GamesW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        GamesParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x49 */
        &FOLDERID_GameTasks,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        GameTasksW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        Microsoft_Windows_GameExplorerW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_LOCAL_REDIRECT_ONLY, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x4a */
        &FOLDERID_HomeGroup,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        HomeGroupFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        HomeGroupParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x4b */
        &FOLDERID_ImplicitAppShortcuts,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        ImplicitAppShortcutsW, /* name */
        NULL, /* description */
        &FOLDERID_UserPinned, /* parent */
        ImplicitAppShortcutsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x4c */
        &FOLDERID_Libraries,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        LibrariesW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Windows_LibrariesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x4d - CSIDL_LINKS */
        &FOLDERID_Links,
        CSIDL_Type_User,
        NULL,
        LinksW,

        KF_CATEGORY_PERUSER, /* category */
        LinksW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        LinksW, /* relative path */
        LinksParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x4e - CSIDL_APPDATA_LOCALLOW */
        &FOLDERID_LocalAppDataLow,
        CSIDL_Type_User,
        NULL,
        AppData_LocalLowW,

        KF_CATEGORY_PERUSER, /* category */
        LocalAppDataLowW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        AppData_LocalLowW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, /* attributes */
        KFDF_LOCAL_REDIRECT_ONLY | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x4f */
        &FOLDERID_MusicLibrary,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        MusicLibraryW, /* name */
        NULL, /* description */
        &FOLDERID_Libraries, /* parent */
        Music_librarymsW, /* relative path */
        MusicLibraryParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE | KFDF_STREAM, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x50 */
        &FOLDERID_OriginalImages,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        Original_ImagesW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        Microsoft_Windows_Photo_Gallery_Original_ImagesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x51 */
        &FOLDERID_PhotoAlbums,
        CSIDL_Type_User,
        NULL,
        Pictures_Slide_ShowsW,

        KF_CATEGORY_PERUSER, /* category */
        PhotoAlbumsW, /* name */
        NULL, /* description */
        &FOLDERID_Pictures, /* parent */
        Slide_ShowsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x52 */
        &FOLDERID_PicturesLibrary,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        PicturesLibraryW, /* name */
        NULL, /* description */
        &FOLDERID_Libraries, /* parent */
        Pictures_librarymsW, /* relative path */
        PicturesLibraryParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE | KFDF_STREAM, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x53 */
        &FOLDERID_Playlists,
        CSIDL_Type_User,
        NULL,
        Music_PlaylistsW,

        KF_CATEGORY_PERUSER, /* category */
        PlaylistsW, /* name */
        NULL, /* description */
        &FOLDERID_Music, /* parent */
        PlaylistsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x54 */
        &FOLDERID_ProgramFilesX64,
        CSIDL_Type_NonExistent,
        NULL,
        NULL,

        KF_CATEGORY_FIXED, /* category */
        ProgramFilesX64W, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x55 */
        &FOLDERID_ProgramFilesCommonX64,
        CSIDL_Type_NonExistent,
        NULL,
        NULL,

        KF_CATEGORY_FIXED, /* category */
        ProgramFilesCommonX64W, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x56 */
        &FOLDERID_Public,
        CSIDL_Type_CurrVer, /* FIXME */
        NULL,
        UsersPublicW,

        KF_CATEGORY_FIXED, /* category */
        PublicW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        PublicParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x57 */
        &FOLDERID_PublicDownloads,
        CSIDL_Type_AllUsers,
        NULL,
        DownloadsW,

        KF_CATEGORY_COMMON, /* category */
        CommonDownloadsW, /* name */
        NULL, /* description */
        &FOLDERID_Public, /* parent */
        DownloadsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x58 */
        &FOLDERID_PublicGameTasks,
        CSIDL_Type_AllUsers,
        NULL,
        Microsoft_Windows_GameExplorerW,

        KF_CATEGORY_COMMON, /* category */
        PublicGameTasksW, /* name */
        NULL, /* description */
        &FOLDERID_ProgramData, /* parent */
        Microsoft_Windows_GameExplorerW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_LOCAL_REDIRECT_ONLY, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x59 */
        &FOLDERID_PublicLibraries,
        CSIDL_Type_AllUsers,
        NULL,
        Microsoft_Windows_LibrariesW,

        KF_CATEGORY_COMMON, /* category */
        PublicLibrariesW, /* name */
        NULL, /* description */
        &FOLDERID_Public, /* parent */
        LibrariesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x5a */
        &FOLDERID_PublicRingtones,
        CSIDL_Type_AllUsers,
        NULL,
        Microsoft_Windows_RingtonesW,

        KF_CATEGORY_COMMON, /* category */
        CommonRingtonesW, /* name */
        NULL, /* description */
        &FOLDERID_ProgramData, /* parent */
        Microsoft_Windows_RingtonesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x5b */
        &FOLDERID_QuickLaunch,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        Quick_LaunchW, /* name */
        NULL, /* description */
        &FOLDERID_RoamingAppData, /* parent */
        Microsoft_Internet_Explorer_Quick_LaunchW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x5c */
        &FOLDERID_RecordedTVLibrary,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_COMMON, /* category */
        RecordedTVLibraryW, /* name */
        NULL, /* description */
        &FOLDERID_PublicLibraries, /* parent */
        RecordedTV_librarymsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE | KFDF_STREAM, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x5d */
        &FOLDERID_Ringtones,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        RingtonesW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        Microsoft_Windows_RingtonesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x5e */
        &FOLDERID_SampleMusic,
        CSIDL_Type_AllUsers,
        NULL,
        Music_Sample_MusicW,

        KF_CATEGORY_COMMON, /* category */
        SampleMusicW, /* name */
        NULL, /* description */
        &FOLDERID_PublicMusic, /* parent */
        Sample_MusicW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x5f */
        &FOLDERID_SamplePictures,
        CSIDL_Type_AllUsers,
        NULL,
        Pictures_Sample_PicturesW,

        KF_CATEGORY_COMMON, /* category */
        SamplePicturesW, /* name */
        NULL, /* description */
        &FOLDERID_PublicPictures, /* parent */
        Sample_PicturesW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x60 */
        &FOLDERID_SamplePlaylists,
        CSIDL_Type_AllUsers,
        NULL,
        Music_Sample_PlaylistsW,

        KF_CATEGORY_COMMON, /* category */
        SamplePlaylistsW, /* name */
        NULL, /* description */
        &FOLDERID_PublicMusic, /* parent */
        Sample_PlaylistsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x61 */
        &FOLDERID_SampleVideos,
        CSIDL_Type_AllUsers,
        NULL,
        Videos_Sample_VideosW,

        KF_CATEGORY_COMMON, /* category */
        SampleVideosW, /* name */
        NULL, /* description */
        &FOLDERID_PublicVideos, /* parent */
        Sample_VideosW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x62 - CSIDL_SAVED_GAMES */
        &FOLDERID_SavedGames,
        CSIDL_Type_User,
        NULL,
        Saved_GamesW,

        KF_CATEGORY_PERUSER, /* category */
        SavedGamesW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        Saved_GamesW, /* relative path */
        SavedGamesParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_ROAMABLE | KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x63 - CSIDL_SEARCHES */
        &FOLDERID_SavedSearches,
        CSIDL_Type_User,
        NULL,
        SearchesW,

        KF_CATEGORY_PERUSER, /* category */
        SearchesW, /* name */
        NULL, /* description */
        &FOLDERID_Profile, /* parent */
        SearchesW, /* relative path */
        SavedSearchesParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE | KFDF_PUBLISHEXPANDEDPATH, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x64 */
        &FOLDERID_SEARCH_CSC,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        CSCFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        SEARCH_CSCParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x65 */
        &FOLDERID_SEARCH_MAPI,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        MAPIFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        SEARCH_MAPIParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x66 */
        &FOLDERID_SearchHome,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        SearchHomeFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        SearchHomeParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x67 */
        &FOLDERID_SidebarDefaultParts,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_COMMON, /* category */
        Default_GadgetsW, /* name */
        NULL, /* description */
        &FOLDERID_ProgramFiles, /* parent */
        Windows_Sidebar_GadgetsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x68 */
        &FOLDERID_SidebarParts,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        GadgetsW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        Microsoft_Windows_Sidebar_GadgetsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x69 */
        &FOLDERID_SyncManagerFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        SyncCenterFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        SyncManagerFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x6a */
        &FOLDERID_SyncResultsFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        SyncResultsFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        SyncResultsFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x6b */
        &FOLDERID_SyncSetupFolder,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        SyncSetupFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        SyncSetupFolderParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x6c */
        &FOLDERID_UserPinned,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        User_PinnedW, /* name */
        NULL, /* description */
        &FOLDERID_QuickLaunch, /* parent */
        User_PinnedW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_HIDDEN, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x6d */
        &FOLDERID_UserProfiles,
        CSIDL_Type_CurrVer,
        UsersW,
        UsersW,

        KF_CATEGORY_FIXED, /* category */
        UserProfilesW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        FILE_ATTRIBUTE_READONLY, /* attributes */
        KFDF_PRECREATE, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x6e */
        &FOLDERID_UserProgramFiles,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        UserProgramFilesW, /* name */
        NULL, /* description */
        &FOLDERID_LocalAppData, /* parent */
        ProgramsW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x6f */
        &FOLDERID_UserProgramFilesCommon,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        UserProgramFilesCommonW, /* name */
        NULL, /* description */
        &FOLDERID_UserProgramFiles, /* parent */
        CommonW, /* relative path */
        NULL, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x70 */
        &FOLDERID_UsersFiles,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        UsersFilesFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        UsersFilesParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x71 */
        &FOLDERID_UsersLibraries,
        CSIDL_Type_Disallowed,
        NULL,
        NULL,

        KF_CATEGORY_VIRTUAL, /* category */
        UsersLibrariesFolderW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        NULL, /* relative path */
        UsersLibrariesParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    },
    { /* 0x72 */
        &FOLDERID_VideosLibrary,
        CSIDL_Type_Disallowed, /* FIXME */
        NULL,
        NULL,

        KF_CATEGORY_PERUSER, /* category */
        VideosLibraryW, /* name */
        NULL, /* description */
        &GUID_NULL, /* parent */
        Videos_librarymsW, /* relative path */
        VideosLibraryParsingNameW, /* parsing */
        NULL, /* tooltip */
        NULL, /* localized */
        NULL, /* icon */
        NULL, /* security */
        0, /* attributes */
        0, /* flags */
        &GUID_NULL /* typeid */
    }
};

static int csidl_from_id( const KNOWNFOLDERID *id )
{
    int i;
    for (i = 0; i < sizeof(CSIDL_Data) / sizeof(CSIDL_Data[0]); i++)
        if (IsEqualGUID( CSIDL_Data[i].id, id )) return i;
    return -1;
}

/*************************************************************************
 * SHGetKnownFolderPath           [SHELL32.@]
 */
HRESULT WINAPI SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD flags, HANDLE token, PWSTR *path)
{
    HRESULT hr;
    WCHAR folder[MAX_PATH];
    int index = csidl_from_id( rfid );

    TRACE("%s, 0x%08x, %p, %p\n", debugstr_guid(rfid), flags, token, path);

    *path = NULL;

    if (index < 0)
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    if (flags & KF_FLAG_CREATE)
        index |= CSIDL_FLAG_CREATE;

    if (flags & KF_FLAG_DONT_VERIFY)
        index |= CSIDL_FLAG_DONT_VERIFY;

    if (flags & KF_FLAG_NO_ALIAS)
        index |= CSIDL_FLAG_NO_ALIAS;

    if (flags & KF_FLAG_INIT)
        index |= CSIDL_FLAG_PER_USER_INIT;

    if (flags & ~(KF_FLAG_CREATE|KF_FLAG_DONT_VERIFY|KF_FLAG_NO_ALIAS|
                  KF_FLAG_INIT|KF_FLAG_DEFAULT_PATH))
    {
        FIXME("flags 0x%08x not supported\n", flags);
        return E_INVALIDARG;
    }

    hr = SHGetFolderPathW( NULL, index, token, (flags & KF_FLAG_DEFAULT_PATH) ?
                           SHGFP_TYPE_DEFAULT : SHGFP_TYPE_CURRENT, folder );
    if (SUCCEEDED(hr))
    {
        *path = CoTaskMemAlloc( (strlenW( folder ) + 1) * sizeof(WCHAR) );
        if (!*path)
            return E_OUTOFMEMORY;
        strcpyW( *path, folder );
    }
    return hr;
}

/*************************************************************************
 * SHGetFolderPathEx           [SHELL32.@]
 */
HRESULT WINAPI SHGetFolderPathEx(REFKNOWNFOLDERID rfid, DWORD flags, HANDLE token, LPWSTR path, DWORD len)
{
    HRESULT hr;
    WCHAR *buffer;

    TRACE("%s, 0x%08x, %p, %p, %u\n", debugstr_guid(rfid), flags, token, path, len);

    if (!path || !len) return E_INVALIDARG;

    hr = SHGetKnownFolderPath( rfid, flags, token, &buffer );
    if (SUCCEEDED( hr ))
    {
        if (strlenW( buffer ) + 1 > len)
        {
            CoTaskMemFree( buffer );
            return HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
        }
        strcpyW( path, buffer );
        CoTaskMemFree( buffer );
    }
    return hr;
}
