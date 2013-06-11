#ifndef GACUI_DEMO_FILE_SYSTEM_INFORMATION
#define GACUI_DEMO_FILE_SYSTEM_INFORMATION

#include "..\..\Public\Source\GacUIWindows.h"
#include <Windows.h>
#include <ShlObj.h>

using namespace vl;
using namespace vl::collections;

/***********************************************************************
File System Operations
***********************************************************************/

extern WString GetWindowsDirectory();
extern void SearchDirectoriesAndFiles(const WString& path, List<WString>& directories, List<WString>& files);
extern bool IsFileDirectory(const WString& fullPath);
extern Ptr<GuiImageData> GetFileIcon(const WString& fullPath, UINT uFlags);
extern WString GetFileDisplayName(const WString& fullPath);
extern WString GetFileTypeName(const WString& fullPath);
extern FILETIME GetFileLastWriteTime(const WString& fullPath);
extern LARGE_INTEGER GetFileSize(const WString& fullPath);
extern WString FileTimeToString(const FILETIME& filetime);
extern WString FileSizeToString(LARGE_INTEGER filesize);

/***********************************************************************
FileProperties
***********************************************************************/

class FileProperties
{
private:
	Ptr<GuiImageData>	smallIcon;
	Ptr<GuiImageData>	bigIcon;
	bool				isDirectory;
	WString				displayName;
	WString				typeName;
	FILETIME			lastWriteTime;
	LARGE_INTEGER		size;

	bool				loaded;
	WString				fullPath;

	void				Load();
public:
	FileProperties(const WString& _fullPath);

	Ptr<GuiImageData>	GetSmallIcon();
	Ptr<GuiImageData>	GetBigIcon();
	bool				IsDirectory();
	WString				GetDisplayName();
	WString				GetTypeName();
	FILETIME			GetLastWriteTime();
	LARGE_INTEGER		GetSize();
};

#endif