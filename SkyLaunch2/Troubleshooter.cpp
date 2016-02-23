#include <Windows.h>
#include <string>
#include <vector>
#include <shlobj.h>
#include "Troubleshooter.h"
#include "DEFINE.h"
#include "resource.h"
#include "Extras.h"
#include <direct.h>

#pragma comment(lib, "shell32.lib")

HWND WindowHWND;


using namespace std;


int DeleteDirectory(const std::string &refcstrRootDirectory,
	bool              bDeleteSubdirectories = true)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
	// subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + "\\*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}


void ShowTroubleShooter()
{
	WindowHWND = CreateDialog(glb_hinst, MAKEINTRESOURCE(IDD_TS), 0, WindowControllerTS);
	ShowWindow(WindowHWND, glb_cmdShow);

	MSG  msg;
	int status;
	while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (!IsDialogMessage(WindowHWND, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


void IreneCrash()
{
	CHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

	if (result != S_OK)
	{
		MessageBoxA(NULL, enc("Failed to automatically delete Config Folder\nGo to My Documents -> DragonNest and delete Config folder."), enc("GUIDE"), MB_OK);
	}
	else
	{
		std::string IniPath = my_documents;
					IniPath += "\\DragonNest\\Config\\";

		if (!DeleteDirectory(IniPath.c_str()))
		{
			MessageBoxA(NULL, enc("Fix Succeed\nTry to enter in game."), enc("OK"), MB_OK);
		}	else {
			MessageBoxA(NULL, enc("Failed to automatically delete Config Folder\nGo to My Documents -> DragonNest and delete Config folder."), enc("GUIDE"), MB_OK);
		}
		
	}
}

void InvalidResource()
{
	MessageBoxA(NULL,enc("You can get Invalid Resource from 2 possible causes:\n1.Your game is located in a directory that is not supported\n->SOLUTION : Move Game folder to a location like C:\\SkyNest\\\n2.Some Resource might be corrupt\n->SOLUTION : Install the last manual patch.\n\nIf those options does not work try to use SkyNest Support Forums to contact us."),enc("GUIDE"),MB_OK);
}

void SupportForum()
{
//https://dragonnest.ro/forums/forumdisplay.php?41-Support
	ShellExecute(NULL, "open", enc("https://dragonnest.ro/forums/forumdisplay.php?41-Support"), NULL, NULL, SW_SHOWNORMAL);

}

void ClientIssuesThread()
{
	ShellExecute(NULL, "open", enc("https://dragonnest.ro/forums/showthread.php?2025-Fix-for-client-Issues"), NULL, NULL, SW_SHOWNORMAL);

}

BOOL CALLBACK WindowControllerTS(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!glb_HWND)
	{
		glb_HWND = hwnd;
	}

	switch (message)
	{
	case WM_INITDIALOG:
	{
			//center window!
			RECT rc;
			GetWindowRect(hwnd, &rc);

			int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
			int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

			SetWindowPos(hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			//Load Icon!
			HICON hIcon = LoadIcon(glb_hinst, MAKEINTRESOURCE(IDI_ICON1));
			SendMessage(hwnd, WM_SETICON, WPARAM(TRUE), LPARAM(hIcon));
			hIcon = LoadIcon(glb_hinst, MAKEINTRESOURCE(IDI_ICON1));
			SendMessage(hwnd, WM_SETICON, WPARAM(FALSE), LPARAM(hIcon));
			//disable the CLOSE button!
			HMENU hmenu = GetSystemMenu(hwnd, FALSE);
			EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
	}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
			case IDOK:
			{
				DestroyWindow(hwnd);
			}
				return TRUE;
			case IDC_BUTTON1:
			case IDC_BUTTON2:
			{
					IreneCrash();
			}
				return TRUE;
			case IDC_BUTTON4:
			{
					InvalidResource();
			}
				return TRUE;
			case IDC_BUTTON3:
			{
					SupportForum();
			}
			return TRUE;
			case IDC_BUTTON5:
			{
					ClientIssuesThread();
			}
			return TRUE;
		return TRUE;
		}
		break;
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return TRUE;
	}
	return FALSE;

}