
#include <iostream>
#include <curl\curl.h>
#include "httpLib.h"
#include "DEFINE.h"
#include "resource.h"
#include <commctrl.h>

using namespace std;

httpLib::httpLib()
{
}


httpLib::~httpLib()
{
}

std::string getFileName(std::string input)
{
	int contor = input.find_last_of('/');
	if (contor != std::string::npos)
		input = input.substr(contor + 1); // f contains the result :)
	return input;
}

bool BytesToSize(double Bytes, char * ByteString, int BufSize)
{
	if (Bytes <= 0)
		return false;

	float tb = 1099511627776.0f;
	float gb = 1073741824.0f;
	float mb = 1048576.0f;
	float kb = 1024.0f;

	char * returnSize = (char *)malloc(BufSize);
	if (returnSize == NULL)
		return false;

	if (Bytes >= tb)
		sprintf(returnSize, "%.2lf TB", (double)Bytes / tb);
	else if (Bytes >= gb && Bytes < tb)
		sprintf(returnSize, "%.2lf GB", (double)Bytes / gb);
	else if (Bytes >= mb && Bytes < gb)
		sprintf(returnSize, "%.2lf MB", (double)Bytes / mb);
	else if (Bytes >= kb && Bytes < mb)
		sprintf(returnSize, "%.2lf KB", (double)Bytes / kb);
	else if (Bytes < kb)
		sprintf(returnSize, "%.2lf Bytes", Bytes);
	else
		sprintf(returnSize, "%.2lf Bytes", Bytes);

	memcpy((void *)ByteString, (void *)returnSize, BufSize);
	free(returnSize);
	return true;
}

extern "C" std::size_t append_to_string(void* contents, std::size_t size, std::size_t nmemb, void* pstr)
{
	const std::size_t sz = size * nmemb;
	const char* cstr = static_cast<const char*>(contents);
	std::string& str = *static_cast< std::string* >(pstr);
	for (std::size_t i = 0; i < sz; ++i) str += cstr[i];
	return sz;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int n = 0;

int download_progress_callback(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
	if (TotalToDownload <= 0.0) {
		return 0;
	}
	//int proc = (100 / TotalToDownload);// / TotalFilesToBeDownloaded;

	/*int totaldotz = 21;
	double fractiondownloaded = NowDownloaded / TotalToDownload;
	// part of the progressmeter that's already "full"
	int dotz = round(fractiondownloaded * totaldotz) / TotalFilesToBeDownloaded;
	*/

	n++;
	if ((n % 100) == 0)
	{
		double fProgress = (NowDownloaded / TotalToDownload)*100;
		int progress = (int)fProgress;
		if (progress <= 100 && progress > 0)
			SendMessage(GetDlgItem(glb_HWND, IDC_PROGRESS1), PBM_SETPOS, (int)progress, 0);
	

		//show procentage of current file !
		HWND HWNDProgress = GetDlgItem(glb_HWND, IDC_TOTAL_PROGRESS);
		char ProgressText[128];
		wsprintf(ProgressText, enc("%d%%"), (int)progress);
		SetWindowText(HWNDProgress, ProgressText);

		//Show TOTAL BYTES of DOWNLOADED / REMAINING TO DOWNLOAD!
		HWND HWNDTotal = GetDlgItem(glb_HWND, IDC_TOTAL);
		char TotalText[128];
		char *totalToDown = (char *)malloc(1024); //alocam 1kb
		char *finishedDown = (char *)malloc(1024);
		BytesToSize(NowDownloaded, finishedDown, 1024);
		BytesToSize(TotalToDownload, totalToDown, 1024);
		wsprintf(TotalText, enc("Downloaded %s of %s"),finishedDown,totalToDown);
		SetWindowText(HWNDTotal, TotalText);

		free(totalToDown);
		free(finishedDown);

		n = 0;
	}
	//if (NowDownloaded == TotalToDownload)

	// It's here you will write the code for the progress message or bar

	return 0;
}

bool httpLib::Download(std::string URL)
{
	curl_global_init(CURL_GLOBAL_ALL); // wrap in an RAII shim

	CURL *curl;
	FILE *fp;
	CURLcode res;
	std::string output_file = getFileName(URL).c_str();
	//output_file.erase(output_file.size() - 1);  //remove \r
	//URL.erase(URL.size() - 1);  //remove \r


	curl = curl_easy_init();
	if (curl) {
		fopen_s(&fp, output_file.c_str() , "wb");
		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
#ifdef CHN_
		curl_easy_setopt(curl, CURLOPT_PORT, 5897);
#endif
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress_callback);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, enc("Mozilla/5.0 (Windows NT 10.0; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0")); // avoid problems with cloudflare.

		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		fclose(fp);
	}

	return true;
}

void httpLib::Download2(std::string URL)
{
	//
}


std::string httpLib::GET(std::string URL)
{
	curl_global_init(CURL_GLOBAL_ALL); // wrap in an RAII shim
	CURL* curl_handle = curl_easy_init(); // use std::unique_ptr with a custom deleter
	std::string page;
	std::string retn;

	curl_easy_setopt(curl_handle, CURLOPT_URL, URL); // url
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, append_to_string); // call 'append_to_string' with data
	// pass the address of string 'page' to the callback 'append_to_string'
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, std::addressof(page));
	//curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "dragonnest.ro"); // user-agent (optional)
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, enc("Mozilla/5.0 (Windows NT 10.0; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0")); // avoid problems with cloudflare.

	const auto result = curl_easy_perform(curl_handle); // get the page
	if (result == CURLE_OK)
		retn = page;
	else std::cerr << "**** error: " << curl_easy_strerror(result) << '\n';

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	return retn;
}


std::string httpLib::GET(std::string URL,std::string UA)
{
	curl_global_init(CURL_GLOBAL_ALL); // wrap in an RAII shim
	CURL* curl_handle = curl_easy_init(); // use std::unique_ptr with a custom deleter
	std::string page;
	std::string retn;

	curl_easy_setopt(curl_handle, CURLOPT_URL, URL); // url
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, append_to_string); // call 'append_to_string' with data
	// pass the address of string 'page' to the callback 'append_to_string'
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, std::addressof(page));
	//curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "dragonnest.ro"); // user-agent (optional)
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, enc("dragonnest.ro")); // avoid problems with cloudflare.

	const auto result = curl_easy_perform(curl_handle); // get the page
	if (result == CURLE_OK)
		retn = page;
	else std::cerr << "**** error: " << curl_easy_strerror(result) << '\n';

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	return retn;
}


std::string httpLib::POST(std::string URL)
{
	return "post";
}
