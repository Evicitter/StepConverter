//Last update time : 08.03.2011 18:02:47

#include "stdafx.h"
#include "StepConverter_VL.h"
#include "StepConverter_VLDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CStepConverter_VLApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CStepConverter_VLApp::CStepConverter_VLApp() {}
CStepConverter_VLApp theApp;

BOOL CStepConverter_VLApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	SetRegistryKey(_T("\"FurionATA\""));

	CStepConverter_VLDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	return FALSE;
}