#include "TrashRipper.hpp"

#include "classChemicalBurner.hpp"
#include "classLayersResources.hpp"
#include "classHashAccumulator.hpp"

int appTR::ShowDump(const bool bShowHelp)
{
	if (!GChemicalBurner)	GChemicalBurner = std::make_unique<appTR::ChemicalBurner>();
	if (!GLayerRes)			GLayerRes		= std::make_unique<appTR::LayersResources>();
	LoadResults(OutputDirectory);
	std::wcout << "dump : GChemicalBurner:\n"; appTR::GChemicalBurner->dump(std::wcout);
	std::wcout << std::endl;
	std::wcout << "dump : GLayerRes    :\n"; appTR::GLayerRes->dump(std::wcout);
	std::wcout << std::endl;

	std::wcout << "dump : GHI_RHash     :";	appTR::GHI_RHash.dump(OutputDirectory, std::wcout);		std::wcout << std::endl;
	std::wcout << "dump : GHI_DRIB      :";	appTR::GHI_DRIB.dump(OutputDirectory, std::wcout);		std::wcout << std::endl;
	std::wcout << "dump : GHI_AESDM     :";	appTR::GHI_AESDM.dump(OutputDirectory, std::wcout);		std::wcout << std::endl;
	std::wcout << "dump : GHI_AESMMO2   :";	appTR::GHI_AESMMO2.dump(OutputDirectory, std::wcout);	std::wcout << std::endl;
	std::wcout << "dump : GHI_AESHIROSE :";	appTR::GHI_AESHIROSE.dump(OutputDirectory, std::wcout);	std::wcout << std::endl;
	std::wcout << std::endl;
	return 0;
}