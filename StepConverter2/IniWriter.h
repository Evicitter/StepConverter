#ifndef INIWRITER_H
#define INIWRITER_H
class CIniWriter
{
public:
 CIniWriter(wchar_t* szFileName);
 void WriteInteger(wchar_t* szSection, wchar_t* szKey, int iValue);
 void WriteFloat(wchar_t* szSection, wchar_t* szKey, float fltValue);
 void WriteBoolean(wchar_t* szSection, wchar_t* szKey, bool bolValue);
 void WriteString(wchar_t* szSection, wchar_t* szKey, wchar_t* szValue);
private:
 wchar_t m_szFileName[255];
};
#endif //INIWRITER_H