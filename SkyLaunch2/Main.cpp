#include <Windows.h>
#include "resource.h"
#include <commctrl.h>
#include "updater.h"
#include "DEFINE.h"
#include "base64.h"
#include "httpLib.h"
#include "Extras.h"
#include "Troubleshooter.h"
#include "Settings.h"
#include <CrashRpt.h>
#include "md5.h"

HINSTANCE glb_hinst;
int glb_cmdShow;
HWND local_launcherHWND;

//globale
void PlayButton();

#ifdef REBORN
crash_rpt::CrashRpt g_crashRpt(
	"e92a3c74-bb29-4da1-b560-939b4421d30f", // GUID assigned to this application.
	L"Launcher",                           // Application name that will be used in a message box.
	L"SkyNest"                            // Company name that will be used in a message box.
	);
#endif

int WINAPI WinMain
(HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow)
{
	glb_hinst = hInst;
	glb_cmdShow = cmdShow;

	HWND hDialog = 0;

	//avoid opening 2launchers
	CheckLauncherRunning();

	//Check for OLD Launcher!
	if (FileExists(enc("Launcher.old")) == true)
	{
		FileDelete(enc("Launcher.old"));
	}



	hDialog = CreateDialog(hInst,
		MAKEINTRESOURCE(IDD_LOGIN),
		0,
		DialogProc);
	//set global HWND.
	local_launcherHWND = hDialog;

	if (!hDialog)
	{
		char buf[100];
		wsprintf(buf, enc("Error x%x"), GetLastError());
		MessageBox(0, buf, enc("CreateDialog"), MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}


	//UPDATER
	updater upd;
	upd.CheckForUpdate();


	ShowWindow(hDialog, cmdShow);
	UpdateWindow(hDialog);


	//MIGRATE HERE
	//check for old V1 account data;
	if (FileExists(enc("Data.sky")) == true)
	{
		//MIGRATE !
		std::vector<std::string> Account = ReadAccountData();
		strcpy(data.Username, xorIT(Account[0]).c_str());
		strcpy(data.Password, xorIT(Account[1]).c_str());

		data.RememberMe = true;

		HWND HWNDUserText = GetDlgItem(hDialog, ID_USER);
		HWND HWNDPassText = GetDlgItem(hDialog, ID_PASS);

		SetWindowText(HWNDUserText, data.Username);
		SetWindowText(HWNDPassText, data.Password);
		//remember me checkbox.
		HWND hwndCheckBox = GetDlgItem(hDialog, ID_REMEMBER);
		SendMessage(hwndCheckBox, BM_SETCHECK, 1, 0);

		//delete old file.
		FileDelete(enc("Data.sky"));
	}
	//END MIGRATE!

	//LOAD ACCOUNT
	if (FileExists(STORE_ACCOUNT) == true)
	{
		settings.LoadSettings();
		if (data.RememberMe)//precheck ? :))
		{
			HWND HWNDUserText = GetDlgItem(hDialog, ID_USER);
			HWND HWNDPassText = GetDlgItem(hDialog, ID_PASS);
			SetWindowText(HWNDUserText, data.Username);
			SetWindowText(HWNDPassText, data.Password);
			//remember me checkbox.
			HWND hwndCheckBox = GetDlgItem(hDialog, ID_REMEMBER);
			SendMessage(hwndCheckBox, BM_SETCHECK, 1, 0);
		}

		//New UI
		HWND hwndCheckBox1 = GetDlgItem(hDialog, ID_NEWUI);
		SendMessage(hwndCheckBox1, BM_SETCHECK, data.ShowNewUI, 0);
	}

#if defined(CHN_) || defined(VTM_TEST) || defined(VTM)
	//
	HWND WindowHWND = GetActiveWindow();
#ifdef VTM_TEST
	SetWindowTextW(WindowHWND, L"iTea DragonNest Launcher (TEST SERVER)");
#endif
#ifdef VTM
	SetWindowTextW(WindowHWND, L"iTea DragonNest Launcher");
#endif
#ifdef CHN_
	SetWindowTextW(WindowHWND, L"Poasf.com Launcher");
#endif

	HWND NEWUI_BUtton = GetDlgItem(hDialog, ID_NEWUI);
	ShowWindow(NEWUI_BUtton, SW_HIDE);
	HWND Troubleshooter_BUtton = GetDlgItem(hDialog, ID_TROUBLESHOOTER);
	ShowWindow(Troubleshooter_BUtton, SW_HIDE);

#endif
	//Message :)
	MSG  msg;
	int status;
	while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (status == -1)
			return -1;
		if (!IsDialogMessage(hDialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

BOOL CALLBACK DialogProc(HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
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

			case IDCANCEL:
				EndDialog(hwnd, 0);
				return TRUE;
			case ID_PLAY:
				PlayButton();
				return TRUE;
			case ID_TROUBLESHOOTER:
				ShowTroubleShooter();
				return TRUE;
		return TRUE;
		}
		break;
		return TRUE;
	case WM_HSCROLL:
		//control->Scroll(hwnd, LOWORD(wParam), HIWORD(wParam));
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return TRUE;
	}
	return FALSE;
}




void PlayButton()
{
	bool AllOK = false;
	char username[18];
	char password[18];
	HWND HWNDUsername = GetDlgItem(local_launcherHWND, ID_USER);
	HWND HWNDPassword = GetDlgItem(local_launcherHWND, ID_PASS);


	GetWindowText(HWNDUsername, username, 18);
	GetWindowText(HWNDPassword, password, 18);
	
	if (strlen(username) < 4 || strlen(username) > 18)
	{
		MessageBox(NULL,enc("Username must between 4 and 18 characters!"), enc("Attention!"), MB_ICONEXCLAMATION);
		return;
	}
	if (strlen(password) < 6 | strlen(password) > 18)
	{
		MessageBox(NULL,enc("Passsword must between 6 and 18 characters!"), enc("Attention!"), MB_ICONEXCLAMATION);
		return;
	}

//	MessageBoxA(NULL, username, password, MB_OK);

#ifdef REBORN

	//DO WEB REQUEST!!
	std::string tempData = enc("login&");
				tempData += username;
				tempData += "&";
				tempData += password;
				tempData += "&";
				tempData += std::to_string(3); //add version3 api line



	std::string api = API_URL;
	std::string base64data = base64_encode((unsigned const char*)tempData.c_str(), tempData.length());
	std::string tempSend = api + enc("?r=") + base64data;
	std::string temp;

	httpLib http;

	temp = http.GET(tempSend,enc("dragonnest.ro"));

	std::string IP, PORT, MD5PASSWORD;


	if (temp.length() > 0)
	{
		std::vector<std::string> delimited;
		SplitStringDelim(temp, ',', delimited); //split vector ^^
		//Version 3 api refference.
		// NOK / OK || ip || port  || MD5 pass
		//MessageBoxA(NULL, temp.c_str(), "OK", MB_OK);
		if (delimited[0].compare(enc("OK")) == 0)
		{
			IP = delimited[1];
			PORT = delimited[2];
			MD5PASSWORD = delimited[3];	
#endif
			if (FileExists(STORE_ACCOUNT) == true)
			{
				FileDelete(STORE_ACCOUNT);
			}
			//precheck settings before writing them.
			//check new UI
			HWND hwndCheckBox = GetDlgItem(local_launcherHWND, ID_NEWUI); //newui
			data.ShowNewUI = SendMessage(hwndCheckBox, BM_GETCHECK, 0, 0);
			HWND hwndCheckBox_REMEMBER_ME = GetDlgItem(local_launcherHWND, ID_REMEMBER); //Remember Me
			data.RememberMe = SendMessage(hwndCheckBox_REMEMBER_ME, BM_GETCHECK, 0, 0);

			strcpy(data.Username,username); //setup account and password.
			strcpy(data.Password,password);

			settings.SaveSettings();
			/*
			HWND hwndCheckBox = GetDlgItem(local_launcherHWND, ID_REMEMBER);
			int res;
			res = SendMessage(hwndCheckBox, BM_GETCHECK, 0, 0);
			if (res == TRUE)
			{
				if (FileExists(STORE_ACCOUNT) == true)
				{
					WriteAccountData(username, password, false);
				}
				else {
					WriteAccountData(username, password, true);
				}
			}
			else
			{
				if (FileExists(STORE_ACCOUNT) == true)
				{
					FileDelete(STORE_ACCOUNT);
				}
			}
			*/

			AllOK = true;
#ifdef REBORN

		}
		else if (delimited[0].compare(enc("NOK")) == 0)
		{
			if (delimited[1].compare(enc("0")) == 0)
			{
				MessageBoxA(NULL, enc("Invalid username or password!"), enc("Error!"), MB_ICONEXCLAMATION);
				return;
			}

			if (delimited[1].compare(enc("99")) == 0)
			{
				MessageBoxA(NULL, delimited[2].c_str(), enc("Message"), MB_OK);
				return;
			}
		}

	}
	else{
		MessageBoxA(NULL, enc("Could not connect to server"), enc("Error"), MB_OK);
		ExitProcess(0);
	}

#endif

	if (AllOK == true)
	{
		//EXECUTE
		char lpArguments[256];
		char lpApplicationName[128];
		
#ifdef REBORN
		if (data.ShowNewUI)
		{
			wsprintf(lpArguments, enc("/newui /logintoken:%s&%s /ip:%s /port:%s /Lver:2 /use_packing /gamechanneling:0"), username, MD5PASSWORD.c_str(), IP.c_str(), PORT.c_str());
		}else{
			wsprintf(lpArguments, enc("/logintoken:%s&%s /ip:%s /port:%s /Lver:2 /use_packing /gamechanneling:0"), username, MD5PASSWORD.c_str(), IP.c_str(), PORT.c_str());
		}wsprintf(lpApplicationName, enc(".\\dragon.exe"));

#else
		if (data.ShowNewUI)
		{
			wsprintf(lpArguments, enc("/newui /logintoken:%s&%s /ip:%s /port:%s /Lver:2 /use_packing /gamechanneling:0"), username, md5(password).c_str(), IP, PORT);
		}
		else{
			wsprintf(lpArguments, enc("/logintoken:%s&%s /ip:%s /port:%s /Lver:2 /use_packing /gamechanneling:0"), username, md5(password).c_str(), IP, PORT);
		}wsprintf(lpApplicationName, enc(".\\dragonnest.exe"));
#endif
		
		SHELLEXECUTEINFO shExecInfo;

		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

		shExecInfo.fMask = NULL;
		shExecInfo.hwnd = NULL;
		shExecInfo.lpVerb = enc("runas");
		shExecInfo.lpFile = lpApplicationName;
		shExecInfo.lpParameters = lpArguments;
		shExecInfo.lpDirectory = NULL;
		shExecInfo.nShow = SW_NORMAL;
		shExecInfo.hInstApp = NULL;

		ShellExecuteEx(&shExecInfo);

		Sleep(1000);
		ExitProcess(0);
	}
}