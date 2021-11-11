#include "IniWriter.h"
#include <iostream>
#include <Windows.h> 
CIniWriter::CIniWriter(wchar_t* szFileName)
{
 wchar_t nbuf[MAX_PATH];
 GetCurrentDirectoryW(MAX_PATH, nbuf);
 memset(m_szFileName, 0, sizeof(m_szFileName));
 wcscpy(m_szFileName, nbuf);
 wcscat(m_szFileName, L"\\");
 wcscat(m_szFileName, szFileName);
}
void CIniWriter::WriteInteger(wchar_t* szSection, wchar_t* szKey, int iValue)
{
 wchar_t szValue[255];
 wsprintf(szValue, L"%d", iValue);
 WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteFloat(wchar_t* szSection, wchar_t* szKey, float fltValue)
{
 wchar_t szValue[255];
 wsprintf(szValue, L"%f", fltValue);
 WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteBoolean(wchar_t* szSection, wchar_t* szKey, bool bolValue)
{
 if(bolValue) WritePrivateProfileString(szSection,  szKey, L"True", m_szFileName);
 else		  WritePrivateProfileString(szSection,  szKey, L"False", m_szFileName);
}
void CIniWriter::WriteString(wchar_t* szSection, wchar_t* szKey, wchar_t* szValue)
{
 WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName);
}