// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <windows.h>
#include <tchar.h>
#include <string>


// stl
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <hash_map>
#include <algorithm>
#include <string>

#include "../DNDefine.h"

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
//#ifndef SAFE_RELEASE
//#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
//#endif


#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
