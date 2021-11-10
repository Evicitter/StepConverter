//Last update time : 13.11.2012 12:49:27

#include "stdafx.h"
#include "StepConverter_VL.h"
#include "StepConverter_VLDlg.h"

#include "G_M8_RAND.h"

#include <ShlObj.h>
#include <intrin.h>
#include <wmmintrin.h>

#include <omp.h>
#include "gl/gl.h"
#include "gl/glu.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include "exFloatFilter.h"
#include "StepConFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Глобальные переменые
long glb_bitmask=0xffffffff;
//0 - show graphics
//1 - show video
//2 - show audio
//3 - show hist
//---------------------

bool bHISTFILTER=false;
bool bSFLAKBLACK=false;
bool bManualStop=false;
int iProgressPos=0;
int iProgressLast=0;
int iiPerfMarkAudio=0;
int iiPerfMarkVideo=0;
int iiAFramesOnSnap=1;
int iiVFramesOnSnap=1;
int CurAFrame=1;
int CurVFrame=1;

CString SetupPath;
CString spWorkFolder;
CString spMediaFile;
CString sFileSelect;

CFile cfRenderOut;
G_M8_RAND ggMR;
__int64 fsTotalFrames=0;
__int64 fsCurFrames=0;

__int64 aHeaderVRD[512];
__int64 vHeaderVRD[512];

//Audio buffers----------------------------------------------------------------------
static unsigned long* AudioGist=NULL;
static float* glAudioGist=NULL;
static unsigned __int64* tmpAudioGist=NULL;
//static double* tmpAudioGist=NULL;
//-----------------------------------------------------------------------------------
//Video buffers----------------------------------------------------------------------
static unsigned long* VideoOrigin=NULL;
static float* glVideoOrigin=NULL;

static unsigned long glAudioMax=0;
static unsigned long glVideoMax=0;
//-----------------------------------------------------------------------------------

static unsigned long* VidOrigBuffer=NULL;
static int AAHeight;
static int AAWidth;

static TVLCVideoBuffer* GLBVB=NULL;

struct GCCache
{
	unsigned char* buf;
	int Param;

	void Check(int newsize)
	{
	if(newsize!=Param)
	{
		if(buf!=NULL) { _aligned_free(buf); buf=NULL; }
		Param=newsize;
		buf = (unsigned char*)_aligned_malloc( Param, 16);
	}
	}
	GCCache() { buf=NULL; Param=0; }
	~GCCache() { if(buf!=NULL) { _aligned_free(buf); buf=NULL; } }
	operator unsigned char*() { return buf; }
	//unsigned char&       operator[](unsigned int n)       { return buf[n]; }
	//const unsigned char& operator[](unsigned int n) const	{ return buf[n]; }
};

static GCCache OutCache;

// диалоговое окно CStepConverter_VLDlg
static BROWSEINFOW bi = {NULL, NULL, NULL, L"Выберите, папку для поиска файлов...", BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_BROWSEFORCOMPUTER, NULL, NULL, 0};

bool IsMediaFile( CString fname )
{
	fname = fname.Right( fname.GetLength() - fname.ReverseFind(L'.') );
	if(!fname.CompareNoCase(L".avi")) return true;
	else if(!fname.CompareNoCase(L".mp4")) return true;
	else if(!fname.CompareNoCase(L".wmv")) return true;
	else if(!fname.CompareNoCase(L".mkv")) return true;
	else if(!fname.CompareNoCase(L".mpg")) return true;
	else if(!fname.CompareNoCase(L".mpeg")) return true;
	else if(!fname.CompareNoCase(L".flv")) return true;
	else if(!fname.CompareNoCase(L".mov")) return true;
	else if(!fname.CompareNoCase(L".ogm")) return true;
	else if(!fname.CompareNoCase(L".ts")) return true;
	else if(!fname.CompareNoCase(L".m2ts")) return true;
	return false;
}

void ListInheritFiles( CString fpath, CListBox* lb )
{
	if( lb == NULL ) return;

	CFileFind finder;
	BOOL bWorking = finder.FindFile( fpath + _T("*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if( finder.IsDots() ) continue;
		if( finder.IsDirectory() )							ListInheritFiles( fpath + finder.GetFileName() + _T("\\"), lb );
		else if( IsMediaFile( finder.GetFileName() ) )		lb->AddString( finder.GetFilePath() );
	}
}

void ListLocalFiles( CString fpath, CListBox* lb )
{
	if( lb == NULL ) return;

	CFileFind finder;
	BOOL bWorking = finder.FindFile( fpath + _T("*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if( !(finder.IsDirectory() || finder.IsDots()) && IsMediaFile( finder.GetFileName() ) )
			lb->AddString( finder.GetFilePath() ); 
	}
	finder.Close();
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
CStepConverter_VLDlg::CStepConverter_VLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStepConverter_VLDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStepConverter_VLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, mb_Play);
	DDX_Control(pDX, IDC_BUTTON2, mb_Pause);
	DDX_Control(pDX, IDC_BUTTON3, mb_Stop);
	DDX_Control(pDX, IDC_LIST1, mlb_Files);
	DDX_Control(pDX, IDC_CHECK1, mc_Output);
	DDX_Control(pDX, IDC_PROGRESS1, mpb_Frame);
	DDX_Control(pDX, IDC_EDIT1, me_SetPath);
	DDX_Control(pDX, IDC_EDIT2, me_ElapseTime);
	DDX_Control(pDX, IDC_CHECK4, mc_FileDeleteAfter);
	DDX_Control(pDX, IDC_EDIT3, me_CurFile);
	DDX_Control(pDX, IDC_EDIT4, me_PerfMark);
	DDX_Control(pDX, IDC_CHECK5, mc_HistFilter);
	DDX_Control(pDX, IDC_EDIT5, me_VFrameOnSnap);
	DDX_Control(pDX, IDC_EDIT6, me_AFrameOnSnap);
	DDX_Control(pDX, IDC_SLIDER1, ms_AFrameOnSnap);
	DDX_Control(pDX, IDC_SLIDER2, ms_VFrameOnSnap);
	DDX_Control(pDX, IDC_GLFRAME, mgb_GLFrame);
	DDX_Control(pDX, IDC_EDIT8, me_ACodec);
	DDX_Control(pDX, IDC_EDIT7, me_VCodec);
	DDX_Control(pDX, IDC_CHECK2, mc_SimpleFlakBlack);
	DDX_Control(pDX, IDC_BUTTON15, mb_tgraphics);
	DDX_Control(pDX, IDC_BUTTON12, mb_tvideo);
	DDX_Control(pDX, IDC_BUTTON13, mb_taudio);
	DDX_Control(pDX, IDC_BUTTON14, mb_thist);
}

BEGIN_MESSAGE_MAP(CStepConverter_VLDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CStepConverter_VLDlg::BC_Play)
	ON_BN_CLICKED(IDC_BUTTON2, &CStepConverter_VLDlg::BC_Pause)
	ON_BN_CLICKED(IDC_BUTTON3, &CStepConverter_VLDlg::BC_Stop)
	ON_BN_CLICKED(IDC_BUTTON4, &CStepConverter_VLDlg::BC_Files)
	ON_BN_CLICKED(IDC_BUTTON5, &CStepConverter_VLDlg::BC_Folder)
	ON_BN_CLICKED(IDC_BUTTON6, &CStepConverter_VLDlg::BC_Close)
	ON_BN_CLICKED(IDC_BUTTON7, &CStepConverter_VLDlg::BC_CloseAll)
	ON_BN_CLICKED(IDC_BUTTON8, &CStepConverter_VLDlg::BC_Delete)
	ON_BN_CLICKED(IDC_BUTTON9, &CStepConverter_VLDlg::BC_DeleteAll)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON10, &CStepConverter_VLDlg::BC_ToFolder)
	ON_WM_DROPFILES()
	ON_WM_HELPINFO()
	ON_WM_CLOSE()
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER2, &CStepConverter_VLDlg::SLIDE_VFS)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &CStepConverter_VLDlg::SLIDE_AFS)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON11, &CStepConverter_VLDlg::BC_Capture)
	ON_BN_CLICKED(IDC_BUTTON15, &CStepConverter_VLDlg::On_TGraphics)
	ON_BN_CLICKED(IDC_BUTTON12, &CStepConverter_VLDlg::On_TVideo)
	ON_BN_CLICKED(IDC_BUTTON13, &CStepConverter_VLDlg::On_TAudio)
	ON_BN_CLICKED(IDC_BUTTON14, &CStepConverter_VLDlg::On_THist)
END_MESSAGE_MAP()

BOOL CStepConverter_VLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// TODO: добавьте дополнительную инициализацию
	mpb_Frame.SetRange32( 0, 2147483647 );
	mpb_Frame.SetState(2);
	mpb_Frame.SetStep(1);

	me_AFrameOnSnap.SetLimitText(4);
	me_VFrameOnSnap.SetLimitText(4);
	me_AFrameOnSnap.SetWindowText(L"1000");
	me_VFrameOnSnap.SetWindowText(L"32");
	ms_AFrameOnSnap.SetRange(1,1000);
	ms_VFrameOnSnap.SetRange(1,1000);
	ms_AFrameOnSnap.SetPos(1000);
	ms_VFrameOnSnap.SetPos(32);

	VCL_Loaded();
	VCL_InitControls( m_hWnd );
	
	//ImgDisplay.Open( mDSVideoDisplay.m_hWnd );

	//Logger SET
	/*VFLog.VideoFormat = vfRGBA32;
	VFLog.ClockSource = csExternal;
	VFLog.FrameRate = 20;
	VFLog.ImageSize.Height = 512;
	VFLog.ImageSize.Width = 512;
	VFLog.OnGenerate.Set( this, &CStepConverter_VLDlg::OnVLogGen );

	DSLogger.FileName = L"";
	DSLogger.AudioCompression.Enabled = false;
	DSLogger.VideoCompression.Enabled = true;
	DSLogger.VideoCompression.Compressions.Clear();
	DSLogger.VideoCompression.Compressions.Add();
	DSLogger.VideoCompression.Compressions[0].Compressor = L"ffdshow video encoder"; // L"Xvid MPEG-4 Codec";
	//DSLogger.VideoCompression.Compressions[0].Quality = 100000;
	//DSLogger.VideoCompression.Compressions[0].KeyFrameRate = 15;
	DSLogger.InputPin.Connect( VFLog.OutputPin );*/
	//-----------------------------------------------------

	//Capture SET
	//DSCapture.ShowVideoDialog( cdVideoCapturePin ); //cdVideoCapture );
	//DSCapture.ShowAudioDialog( cdAudioCapturePin); //cdAudioCapture );
	//-----------------------------------------------------------------------------

	//Player SET
	DSPlayer.FileName = L"";
	//DSPlayer.AudioOutputPin.Connect( AFilter.InputPin );
	//DSPlayer.AudioOutputPin.Connect( AChannelMerger.InputPins[0] );
	
	//DSPlayer.AudioOutputPin.Connect( ASpectrum.InputPin );
//--------------------------------------------------------------------------------------
	DSPlayer.OutputPin.Connect( VFrameQueue.InputPin );//( VFormat.InputPin ); //Out to image
	DSPlayer.ClockSource = csExternal;
	DSPlayer.OnProgress.Set( this, &CStepConverter_VLDlg::OnVideoProgressEvent);
	DSPlayer.OnStop.Set( this, &CStepConverter_VLDlg::OnVideoStopEvent);
	DSPlayer.OnReset.Set( this, &CStepConverter_VLDlg::OnVideoResetEvent);
	//----------------------------------------------------------------------------

	//VFrameQueue
	VFrameQueue.Enabled = false;
	VFrameQueue.Min = 500;
	VFrameQueue.Max = 600;
	VFrameQueue.OutputPin.Connect( VFormat.InputPin );

	//VFormat SET
	VFormat.Format = vfRGBA32;
	VFormat.OutputPin.Connect( VFilter.InputPin );
	//----------------------------------------------------------------------------

	//VFilter SET
	VFilter.OnStart.Set( this, &CStepConverter_VLDlg::OnVFilter_Start );
	VFilter.OnStop.Set( this, &CStepConverter_VLDlg::OnVFilter_Stop );
	VFilter.OnProcessData.Set( this, &CStepConverter_VLDlg::OnVFilter_PD );
	VFilter.SynchronizeType = stNone; //stQueue;
	//------------------------------------------------
	//AFilter SET
	AFilter.OnStart.Set( this, &CStepConverter_VLDlg::OnAFilterAF_Start );
	AFilter.OnStop.Set( this, &CStepConverter_VLDlg::OnAFilter_Stop );
	AFilter.OnProcessData.Set( this, &CStepConverter_VLDlg::OnAFilterAF_PD );
	AFilter.SynchronizeType = stQueue;
	//------------------------------------------------

	//AChannelMerger SET
	//AChannelMerger.AudioFormat.Bits = 24;
	//AChannelMerger.AudioFormat.BufferSize = 2048;
	//AChannelMerger.AudioFormat.Channels = 1;
	//AChannelMerger.AudioFormat.SampleRate = 48000;
	//AChannelMerger.AudioFormat.Enabled = false; 	//<- Main option: As curr
	//AChannelMerger.OutputPin.Connect( ASpectrum.InputPin );
	//-------------------------------------------------

	//ASpectrum SET
	//ASpectrum.CountChannels = 1;
	//ASpectrum.ModedB = adbNone;							//db Power or Amplitude
	//ASpectrum.Normalization = fnNone;
	//ASpectrum.Order = 1024;
	//ASpectrum.SamplingWindowStep = 0;						//podozritelno
	//ASpectrum.Synchronize = false;
	//ASpectrum.OutputPins[0].Connect( ARealBuffer.InputPin );
	//----------------------------------------------------------------------

	//ARealBuffer SET
	ARealBuffer.SynchronizeType = stQueue;
	ARealBuffer.OutputPin.Disconnect();
	ARealBuffer.OnStart.Set(this, &CStepConverter_VLDlg::OnAFilter_Start );
	ARealBuffer.OnStop.Set(this, &CStepConverter_VLDlg::OnAFilter_Stop);
	ARealBuffer.OnProcessData.Set(this, &CStepConverter_VLDlg::OnAFilter_PD);
	//-----------------------------------------------------------------------

	PIXELFORMATDESCRIPTOR pfd;
	int         n;
	HGLRC       hrc;

	m_pDC = new CClientDC(&mgb_GLFrame);
	if (!bSetupPixelFormat())
	{
		MessageBoxW(L"Error setup Pixel Format");
		return FALSE;
	}

	n = GetPixelFormat(m_pDC->GetSafeHdc());
	DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	// проверяем состояние флага PFD_GENERIC_ACCELERATED
	if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED))
	{
     // аппаратное ускорение используется
     //bHardwareAccelerated = true;
	}
	else
	{
     // аппаратное ускорение неиспользуется
     //bHardwareAccelerated = false;
	}

	//CreateRGBPalette();

	hrc = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(), hrc);

	mgb_GLFrame.GetClientRect(&m_oldRect);
//	glClearDepth(1.0f);
//	glEnable(GL_DEPTH_TEST);

	glViewport(0,0,m_oldRect.Width(),m_oldRect.Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1.0, 0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return TRUE;
}

BOOL CStepConverter_VLDlg::bSetupPixelFormat()
{
		static PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
			1,                              // version number
			PFD_DRAW_TO_WINDOW |            // support window
			PFD_SUPPORT_OPENGL |          // support OpenGL
			PFD_DOUBLEBUFFER,             // double buffered
			PFD_TYPE_RGBA,                  // RGBA type
			24,                             // 24-bit color depth
			0, 0, 0, 0, 0, 0,               // color bits ignored
			0,                              // no alpha buffer
			0,                              // shift bit ignored
			0,                              // no accumulation buffer
			0, 0, 0, 0,                     // accum bits ignored
			32,                             // 32-bit z-buffer
			0,                              // no stencil buffer
			0,                              // no auxiliary buffer
			PFD_MAIN_PLANE,                 // main layer
			0,                              // reserved
			0, 0, 0                         // layer masks ignored
		};
		int pixelformat;
		// TODO: Add your specialized creation code here

		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;

		if ( (pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0 )
		{
			MessageBoxW(L"ChoosePixelFormat failed");
			return FALSE;
		}

		if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
		{
			MessageBoxW(L"SetPixelFormat failed");
			return FALSE;
		}

		return TRUE;
}

void CStepConverter_VLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		CRect rect;
		GetClientRect(&rect);
		dc.DrawIcon( (rect.Width() - GetSystemMetrics(SM_CXICON) + 1) >> 1 , (rect.Height() - GetSystemMetrics(SM_CYICON) + 1) >> 1, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
		DrawScene();
	}
}

HCURSOR CStepConverter_VLDlg::OnQueryDragIcon() {  return static_cast<HCURSOR>(m_hIcon); }

void CStepConverter_VLDlg::BC_Play()
{
	int i = mlb_Files.GetCurSel();
	if(i == -1) { MessageBox(L"Select file from list",L"Information",MB_ICONINFORMATION); return; } //Если не выбран файл из списка то на выход

	CString strfname;

	mlb_Files.GetText(i,strfname);
	if( GetFileAttributes(strfname) == -1 ) //Этот файл удалён
		  MessageBox(L"Этого файла уже не существует", L"Error!!!", MB_ICONERROR);

	if( SetupPath.IsEmpty() ) //Папка для выходящих данных не установлена.
	{
		MessageBox(L"Save ToFolder not setup!",L"Error", MB_ICONSTOP);
		return;
	}

	if( !IsMediaFile( strfname ) )  //Это медиа файл?
	{
		MessageBox(L"Файл не относится к медиаконтенту!",L"Error", MB_ICONSTOP);
		return;
	}
	
	sFileSelect = strfname;
	spWorkFolder = SetupPath;
	spMediaFile = strfname.Mid( strfname.ReverseFind(L'\\')+1, strfname.ReverseFind(L'.') - strfname.ReverseFind(L'\\') - 1);

	if( GetFileAttributes(spWorkFolder) == -1 ) //Если папки не существует
	{
		if( !CreateDirectory(spWorkFolder,NULL) ) //Если создать новую не удалось
		{
			MessageBox(L"Can't create folder!!!", L"Error", MB_ICONERROR);
			return;
		}
	}
	
	bHISTFILTER=(bool)mc_HistFilter.GetCheck();
	bSFLAKBLACK=(bool)mc_SimpleFlakBlack.GetCheck();

	DSPlayer.FileName = strfname;
	if( !DSPlayer.Open() )
	{
		MessageBox(L"Не удалось открыть файл (испорчен или нет подходящего кодека!)",L"Ошибка",MB_ICONERROR);
		return;
	}

	mc_HistFilter.EnableWindow(FALSE);
	mc_SimpleFlakBlack.EnableWindow(FALSE);
	me_CurFile.SetWindowText( strfname );

	iiVFramesOnSnap = ms_VFrameOnSnap.GetPos();
	iiAFramesOnSnap = ms_AFrameOnSnap.GetPos();
	if( iiVFramesOnSnap < 1 ) { iiVFramesOnSnap=1; ms_VFrameOnSnap.SetPos(1); }
	if( iiAFramesOnSnap < 1 ) { iiAFramesOnSnap=1; ms_AFrameOnSnap.SetPos(1); }
	if( iiVFramesOnSnap > 1000 ) { iiVFramesOnSnap=1000; ms_VFrameOnSnap.SetPos(1000); }
	if( iiAFramesOnSnap > 1000 ) { iiAFramesOnSnap=1000; ms_AFrameOnSnap.SetPos(1000); }
	CurVFrame = 1;
	CurAFrame = 1;

	//---------------------------
	mpb_Frame.SetState(2);
	mb_Play.EnableWindow(FALSE);
	mb_Pause.EnableWindow();
	mb_Stop.EnableWindow();
	me_VFrameOnSnap.EnableWindow(FALSE);
	me_AFrameOnSnap.EnableWindow(FALSE);
	ms_VFrameOnSnap.EnableWindow(FALSE);
	ms_AFrameOnSnap.EnableWindow(FALSE);

	me_ACodec.SetWindowText( (CTString)DSPlayer.AudioCodecName );
	me_VCodec.SetWindowText( (CTString)DSPlayer.VideoCodecName );

	//---------------------
	s_h = DSPlayer.Height;
	s_w = DSPlayer.Width;
	s_fps = DSPlayer.CanStepFrames;
	DSPlayer.Paused=false;
	DSPlayer.Start();

	//---------------------
	globalSCF.create( spWorkFolder + spMediaFile + L".txt" );
	//---------------------

	SetTimer(0x10ff10ff,1000,NULL);
	SetTimer(0x11111111,10,NULL);
}

void CStepConverter_VLDlg::BC_Pause()
{
	//mb_Pause.SetWindowText(  );
	CString butcapt;
	mb_Pause.GetWindowText(butcapt);
	if(butcapt.Compare(L"Pause") == 0)
	{
		mpb_Frame.SetState(3);
		mb_Pause.SetWindowText(L"Continue");
		KillTimer(0x11111111);
		DSPlayer.Pause();
	}
	else
	{
		mpb_Frame.SetState(2);
		mb_Pause.SetWindowText(L"Pause");
		DSPlayer.Resume();
		SetTimer(0x11111111,10,NULL);
	}
}

void CStepConverter_VLDlg::BC_Stop()
{
	bManualStop=true;
	
	mc_HistFilter.EnableWindow();
	mc_SimpleFlakBlack.EnableWindow();

	mb_Play.EnableWindow();
	mb_Pause.SetWindowText(L"Pause");
	mb_Pause.EnableWindow(FALSE);
	mb_Stop.EnableWindow(FALSE);
	me_VFrameOnSnap.EnableWindow();
	me_AFrameOnSnap.EnableWindow();
	ms_VFrameOnSnap.EnableWindow();
	ms_AFrameOnSnap.EnableWindow();

	KillTimer(0x11111111);
	KillTimer(0x10ff10ff);

	DSPlayer.OnStop.Clear();
	DSPlayer.Stop();
	DSPlayer.Close();
	DSPlayer.OnStop.Set( this, &CStepConverter_VLDlg::OnVideoStopEvent );

	//------------------
	globalSCF.close();
	//------------------
	
	bManualStop=false;
}

void CStepConverter_VLDlg::BC_Capture() {}

void CStepConverter_VLDlg::BC_Files()
{
	static TCHAR BASED_CODE szFilter[] = _T("MediaFiles|*.avi;*.mpeg;*.mpg;*.mkv;*.wmv;*.ogm;*.mp4;*.ts;*.flv;*.mov;*.m2ts|");

	CString fileName;
	CFileDialog fd(TRUE,L"*.*",NULL,6 | OFN_ALLOWMULTISELECT,szFilter,this);
	fd.GetOFN().lpstrFile = fileName.GetBuffer( (2048 * (MAX_PATH + 1)) + 1 );
	fd.GetOFN().nMaxFile = 2048;
	
	POSITION sp;
	if( fd.DoModal() == 1 )
	{
		sp = fd.GetStartPosition(); //cfd.GetPathName();
		while(sp!=NULL)
			mlb_Files.AddString( fd.GetNextPathName(sp) );
		if( mlb_Files.GetCount() != 0 ) mlb_Files.SetCurSel(0);
	}
	fileName.ReleaseBuffer();
}

void CStepConverter_VLDlg::BC_Folder()
{
	bi.hwndOwner = m_hWnd;
	LPCITEMIDLIST lpItemDList;
	lpItemDList=SHBrowseForFolderW(&bi);
	if(!lpItemDList)
	{
		//MessageBox(WindowHandle, L"Сорри не удалось открыть окошко выбора папки... (Переустановите Винду)", L"Мессага об ошибке", MB_ICONERROR);
		return;
	}
	// Достаем полный путь через
	wchar_t szPath[512];
	SHGetPathFromIDList(lpItemDList, szPath);
	int retval = MessageBoxW(L"Включить файлы во вложенных папках?", L"?", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);

	//mlb_Files.ResetContent();
	wcscat_s(szPath, L"\\");

	if(retval == IDYES)
		ListInheritFiles(CString(szPath), &mlb_Files);
	else if(retval == IDNO)		
		ListLocalFiles(CString(szPath), &mlb_Files);

	if( mlb_Files.GetCount() != 0 ) mlb_Files.SetCurSel(0);
}

void CStepConverter_VLDlg::BC_Close()
{
	//for(long i=0; i<mlb_Files.GetCount(); ++i)
	//	if(mlb_Files.GetSel(i)) { mlb_Files.DeleteString( i ); break; }
	int q = mlb_Files.GetCurSel();
	if( q != -1 )
	{
		mlb_Files.DeleteString(q);
		mlb_Files.SetCurSel(q);
	}
}

void CStepConverter_VLDlg::BC_CloseAll() { mlb_Files.ResetContent(); }
void CStepConverter_VLDlg::BC_Delete()
{
	CString delf;
	for(long i=0; i<mlb_Files.GetCount(); ++i)
		if(mlb_Files.GetSel(i))
		{
			mlb_Files.GetText(i, delf);
			if( GetFileAttributes( delf ) != -1 )
			{
				INT res = MessageBoxW( L"Вы уверены что хотите удалить файл?\n(Yes)-Удалить файл\n(No)-Удалить из списка\n(Cancel)-Отмена", L"???", MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON2 );
				if(res == IDYES) { DeleteFileW(delf); mlb_Files.DeleteString( i ); break; }
				else if(res == IDCANCEL) return;
			}
			mlb_Files.DeleteString( i );
			break;
		}
}

void CStepConverter_VLDlg::BC_DeleteAll()
{
 INT res = MessageBox( L"Вы уверены что хотите удалить все файлы?", L"???", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2 );
 if( res == IDYES )
 {
	CString delf;
	for(long i=0; i<mlb_Files.GetCount(); ++i)
	{
		mlb_Files.GetText(i, delf);
		if( GetFileAttributes( delf ) != -1 )
		  DeleteFileW(delf);
	}
	mlb_Files.ResetContent();
 }
}

void CStepConverter_VLDlg::BC_ToFolder()
{
	wchar_t szPath[512];
  //wcscpy( szPath, SetupPath.GetBuffer());
	bi.hwndOwner = m_hWnd;
	//bi.pszDisplayName = szPath;
	LPCITEMIDLIST lpItemDList;
	lpItemDList=SHBrowseForFolderW(&bi);
	if(!lpItemDList)
	{
		return;
	}
	SHGetPathFromIDList(lpItemDList, szPath);
	if( szPath[ wcslen(szPath) - 1 ] != L'\\' ) wcscat_s(szPath, L"\\");
	SetupPath = szPath;
	me_SetPath.SetWindowText( szPath );
}

void __stdcall CStepConverter_VLDlg::OnVideoProgressEvent( void *Sender, __int64 SartFrame, __int64 EndFrame, __int64 CurrenFrame )
{
	iProgressPos = (int)(CurrenFrame * 2147483647 / ( EndFrame - SartFrame ));
}

void __stdcall CStepConverter_VLDlg::OnVideoStopEvent( void *Sender )
{
	if( mc_Output.GetCheck() )
	{
	 int ooo = mlb_Files.GetCurSel();
	 if( mc_FileDeleteAfter.GetCheck() )
	 {
		 CString csdelf;
		 mlb_Files.GetText(ooo, csdelf);
		 mlb_Files.DeleteString( ooo );
		 DeleteFileW( csdelf );
	 }
	 else
		 ++ooo;

	 //--------------------
	 globalSCF.close();
	 //--------------------

	 if( ooo >= mlb_Files.GetCount() ) { MessageBeep(MB_ICONINFORMATION); return; }

	 mlb_Files.SetCurSel( ooo );

	 SetTimer( 0x33333333, 500, NULL ); //BC_PLAY
	}
	if( !bManualStop ) 
	 MessageBeep(0);
}

void __stdcall CStepConverter_VLDlg::OnVideoResetEvent( void *Sender ) {}

//VIDEO===================================================================================
void __stdcall CStepConverter_VLDlg::OnVFilter_PD(void *Sender, TVLCVideoBuffer InBuffer, TVLCVideoBuffer& OutBuffer, bool& SendOutputData)
{
 SendOutputData=false;
 register int blen = (int)InBuffer.GetSize();
 //const unsigned int* p = (const unsigned int*)InBuffer.Read();
 //const __m128i* p = (const __m128i*)InBuffer.Read();
 const unsigned char* p = (const unsigned char*)InBuffer.Read();

 if( (blen == 0) || (p == NULL) ) return;

//#pragma omp parallel for
 
  for(int i=0; i<blen; i+=4)
  {

  }
// globalSCF.writebase_scv((unsigned int*)wavbuf, blen/4 ,  0,0);

 if( CurVFrame >= iiVFramesOnSnap )
 {
  CurVFrame = 0;
	//DrawScene();
	//globalSCF.writebase_scv(NULL,0,0,0); //flush
 }
 ++CurVFrame;
 ++iiPerfMarkVideo;
}

void __stdcall CStepConverter_VLDlg::OnVFilter_Start(void *Sender, int &AWidth, int &AHeight, double SampleRate)
{
	AAHeight = AHeight;
	AAWidth = AWidth;

	//GLBVB = new TVLCVideoBuffer( AAWidth, AAHeight, vfRGBA32 );
	//GLBVB->Set( 0, 0 );
	CurVFrame=1;
	globalSCF.reset();
}

void __stdcall CStepConverter_VLDlg::OnVFilter_Stop(void *Sender)
{
	if(CurVFrame > 1)
	{
		//globalSCF.writebase_scv(NULL,0,0,0); //flush
	}
	//----------------------------------------------------------------------------------
	CurVFrame=1;
}
//---------===========================================---------------------===============

//AUDIO===================================================================================
void __stdcall CStepConverter_VLDlg::OnAFilterAF_PD(void *Sender, TALCAudioBuffer InBuffer, TALCAudioBuffer& OutBuffer, bool& SendOutputData)
{
 SendOutputData=false;
 register int blen = (int)InBuffer.GetSize();
 const unsigned char* p = (const unsigned char*)InBuffer.Read();
 //register int blen = (int)InBuffer.GetSize() / 16;
 //const __m128i* p = (const __m128i*)InBuffer.Read();

 if( CurAFrame >= iiAFramesOnSnap )
 { 
	 CurAFrame = 0; 
	 //globalSCF.writebase_sca(NULL,0);  
 }
 ++CurAFrame; ++iiPerfMarkAudio;
}

void __stdcall CStepConverter_VLDlg::OnAFilter_PD(void *Sender, TSLCRealBuffer InBuffer, TSLCRealBuffer &OutBuffer, bool &SendOutputData)
{
	SendOutputData=false;
	register unsigned int blen = InBuffer.GetSize();
  const double* p = InBuffer.Read();

	if( (blen == 0) || (p == NULL) ) return;

	if( CurAFrame >= iiAFramesOnSnap )
	{
	 CurAFrame = 1;
	}
	++CurAFrame;
  ++iiPerfMarkAudio;
}

void __stdcall CStepConverter_VLDlg::OnAFilter_Start(void *Sender, double SampleRate)
{
	CurAFrame=1;
}

void __stdcall CStepConverter_VLDlg::OnAFilterAF_Start( void *Sender )
{
  CurAFrame=1;
}

void __stdcall CStepConverter_VLDlg::OnAFilter_Stop(void *Sender)
{
	if(CurAFrame > 1)
	{
		//globalSCF.writebase_sca(NULL,0);
	}
	CurAFrame=1;
}
//------==========================---------------------------------------=================

void CStepConverter_VLDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == 0x10ff10ff )
	{
		//MSG gMSG;
		//PeekMessage(&gMSG,m_hWnd,0,0,PM_REMOVE);
		mpb_Frame.SetPos( iProgressPos );
		CString cs;			
		register int pp = (iProgressPos - iProgressLast);
		if(pp == 0) cs = L"Elapsed: inf";
		else { register int tt = (2147483647-iProgressPos) / pp; cs.Format(L"Elapsed: %i min : %i sec", (tt / 60), tt % 60); }
		me_ElapseTime.SetWindowText( cs );
		iProgressLast = iProgressPos;
		//cs.Format(L"AC=%i VC=%i MF=%i", iiPerfMarkAudio, iiPerfMarkVideo, ((__int64)DSPlayer.FramesCount - (__int64)DSPlayer.CurrentFrame));
		cs.Format(L"C=%i/%i (%i)", iiPerfMarkVideo, ((__int64)DSPlayer.FramesCount - (__int64)DSPlayer.CurrentFrame), VFrameQueue.Count);
		me_PerfMark.SetWindowText( cs );

		iiPerfMarkAudio=0;
	}
	else if( nIDEvent == 0x11111111 )
	{
		__int64 difframe = (__int64)DSPlayer.FramesCount - (__int64)DSPlayer.CurrentFrame;

		if( difframe < 50i64 ) { KillTimer(0x11111111); KillTimer(0x10ff10ff); SetTimer(0x22222222,100,NULL); return; }

		short pp=25;
		while(--pp > 0i16)
		{
			DSPlayer.Pump();
		}
		return;
	}
	else if( nIDEvent == 0x22222222 ) //Auto Stop
	{
    KillTimer(0x22222222);
		KillTimer(0x11111111);
		KillTimer(0x10ff10ff);
		DSPlayer.Stop();
	}
	else if( nIDEvent == 0x33333333 )
	{
		KillTimer(0x33333333);
		BC_Play();
	}
}

void CStepConverter_VLDlg::OnDropFiles(HDROP hDropInfo)
{
	int nFiles = DragQueryFileW(hDropInfo, -1, NULL, NULL);
	wchar_t path[256];
	for (int i = 0; i < nFiles; i++)
	{
		DragQueryFileW(hDropInfo, i, path, sizeof(path));
		if( GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY )
		{
			INT retcode = MessageBoxW(L"Включить вложенные папки?", L"?", MB_ICONQUESTION | MB_YESNO);
			if( retcode == IDYES )
				ListInheritFiles(CString(path) + L"\\", &mlb_Files);
			else if( retcode == IDNO )
				ListLocalFiles(CString(path) + L"\\", &mlb_Files);
		}
		else if( IsMediaFile( CString(path) ) )
			   mlb_Files.AddString( path );
	}
	DragFinish(hDropInfo);
	CDialog::OnDropFiles(hDropInfo);
}

BOOL CStepConverter_VLDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	MessageBoxW(L"Программа конверсии медиа файлов в stepcon данные\n\t\tРазработка - Alexander \'AlexB\' Bragin \n(с)2010",L"Help",MB_ICONQUESTION);
	return FALSE;
}

void CStepConverter_VLDlg::OnClose()
{
	//CleanupMem(EXIT_SUCCESS);
	//CleanupKernel(EXIT_SUCCESS);
	//Cleanup(EXIT_SUCCESS);
	//DSCapture.Enabled = false;
	DSPlayer.Enabled = false;
	CDialog::OnClose();
}

void CStepConverter_VLDlg::SLIDE_VFS(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	if( ((int)pNMTPC->dwPos < ms_VFrameOnSnap.GetRangeMin()) || ((int)pNMTPC->dwPos > ms_VFrameOnSnap.GetRangeMax()) ) return;
	CString sd;
	sd.Format(L"%i", pNMTPC->dwPos);
	me_VFrameOnSnap.SetWindowText(sd);
	*pResult = 0;
}

void CStepConverter_VLDlg::SLIDE_AFS(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	if( ((int)pNMTPC->dwPos < ms_AFrameOnSnap.GetRangeMin()) || ((int)pNMTPC->dwPos > ms_AFrameOnSnap.GetRangeMax()) ) return;

	CString asd;
	asd.Format(L"%i", pNMTPC->dwPos);
	me_AFrameOnSnap.SetWindowText(asd);
	*pResult = 0;
}

void CStepConverter_VLDlg::OnDestroy()
{
	CDialog::OnDestroy();

	HGLRC   hrc;
	hrc = wglGetCurrentContext();
	wglMakeCurrent(NULL,  NULL);

	if (hrc)
		wglDeleteContext(hrc);

	//ALFWShutdownOpenAL();
	//ALFWShutdown();
	VCL_Shutdown();
}

void CStepConverter_VLDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect glre;
	mgb_GLFrame.GetClientRect(&glre);
	if(glre.Width() > 0)
	{
		glViewport(0, 0, cx=glre.Width(),cy=glre.Height());

		if((m_oldRect.right > cx) || (m_oldRect.bottom > cy))
			mgb_GLFrame.RedrawWindow();

		m_oldRect = glre;

		glViewport(0,0,m_oldRect.Width(),m_oldRect.Height());
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 1.0, 0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}

void CStepConverter_VLDlg::DrawScene(void)
{
	if( !_bittest( &glb_bitmask, 0 ) )
		return;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glColor3f(1.0f, 1.0f, 1.0f);
	//glBegin(GL_QUADS);
	//glBegin( GL_QUAD_STRIP );
	/*glLineWidth(2.0f);
	if( _bittest(&glb_bitmask,1) && (glVideoOrigin != NULL) )
	{
	 glBegin( GL_LINE_STRIP );
	 glColor3f(1.0f,0.0f,0.0f);
	 for(int f=0; f<256; ++f)
		glVertex2f((float)f, glVideoOrigin[f]);
	 glEnd();
	}

	if(_bittest(&glb_bitmask,2) && (glAudioGist != NULL))
	{
   glBegin( GL_LINE_STRIP );	
	 glColor3f(0.0f,0.0f,1.0f);
	 for(int f=0; f<256; ++f)
		glVertex2f((float)f, glAudioGist[f]);
	 glEnd();
	}

	//------------------------------------------------------------------
	if( bHISTFILTER && _bittest(&glb_bitmask,3) && (glF06 != NULL) )
	{
		glBegin( GL_LINE_STRIP );	//Yellow F06
		glColor3f(1.0f,1.0f,0.0f);
		for(int f=0; f<256; ++f)
			glVertex2f((float)f, glF06[f]);
		glEnd();
	}*/
	//------------------------------------------------------------------

	/*if( OutCache.buf != NULL )
	{
	 glBegin(GL_POINTS);
	 for( int h=0; h<AAHeight; ++h)
	 for( int w=0; w<AAWidth; ++w)
	 {
		 unsigned char uctmp = OutCache.buf[ h*AAWidth + w ];
		 glColor3ub( uctmp,uctmp,uctmp );
		 glVertex2f( (float)w / (float)AAWidth, (float)h / (float)AAHeight );
	 }
	 glEnd();
	}*/

	glBegin(GL_QUADS);
	glColor3f(0.0f,0.0f,0.0f); glVertex2f(0.0f,		0.0f);
	glColor3f(1.0f,1.0f,1.0f); glVertex2f(0.0f,		1.0f);
	glColor3f(1.0f,1.0f,1.0f); glVertex2f(1.0f,	  1.0f);
	glColor3f(0.0f,0.0f,0.0f); glVertex2f(1.0f,	  0.0f);
	glEnd();

	//float m_XMousePos = (float)(2.0f*hl.x)/(float)(pp.right)-1.0f;
	//float m_YMousePos = (float)(2.0f*hl.y)/(float)(pp.bottom)-1.0f;
	glFlush();

	SwapBuffers(wglGetCurrentDC());
}

void CStepConverter_VLDlg::On_TGraphics()
{
	// TODO: добавьте свой код обработчика уведомлений
	BOOLEAN bret = _bittestandcomplement( &glb_bitmask, 0 );
	mb_tvideo.EnableWindow( !bret );
	mb_taudio.EnableWindow( !bret );
	mb_thist.EnableWindow( !bret );

	mb_tgraphics.SetWindowText( (!bret) ? L"*" : L" " );
}

void CStepConverter_VLDlg::On_TVideo()
{
	BOOLEAN bret = _bittestandcomplement( &glb_bitmask, 1 );
	mb_tvideo.SetWindowText( (!bret) ? L"V" : L" " );
}

void CStepConverter_VLDlg::On_TAudio()
{
	BOOLEAN bret = _bittestandcomplement( &glb_bitmask, 2 );
	mb_taudio.SetWindowText( (!bret) ? L"A" : L" " );
}

void CStepConverter_VLDlg::On_THist()
{
	BOOLEAN bret = _bittestandcomplement( &glb_bitmask, 3 );
	mb_thist.SetWindowText( (!bret) ? L"H" : L" " );
}