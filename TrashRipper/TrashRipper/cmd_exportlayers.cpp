#include "TrashRipper.hpp"

#include "classLayersResources.hpp"

int appTR::ExportLayers(const bool bShowHelp)
{
	if (!appTR::GLayerRes)			appTR::GLayerRes = std::make_unique<appTR::LayersResources>();

	if (appTR::GLayerRes && !appTR::GLayerRes->load(OutputDirectory + L"GLayerRes.bin"))
	{
		std::wcerr << "File \'GLayerRes.bin\' don\'t load.\n";
		return 1;
	}
	appTR::GLayerRes->layers_export(OutputDirectory);
	return 0;
}