//Last update time : 03.01.2013 15:44:50

#ifndef NetModuleH
#define NetModuleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TNModule : public TDataModule
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
	__fastcall TNModule(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TNModule *NModule;
//---------------------------------------------------------------------------
#endif