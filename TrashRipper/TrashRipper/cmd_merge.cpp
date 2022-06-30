#include "TrashRipper.hpp"

#include "classChemicalBurner.hpp"
#include "classLayersResources.hpp"
//#include "classHashAccumulator.hpp"

int appTR::MergeResults(const bool bShowHelp)
{
	if (!appTR::GChemicalBurner)	appTR::GChemicalBurner	= std::make_unique<appTR::ChemicalBurner>();
	if (!appTR::GLayerRes)			appTR::GLayerRes		= std::make_unique<appTR::LayersResources>();
	LoadResults(OutputDirectory);

	std::wstring mergedir;
	std::wcerr << "Input directory from other files merge to RESPATH<" << OutputDirectory << "> envvar.\n";
	std::wcin >> mergedir;
	if ((mergedir.size() > 3) && (mergedir.back() != L'\\' && mergedir.back() != L'/'))
		mergedir += L'\\';

	//GHI_RHash.merge(mergedir);
	//GHI_DRIB.merge(mergedir);
	//GHI_AESMMO2.merge(mergedir);
	//GHI_AESHIROSE.merge(mergedir);

	{
		appTR::ChemicalBurner CB;
		if (CB.load(mergedir + L"GChemicalBurner.bin"))	appTR::GChemicalBurner->merge(CB);
	}
	{
		appTR::LayersResources LR;
		if (LR.load(mergedir + L"GLayerRes.bin"))		appTR::GLayerRes->merge(LR);
	}
	SaveResults(OutputDirectory);
	return 0;
}