//Last update time : 24.07.2013 07:15:18

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LPComponent.h"
#include "SLCommonFilter.h"
#include "SLCommonGen.h"
#include "VLColorTwist.h"
#include "VLCommonFilter.h"
#include "VLCommonGen.h"
#include "VLJaehneGen.h"
#include "LPControl.h"
#include "SLControlCollection.h"
#include "VLCommonDisplay.h"
#include "VLImageDisplay.h"
#include "VLDSImageDisplay.h"
#include "LPControlDrawLayers.h"
#include "SLScope.h"
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMForm : public TForm
{
__published:	// IDE-managed Components
	TButton *btn_play;
	TVLDSImageDisplay *VLDSImageDisplay1;
	TSLScope *SLScope1;
	TProgressBar *PB_AV;
	TGroupBox *GroupBox1;
	TListBox *FileList;
	TButton *btn_stop;
	TGroupBox *GroupBox2;
	TBitBtn *bb_deletelist;
	TBitBtn *bb_deletefile;
	TButton *btn_pause;
	TBitBtn *bb_clearlist;
	TGroupBox *GroupBox3;
	TEdit *linestatus1;
	TTimer *TimerUpdateStatus;
	TGroupBox *GroupBox4;
	TEdit *OutputFolder;
	TButton *ButtonOutFolder;
	TEdit *linestatus2;
	TPanel *Panel1;
	TEdit *linestatus3;
	TGroupBox *GroupBox5;
	TSpeedButton *SpeedButton1;
	TMemo *Log;
	TSpeedButton *SpeedButton4;
	TSpeedButton *SpeedButton5;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	void __fastcall btn_Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btn_ListClick(TObject *Sender);
	void __fastcall GlobalTimer(TObject *Sender);
	void __fastcall ButtonOutFolderClick(TObject *Sender);
	void __fastcall OptionsClick(TObject *Sender);

private:	// User declarations
	void __fastcall WmDropFiles(TWMDropFiles& Message);
public:		// User declarations
	__fastcall TMForm(TComponent* Owner);
	BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_DROPFILES,TWMDropFiles,WmDropFiles)
	END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TMForm *MForm;
//---------------------------------------------------------------------------

#include <vector>

struct mm16 { unsigned char val[16]; operator __int64() const { __int64 x; memcpy(&x,val,8); return x; }; };
struct mm32 { unsigned char val[32]; };

struct MMvectors
{
 std::vector<mm16> vRH;
 std::vector<__int64> vRH64;
 std::vector<mm32> vADM;
 std::vector<mm32> vAHI;
};

extern MMvectors mgrbcache; //not use
extern MMvectors mframecache;
extern MMvectors msamplecache;
extern void __fastcall GroupHashWrite(void* buffer,const int size, void* hashfile, MMvectors* mcache, bool bFinal=false);
#endif