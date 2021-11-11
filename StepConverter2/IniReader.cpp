#include "IniReader.h"
#include <iostream>
#include <Windows.h>

CIniReader::CIniReader(wchar_t* szFileName)
{
 wchar_t nbuf[MAX_PATH];
 GetCurrentDirectoryW(MAX_PATH, nbuf);
 memset(m_szFileName, 0, sizeof(m_szFileName));
 wcscpy(m_szFileName, nbuf);
 wcscat(m_szFileName, L"\\");
 wcscat(m_szFileName, szFileName);
}
int CIniReader::ReadInteger(wchar_t* szSection, wchar_t* szKey, int iDefaultValue)
{
 int iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, m_szFileName); 
 return iResult;
}
float CIniReader::ReadFloat(wchar_t* szSection, wchar_t* szKey, float fltDefaultValue)
{
 wchar_t szResult[255];
 wchar_t szDefault[255];
 float fltResult;
 wsprintf(szDefault, L"%f",fltDefaultValue);
 GetPrivateProfileString(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
 fltResult =  _wtof(szResult);
 return fltResult;
}
bool CIniReader::ReadBoolean(wchar_t* szSection, wchar_t* szKey, bool bolDefaultValue)
{
 wchar_t szResult[255];
 wchar_t szDefault[255];
 bool bolResult;
 wsprintf(szDefault, L"%s", bolDefaultValue? L"True" : L"False");
 GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName); 
 bolResult =  (_wcsicmp(szResult, L"true") == 0) ? true : false;
 return bolResult;
}
wchar_t* CIniReader::ReadString(wchar_t* szSection, wchar_t* szKey, const wchar_t* szDefaultValue)
{
 wchar_t* szResult = new wchar_t[255];
 memset(szResult, 0x00, 255*2);
 GetPrivateProfileString(szSection,  szKey, 
		szDefaultValue, szResult, 255, m_szFileName); 
 return szResult;
}