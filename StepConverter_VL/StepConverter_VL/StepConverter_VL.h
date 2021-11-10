//Last update time : 05.06.2010 15:20:54
#pragma once
#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif
#include "resource.h"
class CStepConverter_VLApp : public CWinApp
{
public:
	CStepConverter_VLApp();
	public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};
extern CStepConverter_VLApp theApp;