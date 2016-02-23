struct Data
{
	BYTE version;
	char Username[20];
	char Password[20];
	bool RememberMe;
	bool ShowNewUI;
};
extern Data data;

class Settings
{
public:
	~Settings();
	Settings();
	void LoadSettings();
	void SaveSettings();
	void EncDecData(BYTE * data);


private:
	void SetVersion(BYTE ver);
};

extern Settings settings;