//Last update time : 06.03.2013 18:46:22

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("AVGraph.cpp", AVG); /* TDataModule: File Type */
USEFORM("MainForm.cpp", MForm);
USEFORM("NetModule.cpp", NModule); /* TDataModule: File Type */
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TMForm), &MForm);
		Application->CreateForm(__classid(TAVG), &AVG);
		Application->CreateForm(__classid(TNModule), &NModule);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
