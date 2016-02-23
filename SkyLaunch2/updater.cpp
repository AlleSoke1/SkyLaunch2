#include <iostream>
#include <Windows.h>
#include <commctrl.h>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <sstream>
#include <process.h>
#include <thread>
#include "resource.h"
#include "httpLib.h"
#include "updater.h"
#include "DEFINE.h"
#include "Extras.h"
#include "ETPatcher.h"
#include <tlhelp32.h>


HWND glb_HWND;
HWND updaterWindowHWND;
int TotalFilesToBeDownloaded;


updater::updater()
{

}

updater::~updater()
{
	ServerVer = 0;
}

int updater::getClientVer()
{
	int cVer = 0;
	std::ifstream version;
	version.open(enc("Version.cfg"));
	if (version.is_open())
	{
		std::string line;
		std::getline(version, line);
		sscanf_s(line.c_str(), enc("Version %d"), &cVer);
	}
	else{
		cVer = 1;
		this->writeVersion(1);
	}
	version.close();
	return cVer;
}

bool updater::writeVersion(int vernum)
{
	std::ofstream version;
	version.open(enc("Version.cfg"));
	if (version.is_open())
	{
		version << enc("Version ") << vernum;
	}
	else
	{
		return false;
	}
	version.close();

	return true;
}

std::vector<std::string> updater::getupdateFileList(int updatenum)
{
	std::vector<std::string> tempvector;

	std::string cFileURL = UPDATE_URL;
				cFileURL += std::to_string(updatenum);
				cFileURL += enc("/update.sky");
	
	httpLib http;
	std::string result = http.GET(cFileURL);

	std::stringstream ss(result);
	std::string str;
	while (std::getline(ss, str,'\n'))
	{
		tempvector.push_back(str);
	}
#ifdef DEBUG
	for (int i = 0; i < tempvector.size(); i++)
	{
		MessageBoxA(NULL, tempvector[i].c_str() , "FileList", MB_OK);
	}
#endif
	return tempvector;
}

void updater::DownloadFileWrapper(std::string URL)
{
	httpLib http;
	http.Download(URL);
}

void updater::doRestart()
{
	SHELLEXECUTEINFO shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = enc("runas");
	shExecInfo.lpFile = enc("Launcher.exe");
	shExecInfo.lpParameters = NULL;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_NORMAL;
	shExecInfo.hInstApp = NULL;

	ShellExecuteEx(&shExecInfo);

	ExitProcess(0);
}

void updater::doUpdate(int vernum)
{
	std::vector<std::string> Filelist = this->getupdateFileList(vernum);

	//Set TOTAL FILES for progress bar.
	TotalFilesToBeDownloaded = Filelist.size();
	this->currentFile = 0;
	bool restart = false;
	for (int i = 0; i < Filelist.size(); i++)
	{
		Filelist[i].erase(Filelist[i].size() - 1); //remove \r

		this->currentFile++;
		std::string cDownloadURL = UPDATE_URL;
					cDownloadURL += std::to_string(vernum);
					cDownloadURL += "/";
					cDownloadURL += Filelist[i].c_str();
		/*Check for Launcher.exe file name to swap with current executable.:)*/
			//URL.erase(URL.size() - 1);  //remove \r
	
	  
	   if (Filelist[i].compare(enc("Launcher.exe")) == 0)
	   {
		   FileRename(Filelist[i], enc("Launcher.old"));
		  //MessageBoxA(NULL, enc("RENAME."), enc("Press OK"), MB_OK);	
		   restart = true;
	   }
	   else
	   {
		   if (FileExists(Filelist[i]) == true)
		   {
			   FileDelete(Filelist[i]);
			 //  MessageBoxA(NULL, "DELETE OK!", Filelist[i].c_str(), MB_ICONWARNING);
		   }
	   }

	   //Set progress bar to 0 for each file ~!
	    SendMessage(GetDlgItem(glb_HWND, IDC_PROGRESS1), PBM_STEPIT, 0, 0); //works fine
		this->DownloadFileWrapper(cDownloadURL);

		//Set MESSAGE
		HWND HWNDPatcherText = GetDlgItem(glb_HWND, IDC_UPDATER_TEXT);
		char ChatText[128];
		wsprintf(ChatText, enc("[SERVER #%d || CLIENT #%d]Downloading Patch #%d - Current File %d/%d"), ServerVer, ClientVer, ClientVer + 1, this->currentFile ,Filelist.size());
		SetWindowText(HWNDPatcherText, ChatText);


		//free memory
		free(&cDownloadURL);
		free(&TotalFilesToBeDownloaded);
	}

	//Set progress bar to 0
//	SendMessage(GetDlgItem(glb_HWND, IDC_PROGRESS1), PBM_DELTAPOS, 0, 0);
	//SendMessage(GetDlgItem(glb_HWND, IDC_PROGRESS1), PBM_STEPIT, 0, 0); //works fine

	ClientVer++;
	//MessageBoxA(NULL, std::to_string(vernum).c_str(), enc("Updating !"), MB_OK);
	this->writeVersion(ClientVer);
	free(&Filelist);

	//RESTART APPLICATION.
	if (restart == true)
	{
		this->doRestart();
	}

}

void updater::CheckForDragonRunning()
{
		bool exists = false;
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
		if (!strcmp(entry.szExeFile, enc("dragon.exe")))
			exists = true;

		CloseHandle(snapshot);
		
		if (exists == true)
		{
			MessageBoxA(NULL, enc("New Update available , please close game to update ! \nIf game is not open , kill dragon.exe process!"), enc("New Update"), MB_OK);
			ExitProcess(0);
		}
}

void updater::CheckForUpdate()
{
	//check version
	std::string cVerURL = UPDATE_URL;
				cVerURL += enc("version.sky");

	httpLib http;
	std::string rezultat =	http.GET(cVerURL);
	
	this->ServerVer = atoi(rezultat.c_str());
	this->ClientVer = this->getClientVer();

	if (ServerVer > ClientVer)
	{
		this->CheckForDragonRunning();

	//	std::thread DownloadThread(&updater::DownloadFileWrapper, this, cDownloadURL);
		std::thread threadGUI(&updater::InitWindow,this);

		//MessageBoxA(NULL, enc("New Update Available"), enc("Press OK"), MB_OK);
		while (ClientVer != ServerVer)
		{
			doUpdate(ClientVer+1);
		}
		//MessageBoxA(NULL, enc("Update Finished."), enc("Press OK"), MB_OK);	

		threadGUI.detach();
		this->DestroyUpdateWindow();

	}
}

void updater::DestroyUpdateWindow()
{
	DestroyWindow(updaterWindowHWND);
	ShowWindow(updaterWindowHWND, 0); //hideit
	UpdateWindow(updaterWindowHWND);
}

void updater::InitWindow()
{
	updaterWindowHWND = CreateDialog(glb_hinst, MAKEINTRESOURCE(IDD_UPDATER), 0, WindowController);
	ShowWindow(updaterWindowHWND, glb_cmdShow);

	MSG  msg;
	int status;
	while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		
		if (!IsDialogMessage(updaterWindowHWND, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


BOOL CALLBACK WindowController(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
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
					 SendMessage(GetDlgItem(hwnd, IDC_PROGRESS1), PBM_DELTAPOS, 10, 0);
				}
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