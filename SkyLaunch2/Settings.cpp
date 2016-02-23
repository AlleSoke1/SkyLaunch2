#include <Windows.h>
#include "Settings.h"
#include <fstream>
#include "se.h"
#include "resource.h"
#include "DEFINE.h"

Data data;
Settings settings;


Settings::~Settings()
{
	//destructor
}

Settings::Settings()
{
	//constructor
}

void Settings::LoadSettings()
{
	std::fstream fh;
	fh.open(STORE_ACCOUNT, std::fstream::in | std::fstream::binary);
	fh.read((char*)&data.version, sizeof(data.version));
	fh.read((char*)&data.Username, sizeof(data.Username));
	fh.read((char*)&data.Password, sizeof(data.Password));
	fh.read((char*)&data.RememberMe, sizeof(data.RememberMe));
	fh.read((char*)&data.ShowNewUI, sizeof(data.ShowNewUI));
	this->EncDecData((BYTE*)&data);
	fh.close();
}

void Settings::SaveSettings()
{
	//set Version!
	this->SetVersion(STORE_ACCOUNT_VERSION);

	std::fstream fh;
	fh.open(STORE_ACCOUNT, std::fstream::out | std::fstream::binary);
	this->EncDecData((BYTE*)&data);
	fh.write((char*)&data, sizeof(Data));
	fh.close();
}

void Settings::SetVersion(BYTE ver)
{
	data.version = ver;
}

void Settings::EncDecData(BYTE * ce)
{
	for (DWORD i = 0; i < sizeof(Data); i++)
	{
		ce[i] ^= XOR_KEY;
	} 
}