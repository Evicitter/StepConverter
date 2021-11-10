//Last update time : 13.10.2012 17:37:17

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "Include/CVLDSVideoLogger.h"
#include "Include/CVLGenericGen.h"

#include "Include/CVLDSCapture.h" 
#include "Include/CVLDSVideoPlayer.h" 
#include "Include/CVLDSImageDisplay.h" 

#include "Include/CVLChangeFormat.h"

#include "Include/CVLGenericFilter.h"
#include "Include/CALGenericFilter.h"

//#include "Include/CVLFreeFrame.h"

//#include "Include/CALChannelMerger.h"
//#include "Include/CALSpectrum.h"
#include "Include/CSLGenericReal.h"


#include "Include/CVLFrameQueue.h"

//enum TThreadPriority { tpIdle, tpLowest, tpLower, tpNormal, tpHigher, tpHighest, tpTimeCritical };

class CStepConverter_VLDlg : public CDialog
{
public:
	CStepConverter_VLDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_STEPCONVERTER_VL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	CRect       m_oldRect;
	CClientDC   *m_pDC;

	BOOL bSetupPixelFormat(void);
	void DrawScene(void);

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:

	int s_w;
	int s_h;
	unsigned int s_fps;

	//CTVLGenericGen	   VFLog;
	//CTVLDSVideoLogger  DSLogger;

	//CTVLDSCapture			 DSCapture;
	CTVLDSVideoPlayer  DSPlayer;
	//CTVLDSImageDisplay ImgDisplay;
	CTVLChangeFormat	VFormat;

	CTVLGenericFilter  VFilter;
	CTALGenericFilter  AFilter;

	CTVLFrameQueue VFrameQueue;

	//CTVLFreeFrame VFreeFrame;

	//CTVLGenericFilter  VF06Filter;

	//For Audio Reee
//	CTALChannelMerger AChannelMerger;
//	CTALSpectrum ASpectrum;
	CTSLGenericReal ARealBuffer;
	//----------------------------
public:
	CButton mb_Play;
	CButton mb_Pause;
	CButton mb_Stop;
	afx_msg void BC_Play();
	afx_msg void BC_Pause();
	afx_msg void BC_Stop();
	afx_msg void BC_Files();
	afx_msg void BC_Folder();
	afx_msg void BC_Close();
	afx_msg void BC_CloseAll();
	afx_msg void BC_Delete();
	afx_msg void BC_DeleteAll();
	void __stdcall OnVideoProgressEvent( void *Sender, __int64 SartFrame, __int64 EndFrame, __int64 CurrenFrame );
	void __stdcall OnVideoStopEvent( void *Sender );
	void __stdcall OnVideoResetEvent( void *Sender );

	void __stdcall OnVFilter_PD(void *Sender, TVLCVideoBuffer InBuffer, TVLCVideoBuffer& OutBuffer, bool& SendOutputData);
	void __stdcall OnVFilter_Start(void *Sender, int &AWidth, int &AHeight, double SampleRate);
	void __stdcall OnVFilter_Stop(void *Sender);

	void __stdcall OnAFilterAF_PD(void *Sender, TALCAudioBuffer InBuffer, TALCAudioBuffer& OutBuffer, bool& SendOutputData);
	void __stdcall OnAFilter_PD(void *Sender, TSLCRealBuffer BufferIn, TSLCRealBuffer &BufferOut, bool &SendOutputData);
	void __stdcall OnAFilter_Start(void *Sender, double SampleRate);
	void __stdcall OnAFilter_Stop(void *Sender);
	void __stdcall OnAFilterAF_Start(void *Sender);
//---------------------------------------------------------------------------------------

	CEdit me_SetPath;
	CListBox mlb_Files;
	CButton mc_Output;

	CProgressCtrl mpb_Frame;

	CEdit me_ElapseTime;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void BC_ToFolder();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);	
	CButton mc_FileDeleteAfter;
	CEdit me_CurFile;
	// компонент оценки производительности
	CEdit me_PerfMark;
	afx_msg void OnClose();
	CButton mc_HistFilter;

	afx_msg void BC_GetStatus();

	CEdit me_VFrameOnSnap;
	CEdit me_AFrameOnSnap;
	CSliderCtrl ms_AFrameOnSnap;
	CSliderCtrl ms_VFrameOnSnap;
	afx_msg void SLIDE_VFS(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void SLIDE_AFS(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic mgb_GLFrame;
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit me_ACodec;
	CEdit me_VCodec;
	afx_msg void BC_Capture();
	CButton mc_SimpleFlakBlack;
	afx_msg void On_TGraphics();
	afx_msg void On_TVideo();
	afx_msg void On_TAudio();
	afx_msg void On_THist();
	CButton mb_tgraphics;
	CButton mb_tvideo;
	CButton mb_taudio;
	CButton mb_thist;
};