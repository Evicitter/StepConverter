//Last update time : 03.01.2013 15:45:08
#include <vcl.h>
#pragma hdrstop
#include "NetModule.h"
#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TNModule *NModule;
//---------------------------------------------------------------------------
__fastcall TNModule::TNModule(TComponent* Owner)
	: TDataModule(Owner)
{
}
//---------------------------------------------------------------------------