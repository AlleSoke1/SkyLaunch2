#include <vector>
#include <string>
#pragma once

class updater
{
public:
	updater();
	~updater();


	void doRestart();

	std::vector<std::string> getupdateFileList(int);
	
	void DownloadFileWrapper(std::string URL);

	void doUpdate(int vernum);
	void CheckForDragonRunning();
	void CheckForUpdate();
	int getClientVer();
	bool writeVersion(int vernum);
	//WINAPI WINDOW CLASS
	void InitWindow();
	void DestroyUpdateWindow();
	//var
	int ServerVer;
	int ClientVer;
	int currentFile;
};

BOOL CALLBACK WindowController(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


struct updateStruct{
	int version;
	char reserved[50]; //50 reserved bytes
};
