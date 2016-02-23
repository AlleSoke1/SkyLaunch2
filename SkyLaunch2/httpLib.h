#pragma once
class httpLib
{
public:
	httpLib();
	~httpLib();

	bool Download(std::string URL);	
	void Download2(std::string URL);
	std::string GET(std::string URL);
	std::string GET(std::string URL,std::string UA);
	std::string POST(std::string URL);

};

