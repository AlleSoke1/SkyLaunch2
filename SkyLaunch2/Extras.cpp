#include <iostream>
#include <istream>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>
#include "Extras.h"
#include "DEFINE.h"
#include <tlhelp32.h>

bool FileRename(std::string oldname,std::string newname)
{
	return rename(oldname.c_str(), newname.c_str());
}

bool FileExists(std::string path)
{
	std::ifstream my_file(path);
	if (my_file)
	{
		return true;
	}
	return false;
}

bool FileDelete(std::string path)
{
	int retCode = remove(path.c_str());
	if (retCode != 0)
	{
		char buff[128];
		wsprintf(buff, enc("Cannot delete file %s \nPlease remove manually!\nError : %d"), path.c_str(), retCode);
		MessageBoxA(NULL, buff, "ERROR", MB_OK);
		return true;
	}
	else
	{
		return false;
	}

}

std::string xorIT(std::string what)
{
	for (int i = 0; i < what.length(); i++)
	{
		what[i] = what[i] ^ XOR_KEY;
	}
	return what;
}

bool WriteAccountData(std::string user, std::string password, bool createFlag)
{
	std::ofstream File(STORE_ACCOUNT);
	if (File.is_open())
	{
		File << xorIT(user);
		File << " ";
		File << xorIT(password);
		File.close();
	}

	return true;
}


std::vector<std::string> ReadAccountData()
{
	std::string line;
	if (FileExists(enc("Data.sky")) == true)
	{
		std::ifstream File(enc("Data.sky"));
		if (File.is_open())
		{
			//There's only one LINE !^^
			std::getline(File, line);
			File.close();
		}
	}
	return SplitString(line);
}



std::vector<std::string> SplitString(std::string input)
{
	std::string buf;
	std::stringstream ss(input);
	std::vector<std::string> tokens;

	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

std::vector<std::string> &SplitStringDelim(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


void CheckLauncherRunning()
{
	int Total = 0;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
	while (Process32Next(snapshot, &entry))
	if (strcmp(entry.szExeFile, enc("Launcher.exe")) == 0)
		Total++;

	CloseHandle(snapshot);

	if (Total > 1)
	{
		MessageBoxA(NULL, enc("Launcher already running! Close it!"), enc("Error"), MB_OK);
		ExitProcess(0);
	}
}