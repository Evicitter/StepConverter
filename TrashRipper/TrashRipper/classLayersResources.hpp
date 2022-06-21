#pragma once

#include <iostream>
#include <string>
#include <memory>

namespace appTR {

class LayersResources
{
public:
    static const size_t maxels	 = 256u * 256u * 256u;
    static const size_t maxbytes = maxels * 8u;			//128 MB

    LayersResources();
    ~LayersResources();

	void SaturateSpin(const unsigned char* res, const size_t nsize);

    void merge(const LayersResources& other);
    void dump(std::wostream& out) const;
    bool load(std::wstring& _filename);
    bool save(std::wstring& _filename) const;
 
	bool layers_export(const std::wstring& outname) const;

private:
    unsigned long long *hR, *hG, *hB, *hH, *hS, *hV; //256 * 256 * 256 * 8 = 128 MB * 6 = 768 MB
};


extern std::unique_ptr<LayersResources>	GLayerRes;

}