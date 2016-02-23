#include "se.h"

//#define SKYNEST


#ifdef SKYNEST
#define UPDATE_URL enc("http://cdn.dragonnest.ro/reborn/update/client/")
#define API_URL enc("http://reborn.dragonnest.ro/motto-yesno")
#define STORE_ACCOUNT enc("Settings.dat")
#define STORE_ACCOUNT_VERSION 1
#define XOR_KEY 0xFF
#endif

#ifdef CHN_
#define UPDATE_URL enc("http://117.27.240.101:5897/")
#define API_URL enc("not_used")
#define STORE_ACCOUNT enc("Settings.dat")
#define STORE_ACCOUNT_VERSION 1
#define XOR_KEY 0xC3

#define IP enc("117.27.248.30")
#define PORT enc("14300")
#endif

#ifdef VTM_TEST
#define UPDATE_URL enc("http://124.158.12.183/IT/update/")
#define API_URL enc("not_used")
#define STORE_ACCOUNT enc("Settings.dat")
#define STORE_ACCOUNT_VERSION 1
#define XOR_KEY 0xC4

#define IP enc("124.158.12.183")
#define PORT enc("14300")
#endif

#ifdef VTM
#define UPDATE_URL enc("http://124.158.12.182/IT/update/")
#define API_URL enc("not_used")
#define STORE_ACCOUNT enc("Settings.dat")
#define STORE_ACCOUNT_VERSION 1
#define XOR_KEY 0xC4

#define IP enc("124.158.12.182")
#define PORT enc("14300")
#endif

//WND GLOBALS
extern HINSTANCE glb_hinst;
extern int glb_cmdShow;
extern HWND glb_HWND;

extern int TotalFilesToBeDownloaded;

BOOL CALLBACK DialogProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);