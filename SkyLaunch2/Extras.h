#pragma once


bool FileRename(std::string oldname, std::string newname);
bool FileExists(std::string path);
bool FileDelete(std::string path);
std::string capital(std::string name);
std::string xorIT(std::string what);
bool WriteAccountData(std::string user, std::string password, bool createFlag);
std::vector<std::string> ReadAccountData();
std::vector<std::string> SplitString(std::string input);
std::vector<std::string> &SplitStringDelim(const std::string &s, char delim, std::vector<std::string> &elems);
void CheckLauncherRunning();